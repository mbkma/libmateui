/*
 * mate-ui-window.c - Window management helpers for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "config.h"
#include "mate-ui-window.h"

typedef struct
{
    GtkWidget         *main_box;
    GtkWidget         *menubar;
    GtkWidget         *toolbar;
    GtkWidget         *content;
    GtkWidget         *statusbar;
    MateUiWindowFlags  flags;

    /* Settings binding */
    GSettings         *settings;
    gchar             *width_key;
    gchar             *height_key;
    gchar             *maximized_key;
    gulong             configure_handler;
    gulong             state_handler;
} MateUiWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(MateUiWindow, mate_ui_window, GTK_TYPE_APPLICATION_WINDOW)


static void
mate_ui_window_rebuild_layout(MateUiWindow *self)
{
    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(self);

    /* Remove all children from main_box */
    GList *children = gtk_container_get_children(GTK_CONTAINER(priv->main_box));
    for (GList *l = children; l != NULL; l = l->next)
    {
        gtk_container_remove(GTK_CONTAINER(priv->main_box), GTK_WIDGET(l->data));
    }
    g_list_free(children);

    /* Re-add in order: menubar, toolbar, content, statusbar */
    if (priv->menubar != NULL)
    {
        gtk_box_pack_start(GTK_BOX(priv->main_box), priv->menubar, FALSE, FALSE, 0);
        gtk_widget_show(priv->menubar);
    }

    if (priv->toolbar != NULL)
    {
        gtk_box_pack_start(GTK_BOX(priv->main_box), priv->toolbar, FALSE, FALSE, 0);
        gtk_widget_show(priv->toolbar);
    }

    if (priv->content != NULL)
    {
        gtk_box_pack_start(GTK_BOX(priv->main_box), priv->content, TRUE, TRUE, 0);
        gtk_widget_show(priv->content);
    }

    if (priv->statusbar != NULL)
    {
        gtk_box_pack_end(GTK_BOX(priv->main_box), priv->statusbar, FALSE, FALSE, 0);
        gtk_widget_show(priv->statusbar);
    }
}

static gboolean
mate_ui_window_configure_event(GtkWidget         *widget,
                                GdkEventConfigure *event,
                                gpointer           user_data G_GNUC_UNUSED)
{
    MateUiWindow *self = MATE_UI_WINDOW(widget);
    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(self);

    if (priv->settings == NULL)
        return FALSE;

    /* Don't save size when maximized */
    GdkWindow *gdk_window = gtk_widget_get_window(widget);
    if (gdk_window != NULL)
    {
        GdkWindowState state = gdk_window_get_state(gdk_window);
        if (state & GDK_WINDOW_STATE_MAXIMIZED)
            return FALSE;
    }

    if (priv->width_key != NULL)
        g_settings_set_int(priv->settings, priv->width_key, event->width);

    if (priv->height_key != NULL)
        g_settings_set_int(priv->settings, priv->height_key, event->height);

    return FALSE;
}

static gboolean
mate_ui_window_state_event(GtkWidget           *widget,
                            GdkEventWindowState *event,
                            gpointer             user_data G_GNUC_UNUSED)
{
    MateUiWindow *self = MATE_UI_WINDOW(widget);
    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(self);

    if (priv->settings == NULL || priv->maximized_key == NULL)
        return FALSE;

    if (event->changed_mask & GDK_WINDOW_STATE_MAXIMIZED)
    {
        gboolean maximized = (event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED) != 0;
        g_settings_set_boolean(priv->settings, priv->maximized_key, maximized);
    }

    return FALSE;
}

static void
mate_ui_window_init(MateUiWindow *self)
{
    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(self);

    priv->main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(self), priv->main_box);
    gtk_widget_show(priv->main_box);

    priv->menubar = NULL;
    priv->toolbar = NULL;
    priv->content = NULL;
    priv->statusbar = NULL;
    priv->flags = MATE_UI_WINDOW_NONE;

    priv->settings = NULL;
    priv->width_key = NULL;
    priv->height_key = NULL;
    priv->maximized_key = NULL;
    priv->configure_handler = 0;
    priv->state_handler = 0;
}

static void
mate_ui_window_finalize(GObject *object)
{
    MateUiWindow *self = MATE_UI_WINDOW(object);
    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(self);

    g_clear_object(&priv->settings);
    g_free(priv->width_key);
    g_free(priv->height_key);
    g_free(priv->maximized_key);

    G_OBJECT_CLASS(mate_ui_window_parent_class)->finalize(object);
}


static void
mate_ui_window_class_init(MateUiWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = mate_ui_window_finalize;
}

