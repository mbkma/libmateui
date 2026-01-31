/*
 * mate-ui-session.h - Session management helpers for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef MATE_UI_SESSION_H
#define MATE_UI_SESSION_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * MateUiInhibitFlags:
 * @MATE_UI_INHIBIT_LOGOUT: Inhibit session logout
 * @MATE_UI_INHIBIT_USER_SWITCH: Inhibit user switching
 * @MATE_UI_INHIBIT_SUSPEND: Inhibit system suspend
 * @MATE_UI_INHIBIT_IDLE: Inhibit session idle (screensaver)
 *
 * Flags indicating what to inhibit.
 */
typedef enum
{
    MATE_UI_INHIBIT_LOGOUT      = 1 << 0,
    MATE_UI_INHIBIT_USER_SWITCH = 1 << 1,
    MATE_UI_INHIBIT_SUSPEND     = 1 << 2,
    MATE_UI_INHIBIT_IDLE        = 1 << 3,
} MateUiInhibitFlags;

/**
 * MateUiSessionInhibitor:
 *
 * Opaque structure representing a session inhibitor.
 */
typedef struct _MateUiSessionInhibitor MateUiSessionInhibitor;

/**
 * mate_ui_session_inhibit:
 * @app: (nullable): A #GtkApplication or %NULL
 * @window: (nullable): A #GtkWindow or %NULL
 * @flags: What to inhibit
 * @reason: Human-readable reason for the inhibition
 *
 * Inhibits session actions. The inhibitor must be released with
 * mate_ui_session_uninhibit().
 *
 * Returns: (transfer full) (nullable): An inhibitor handle or %NULL on failure
 */
MateUiSessionInhibitor *mate_ui_session_inhibit(GtkApplication     *app,
                                                  GtkWindow          *window,
                                                  MateUiInhibitFlags  flags,
                                                  const gchar        *reason);

/**
 * mate_ui_session_uninhibit:
 * @inhibitor: An inhibitor returned by mate_ui_session_inhibit()
 *
 * Releases a session inhibitor.
 */
void mate_ui_session_uninhibit(MateUiSessionInhibitor *inhibitor);

/**
 * mate_ui_session_is_inhibited:
 * @flags: Flags to check
 *
 * Checks if any session actions are currently inhibited.
 *
 * Returns: %TRUE if inhibited
 */
gboolean mate_ui_session_is_inhibited(MateUiInhibitFlags flags);

/**
 * mate_ui_session_register:
 * @app: A #GtkApplication
 * @client_id: (nullable): Session client ID or %NULL
 *
 * Registers the application with the session manager.
 *
 * Returns: %TRUE on success
 */
gboolean mate_ui_session_register(GtkApplication *app,
                                   const gchar    *client_id);

/**
 * mate_ui_session_set_restart_command:
 * @app: A #GtkApplication
 * @argc: Number of arguments
 * @argv: Argument vector
 *
 * Sets the command to restart this application.
 */
void mate_ui_session_set_restart_command(GtkApplication  *app,
                                          gint             argc,
                                          const gchar    **argv);

/**
 * mate_ui_session_request_save:
 * @app: A #GtkApplication
 *
 * Requests the session to save state.
 *
 * Returns: %TRUE if request was accepted
 */
gboolean mate_ui_session_request_save(GtkApplication *app);

/**
 * mate_ui_session_request_logout:
 * @prompt: Whether to prompt the user
 *
 * Requests the session to log out.
 */
void mate_ui_session_request_logout(gboolean prompt);

/**
 * mate_ui_session_request_shutdown:
 * @prompt: Whether to prompt the user
 *
 * Requests the system to shut down.
 */
void mate_ui_session_request_shutdown(gboolean prompt);

/**
 * mate_ui_session_request_reboot:
 * @prompt: Whether to prompt the user
 *
 * Requests the system to reboot.
 */
void mate_ui_session_request_reboot(gboolean prompt);

/**
 * MateUiSessionSaveCallback:
 * @user_data: User data
 *
 * Callback for session save requests.
 *
 * Returns: %TRUE if save was successful
 */
typedef gboolean (*MateUiSessionSaveCallback)(gpointer user_data);

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
void mate_ui_session_set_save_callback(GtkApplication            *app,
                                        MateUiSessionSaveCallback  callback,
                                        gpointer                   user_data,
                                        GDestroyNotify             destroy);

/**
 * mate_ui_session_get_idle_time:
 *
 * Gets the current idle time in milliseconds.
 *
 * Returns: Idle time in milliseconds
 */
guint64 mate_ui_session_get_idle_time(void);

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
guint mate_ui_session_set_idle_callback(guint64        idle_time_ms,
                                         GCallback      callback,
                                         gpointer       user_data,
                                         GDestroyNotify destroy);

G_END_DECLS

#endif /* MATE_UI_SESSION_H */
