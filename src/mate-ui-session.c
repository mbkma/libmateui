/*
 * mate-ui-session.c - Session management helpers for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "config.h"
#include "mate-ui-session.h"

#include <gio/gio.h>

#ifdef HAVE_X11
#include <gdk/gdkx.h>
#ifdef HAVE_XSS
#include <X11/extensions/scrnsaver.h>
#endif
#endif

struct _MateUiSessionInhibitor
{
    GtkApplication     *app;
    guint               cookie;
    MateUiInhibitFlags  flags;
    gboolean            is_dbus;
    guint               dbus_cookie;
};

/* Session manager D-Bus interface */
#define SM_DBUS_NAME      "org.gnome.SessionManager"
#define SM_DBUS_PATH      "/org/gnome/SessionManager"
#define SM_DBUS_INTERFACE "org.gnome.SessionManager"

static GDBusProxy *
get_session_manager_proxy(void)
{
    static GDBusProxy *proxy = NULL;

    if (proxy == NULL)
    {
        GError *error = NULL;
        proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,
                                               G_DBUS_PROXY_FLAGS_NONE,
                                               NULL,
                                               SM_DBUS_NAME,
                                               SM_DBUS_PATH,
                                               SM_DBUS_INTERFACE,
                                               NULL,
                                               &error);
        if (error != NULL)
        {
            g_warning("Failed to connect to session manager: %s", error->message);
            g_error_free(error);
        }
    }

    return proxy;
}

/* Convert our flags to GNOME SessionManager flags */
static guint
convert_inhibit_flags(MateUiInhibitFlags flags)
{
    guint sm_flags = 0;

    if (flags & MATE_UI_INHIBIT_LOGOUT)
        sm_flags |= 1;  /* GSM_INHIBITOR_FLAG_LOGOUT */
    if (flags & MATE_UI_INHIBIT_USER_SWITCH)
        sm_flags |= 2;  /* GSM_INHIBITOR_FLAG_SWITCH_USER */
    if (flags & MATE_UI_INHIBIT_SUSPEND)
        sm_flags |= 4;  /* GSM_INHIBITOR_FLAG_SUSPEND */
    if (flags & MATE_UI_INHIBIT_IDLE)
        sm_flags |= 8;  /* GSM_INHIBITOR_FLAG_IDLE */

    return sm_flags;
}

/**
 * mate_ui_session_inhibit:
 * @app: (nullable): A #GtkApplication or %NULL
 * @window: (nullable): A #GtkWindow or %NULL
 * @flags: What to inhibit
 * @reason: Human-readable reason for the inhibition
 *
 * Inhibits session actions.
 *
 * Returns: (transfer full) (nullable): An inhibitor handle or %NULL on failure
 */
MateUiSessionInhibitor *
mate_ui_session_inhibit(GtkApplication     *app,
                         GtkWindow          *window,
                         MateUiInhibitFlags  flags,
                         const gchar        *reason)
{
    g_return_val_if_fail(reason != NULL, NULL);

    MateUiSessionInhibitor *inhibitor = g_new0(MateUiSessionInhibitor, 1);
    inhibitor->flags = flags;

    /* Try GTK application inhibit first */
    if (app != NULL && GTK_IS_APPLICATION(app))
    {
        GtkApplicationInhibitFlags gtk_flags = 0;

        if (flags & MATE_UI_INHIBIT_LOGOUT)
            gtk_flags |= GTK_APPLICATION_INHIBIT_LOGOUT;
        if (flags & MATE_UI_INHIBIT_USER_SWITCH)
            gtk_flags |= GTK_APPLICATION_INHIBIT_SWITCH;
        if (flags & MATE_UI_INHIBIT_SUSPEND)
            gtk_flags |= GTK_APPLICATION_INHIBIT_SUSPEND;
        if (flags & MATE_UI_INHIBIT_IDLE)
            gtk_flags |= GTK_APPLICATION_INHIBIT_IDLE;

        inhibitor->cookie = gtk_application_inhibit(app, window, gtk_flags, reason);
        if (inhibitor->cookie != 0)
        {
            inhibitor->app = g_object_ref(app);
            inhibitor->is_dbus = FALSE;
            return inhibitor;
        }
    }

    /* Fall back to D-Bus session manager */
    GDBusProxy *proxy = get_session_manager_proxy();
    if (proxy != NULL)
    {
        const gchar *app_id = app ? g_application_get_application_id(G_APPLICATION(app)) : "mate-application";
        guint32 toplevel_xid = 0;

#ifdef HAVE_X11
        if (window != NULL && gtk_widget_get_realized(GTK_WIDGET(window)))
        {
            GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
            if (GDK_IS_X11_WINDOW(gdk_window))
            {
                toplevel_xid = GDK_WINDOW_XID(gdk_window);
            }
        }
#endif

        GError *error = NULL;
        GVariant *result = g_dbus_proxy_call_sync(proxy,
                                                   "Inhibit",
                                                   g_variant_new("(susu)",
                                                                  app_id,
                                                                  toplevel_xid,
                                                                  reason,
                                                                  convert_inhibit_flags(flags)),
                                                   G_DBUS_CALL_FLAGS_NONE,
                                                   -1,
                                                   NULL,
                                                   &error);

        if (result != NULL)
        {
            g_variant_get(result, "(u)", &inhibitor->dbus_cookie);
            g_variant_unref(result);
            inhibitor->is_dbus = TRUE;
            return inhibitor;
        }

        if (error != NULL)
        {
            g_warning("Failed to inhibit via session manager: %s", error->message);
            g_error_free(error);
        }
    }

    g_free(inhibitor);
    return NULL;
}