/**
 * mate_ui_window_new:
 * @app: A #GtkApplication
 * @title: (nullable): The window title
 * @flags: Flags controlling window behavior
 *
 * Creates a new MATE UI window attached to the given application.
 *
 * Returns: (transfer full): A new #MateUiWindow
 */
GtkWidget *
mate_ui_window_new(GtkApplication    *app,
                    const gchar       *title,
                    MateUiWindowFlags  flags)
{
    g_return_val_if_fail(GTK_IS_APPLICATION(app), NULL);

    MateUiWindow *window = g_object_new(MATE_UI_TYPE_WINDOW,
                                         "application", app,
                                         NULL);

    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(window);
    priv->flags = flags;

    if (title != NULL)
        gtk_window_set_title(GTK_WINDOW(window), title);

    /* Set up headerbar if requested */
    if (flags & MATE_UI_WINDOW_USE_HEADERBAR)
    {
        GtkWidget *headerbar = gtk_header_bar_new();
        gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), TRUE);
        if (title != NULL)
            gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar), title);
        gtk_window_set_titlebar(GTK_WINDOW(window), headerbar);
        gtk_widget_show(headerbar);
    }

    return GTK_WIDGET(window);
}

/**
 * mate_ui_window_set_menubar:
 * @window: A #MateUiWindow
 * @menubar: (nullable): A #GtkMenuBar or %NULL to remove
 *
 * Sets or removes the menubar for this window.
 */
void
mate_ui_window_set_menubar(MateUiWindow *window,
                            GtkWidget    *menubar)
{
    g_return_if_fail(MATE_UI_IS_WINDOW(window));
    g_return_if_fail(menubar == NULL || GTK_IS_MENU_BAR(menubar));

    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(window);

    if (priv->menubar == menubar)
        return;

    if (priv->menubar != NULL)
        g_object_unref(priv->menubar);

    priv->menubar = menubar;

    if (priv->menubar != NULL)
        g_object_ref_sink(priv->menubar);

    mate_ui_window_rebuild_layout(window);
}

/**
 * mate_ui_window_get_menubar:
 * @window: A #MateUiWindow
 *
 * Gets the menubar for this window.
 *
 * Returns: (transfer none) (nullable): The #GtkMenuBar or %NULL
 */
GtkWidget *
mate_ui_window_get_menubar(MateUiWindow *window)
{
    g_return_val_if_fail(MATE_UI_IS_WINDOW(window), NULL);

    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(window);
    return priv->menubar;
}

/**
 * mate_ui_window_set_toolbar:
 * @window: A #MateUiWindow
 * @toolbar: (nullable): A #GtkToolbar or %NULL to remove
 *
 * Sets or removes the toolbar for this window.
 */
void
mate_ui_window_set_toolbar(MateUiWindow *window,
                            GtkWidget    *toolbar)
{
    g_return_if_fail(MATE_UI_IS_WINDOW(window));
    g_return_if_fail(toolbar == NULL || GTK_IS_TOOLBAR(toolbar));

    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(window);

    if (priv->toolbar == toolbar)
        return;

    priv->toolbar = toolbar;
    mate_ui_window_rebuild_layout(window);
}

/**
 * mate_ui_window_get_toolbar:
 * @window: A #MateUiWindow
 *
 * Gets the toolbar for this window.
 *
 * Returns: (transfer none) (nullable): The #GtkToolbar or %NULL
 */
GtkWidget *
mate_ui_window_get_toolbar(MateUiWindow *window)
{
    g_return_val_if_fail(MATE_UI_IS_WINDOW(window), NULL);

    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(window);
    return priv->toolbar;
}

/**
 * mate_ui_window_set_content:
 * @window: A #MateUiWindow
 * @content: The main content widget
 *
 * Sets the main content widget for this window.
 */
void
mate_ui_window_set_content(MateUiWindow *window,
                            GtkWidget    *content)
{
    g_return_if_fail(MATE_UI_IS_WINDOW(window));
    g_return_if_fail(GTK_IS_WIDGET(content));

    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(window);

    if (priv->content == content)
        return;

    priv->content = content;
    mate_ui_window_rebuild_layout(window);
}

/**
 * mate_ui_window_get_content:
 * @window: A #MateUiWindow
 *
 * Gets the main content widget.
 *
 * Returns: (transfer none) (nullable): The content widget or %NULL
 */
GtkWidget *
mate_ui_window_get_content(MateUiWindow *window)
{
    g_return_val_if_fail(MATE_UI_IS_WINDOW(window), NULL);

    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(window);
    return priv->content;
}

/**
 * mate_ui_window_set_statusbar:
 * @window: A #MateUiWindow
 * @statusbar: (nullable): A #GtkStatusbar or %NULL to remove
 *
 * Sets or removes the statusbar for this window.
 */