/**
 * mate_ui_session_uninhibit:
 * @inhibitor: An inhibitor returned by mate_ui_session_inhibit()
 *
 * Releases a session inhibitor.
 */
void
mate_ui_session_uninhibit(MateUiSessionInhibitor *inhibitor)
{
    if (inhibitor == NULL)
        return;

    if (!inhibitor->is_dbus && inhibitor->app != NULL)
    {
        gtk_application_uninhibit(inhibitor->app, inhibitor->cookie);
        g_object_unref(inhibitor->app);
    }
    else if (inhibitor->is_dbus)
    {
        GDBusProxy *proxy = get_session_manager_proxy();
        if (proxy != NULL)
        {
            GError *error = NULL;
            g_dbus_proxy_call_sync(proxy,
                                    "Uninhibit",
                                    g_variant_new("(u)", inhibitor->dbus_cookie),
                                    G_DBUS_CALL_FLAGS_NONE,
                                    -1,
                                    NULL,
                                    &error);

            if (error != NULL)
            {
                g_warning("Failed to uninhibit: %s", error->message);
                g_error_free(error);
            }
        }
    }

    g_free(inhibitor);
}

/**
 * mate_ui_session_is_inhibited:
 * @flags: Flags to check
 *
 * Checks if any session actions are currently inhibited.
 *
 * Returns: %TRUE if inhibited
 */
gboolean
mate_ui_session_is_inhibited(MateUiInhibitFlags flags)
{
    GDBusProxy *proxy = get_session_manager_proxy();
    if (proxy == NULL)
        return FALSE;

    GError *error = NULL;
    GVariant *result = g_dbus_proxy_call_sync(proxy,
                                               "IsInhibited",
                                               g_variant_new("(u)", convert_inhibit_flags(flags)),
                                               G_DBUS_CALL_FLAGS_NONE,
                                               -1,
                                               NULL,
                                               &error);

    if (result != NULL)
    {
        gboolean inhibited;
        g_variant_get(result, "(b)", &inhibited);
        g_variant_unref(result);
        return inhibited;
    }

    if (error != NULL)
    {
        g_warning("Failed to check inhibit status: %s", error->message);
        g_error_free(error);
    }

    return FALSE;
}

/**
 * mate_ui_session_register:
 * @app: A #GtkApplication
 * @client_id: (nullable): Session client ID or %NULL
 *
 * Registers the application with the session manager.
 *
 * Returns: %TRUE on success
 */
gboolean
mate_ui_session_register(GtkApplication *app,
                          const gchar    *client_id)
{
    g_return_val_if_fail(GTK_IS_APPLICATION(app), FALSE);

    GDBusProxy *proxy = get_session_manager_proxy();
    if (proxy == NULL)
        return FALSE;

    const gchar *app_id = g_application_get_application_id(G_APPLICATION(app));
    if (app_id == NULL)
        app_id = "mate-application";

    GError *error = NULL;
    GVariant *result = g_dbus_proxy_call_sync(proxy,
                                               "RegisterClient",
                                               g_variant_new("(ss)",
                                                              app_id,
                                                              client_id ? client_id : ""),
                                               G_DBUS_CALL_FLAGS_NONE,
                                               -1,
                                               NULL,
                                               &error);

    if (result != NULL)
    {
        g_variant_unref(result);
        return TRUE;
    }

    if (error != NULL)
    {
        g_warning("Failed to register with session manager: %s", error->message);
        g_error_free(error);
    }

    return FALSE;
}

/**
 * mate_ui_session_set_restart_command:
 * @app: A #GtkApplication
 * @argc: Number of arguments
 * @argv: Argument vector
 *
 * Sets the command to restart this application.
 */
void
mate_ui_session_set_restart_command(GtkApplication  *app,
                                     gint             argc G_GNUC_UNUSED,
                                     const gchar    **argv)
{
    g_return_if_fail(GTK_IS_APPLICATION(app));
    g_return_if_fail(argv != NULL);

    /* Store as application data for session save */
    gchar **argv_copy = g_strdupv((gchar **)argv);
    g_object_set_data_full(G_OBJECT(app), "mate-ui-restart-command",
                            argv_copy, (GDestroyNotify)g_strfreev);
}

/**
 * mate_ui_session_request_save:
 * @app: A #GtkApplication
 *
 * Requests the session to save state.
 *
 * Returns: %TRUE if request was accepted
 */
gboolean
mate_ui_session_request_save(GtkApplication *app)
{
    g_return_val_if_fail(GTK_IS_APPLICATION(app), FALSE);

    GDBusProxy *proxy = get_session_manager_proxy();
    if (proxy == NULL)
        return FALSE;

    GError *error = NULL;
    GVariant *result = g_dbus_proxy_call_sync(proxy,
                                               "RequestSave",
                                               NULL,
                                               G_DBUS_CALL_FLAGS_NONE,
                                               -1,
                                               NULL,
                                               &error);

    if (result != NULL)
    {
        g_variant_unref(result);
        return TRUE;
    }

    if (error != NULL)
    {
        g_warning("Failed to request session save: %s", error->message);
        g_error_free(error);
    }

    return FALSE;
}

/**
 * mate_ui_session_request_logout:
 * @prompt: Whether to prompt the user
 *
 * Requests the session to log out.
 */
void
mate_ui_session_request_logout(gboolean prompt)
{
    GDBusProxy *proxy = get_session_manager_proxy();
    if (proxy == NULL)
        return;

    GError *error = NULL;
    GVariant *result = g_dbus_proxy_call_sync(proxy,
                                               "Logout",
                                               g_variant_new("(u)", prompt ? 0 : 1),
                                               G_DBUS_CALL_FLAGS_NONE,
                                               -1,
                                               NULL,
                                               &error);

    if (result != NULL)
        g_variant_unref(result);

    if (error != NULL)
    {
        g_warning("Failed to request logout: %s", error->message);
        g_error_free(error);
    }
}

/**
 * mate_ui_session_request_shutdown:
 * @prompt: Whether to prompt the user
 *
 * Requests the system to shut down.
 */
void
mate_ui_session_request_shutdown(gboolean prompt G_GNUC_UNUSED)
{
    GDBusProxy *proxy = get_session_manager_proxy();
    if (proxy == NULL)
        return;

    GError *error = NULL;
    GVariant *result = g_dbus_proxy_call_sync(proxy,
                                               "Shutdown",
                                               NULL,
                                               G_DBUS_CALL_FLAGS_NONE,
                                               -1,
                                               NULL,
                                               &error);

    if (result != NULL)
        g_variant_unref(result);

    if (error != NULL)
    {
        g_warning("Failed to request shutdown: %s", error->message);
        g_error_free(error);
    }
}

/**
 * mate_ui_session_request_reboot:
 * @prompt: Whether to prompt the user
 *
 * Requests the system to reboot.
 */
void
mate_ui_session_request_reboot(gboolean prompt G_GNUC_UNUSED)
{
    GDBusProxy *proxy = get_session_manager_proxy();
    if (proxy == NULL)
        return;

    GError *error = NULL;
    GVariant *result = g_dbus_proxy_call_sync(proxy,
                                               "Reboot",
                                               NULL,
                                               G_DBUS_CALL_FLAGS_NONE,
                                               -1,
                                               NULL,
                                               &error);

    if (result != NULL)
        g_variant_unref(result);

    if (error != NULL)
    {
        g_warning("Failed to request reboot: %s", error->message);
        g_error_free(error);
    }
}