void
mate_ui_window_set_statusbar(MateUiWindow *window,
                              GtkWidget    *statusbar)
{
    g_return_if_fail(MATE_UI_IS_WINDOW(window));

    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(window);

    if (priv->statusbar == statusbar)
        return;

    priv->statusbar = statusbar;
    mate_ui_window_rebuild_layout(window);
}

/**
 * mate_ui_window_get_statusbar:
 * @window: A #MateUiWindow
 *
 * Gets the statusbar for this window.
 *
 * Returns: (transfer none) (nullable): The #GtkStatusbar or %NULL
 */
GtkWidget *
mate_ui_window_get_statusbar(MateUiWindow *window)
{
    g_return_val_if_fail(MATE_UI_IS_WINDOW(window), NULL);

    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(window);
    return priv->statusbar;
}

/**
 * mate_ui_window_bind_settings:
 * @window: A #MateUiWindow
 * @settings: A #GSettings object
 * @width_key: (nullable): Key name for window width, or %NULL
 * @height_key: (nullable): Key name for window height, or %NULL
 * @maximized_key: (nullable): Key name for maximized state, or %NULL
 *
 * Binds window geometry to GSettings keys for persistence.
 */
void
mate_ui_window_bind_settings(MateUiWindow *window,
                              GSettings    *settings,
                              const gchar  *width_key,
                              const gchar  *height_key,
                              const gchar  *maximized_key)
{
    g_return_if_fail(MATE_UI_IS_WINDOW(window));
    g_return_if_fail(G_IS_SETTINGS(settings));

    MateUiWindowPrivate *priv = mate_ui_window_get_instance_private(window);

    /* Clear previous binding */
    g_clear_object(&priv->settings);
    g_clear_pointer(&priv->width_key, g_free);
    g_clear_pointer(&priv->height_key, g_free);
    g_clear_pointer(&priv->maximized_key, g_free);

    if (priv->configure_handler > 0)
    {
        g_signal_handler_disconnect(window, priv->configure_handler);
        priv->configure_handler = 0;
    }
    if (priv->state_handler > 0)
    {
        g_signal_handler_disconnect(window, priv->state_handler);
        priv->state_handler = 0;
    }

    /* Set new binding */
    priv->settings = g_object_ref(settings);
    priv->width_key = g_strdup(width_key);
    priv->height_key = g_strdup(height_key);
    priv->maximized_key = g_strdup(maximized_key);

    /* Connect signals */
    priv->configure_handler = g_signal_connect(window, "configure-event",
                                                G_CALLBACK(mate_ui_window_configure_event), NULL);
    priv->state_handler = g_signal_connect(window, "window-state-event",
                                            G_CALLBACK(mate_ui_window_state_event), NULL);
}

/**
 * mate_ui_window_set_default_size_from_settings:
 * @window: A #MateUiWindow
 * @settings: A #GSettings object
 * @width_key: Key name for window width
 * @height_key: Key name for window height
 * @default_width: Default width if not in settings
 * @default_height: Default height if not in settings
 *
 * Sets the default window size from GSettings, with fallback defaults.
 */
void
mate_ui_window_set_default_size_from_settings(MateUiWindow *window,
                                               GSettings    *settings,
                                               const gchar  *width_key,
                                               const gchar  *height_key,
                                               gint          default_width,
                                               gint          default_height)
{
    g_return_if_fail(MATE_UI_IS_WINDOW(window));
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(width_key != NULL);
    g_return_if_fail(height_key != NULL);

    gint width = g_settings_get_int(settings, width_key);
    gint height = g_settings_get_int(settings, height_key);

    if (width <= 0)
        width = default_width;
    if (height <= 0)
        height = default_height;

    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
}

/**
 * mate_ui_window_center_on_parent:
 * @window: A #MateUiWindow
 *
 * Centers the window on its transient parent, if any.
 */
void
mate_ui_window_center_on_parent(MateUiWindow *window)
{
    g_return_if_fail(MATE_UI_IS_WINDOW(window));

    GtkWindow *parent = gtk_window_get_transient_for(GTK_WINDOW(window));
    if (parent == NULL)
        return;

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ON_PARENT);
}

/**
 * mate_ui_window_present_with_time:
 * @window: A #MateUiWindow
 *
 * Presents the window to the user with proper timestamp handling.
 */
void
mate_ui_window_present_with_time(MateUiWindow *window)
{
    g_return_if_fail(MATE_UI_IS_WINDOW(window));

    guint32 timestamp = gtk_get_current_event_time();
    if (timestamp == GDK_CURRENT_TIME)
        timestamp = g_get_monotonic_time() / 1000;

    gtk_window_present_with_time(GTK_WINDOW(window), timestamp);
}