/* Save callback data */
typedef struct
{
    MateUiSessionSaveCallback callback;
    gpointer                  user_data;
    GDestroyNotify            destroy;
} SaveCallbackData;

static void
save_callback_data_free(gpointer data)
{
    SaveCallbackData *d = data;
    if (d->destroy != NULL)
        d->destroy(d->user_data);
    g_free(d);
}

/**
 * mate_ui_session_set_save_callback:
 * @app: A #GtkApplication
 * @callback: Callback for save requests
 * @user_data: User data for callback
 * @destroy: (nullable): Destroy notify for user data
 *
 * Sets a callback to be called when the session manager requests
 * the application to save its state.
 */
void
mate_ui_session_set_save_callback(GtkApplication            *app,
                                   MateUiSessionSaveCallback  callback,
                                   gpointer                   user_data,
                                   GDestroyNotify             destroy)
{
    g_return_if_fail(GTK_IS_APPLICATION(app));

    SaveCallbackData *data = g_new0(SaveCallbackData, 1);
    data->callback = callback;
    data->user_data = user_data;
    data->destroy = destroy;

    g_object_set_data_full(G_OBJECT(app), "mate-ui-save-callback",
                            data, save_callback_data_free);
}

/**
 * mate_ui_session_get_idle_time:
 *
 * Gets the current idle time in milliseconds.
 *
 * Returns: Idle time in milliseconds
 */
guint64
mate_ui_session_get_idle_time(void)
{
#if defined(HAVE_X11) && defined(HAVE_XSS)
    GdkDisplay *display = gdk_display_get_default();
    if (GDK_IS_X11_DISPLAY(display))
    {
        Display *xdisplay = GDK_DISPLAY_XDISPLAY(display);
        int event_base, error_base;

        if (XScreenSaverQueryExtension(xdisplay, &event_base, &error_base))
        {
            XScreenSaverInfo *info = XScreenSaverAllocInfo();
            if (info != NULL)
            {
                Window root = DefaultRootWindow(xdisplay);
                XScreenSaverQueryInfo(xdisplay, root, info);
                guint64 idle = info->idle;
                XFree(info);
                return idle;
            }
        }
    }
#endif

    /* Fallback: return 0 (not idle) */
    return 0;
}

/* Idle callback data */
typedef struct
{
    guint64        threshold;
    GCallback      callback;
    gpointer       user_data;
    GDestroyNotify destroy;
    guint          source_id;
} IdleCallbackData;

static gboolean
idle_check_callback(gpointer user_data)
{
    IdleCallbackData *data = user_data;
    guint64 idle_time = mate_ui_session_get_idle_time();

    if (idle_time >= data->threshold)
    {
        void (*cb)(gpointer) = (void (*)(gpointer))data->callback;
        cb(data->user_data);
    }

    return G_SOURCE_CONTINUE;
}

static void
idle_callback_data_free(gpointer data)
{
    IdleCallbackData *d = data;
    if (d->destroy != NULL)
        d->destroy(d->user_data);
    g_free(d);
}

/**
 * mate_ui_session_set_idle_callback:
 * @idle_time_ms: Idle time threshold in milliseconds
 * @callback: Callback to invoke when idle
 * @user_data: User data for callback
 * @destroy: (nullable): Destroy notify for user data
 *
 * Sets a callback to be called when the user has been idle
 * for the specified time.
 *
 * Returns: A source ID that can be used with g_source_remove()
 */
guint
mate_ui_session_set_idle_callback(guint64        idle_time_ms,
                                   GCallback      callback,
                                   gpointer       user_data,
                                   GDestroyNotify destroy)
{
    g_return_val_if_fail(callback != NULL, 0);

    IdleCallbackData *data = g_new0(IdleCallbackData, 1);
    data->threshold = idle_time_ms;
    data->callback = callback;
    data->user_data = user_data;
    data->destroy = destroy;

    /* Check every second */
    guint source_id = g_timeout_add_seconds_full(G_PRIORITY_DEFAULT,
                                                   1,
                                                   idle_check_callback,
                                                   data,
                                                   idle_callback_data_free);
    data->source_id = source_id;
    return source_id;
}
