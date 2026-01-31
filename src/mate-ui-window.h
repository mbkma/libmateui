/*
 * mate-ui-window.h - Window management helpers for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#ifndef MATE_UI_WINDOW_H
#define MATE_UI_WINDOW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MATE_UI_TYPE_WINDOW (mate_ui_window_get_type())
G_DECLARE_DERIVABLE_TYPE(MateUiWindow, mate_ui_window, MATE_UI, WINDOW, GtkApplicationWindow)

/**
 * MateUiWindowClass:
 * @parent_class: The parent class
 *
 * The class structure for #MateUiWindow
 */
struct _MateUiWindowClass
{
    GtkApplicationWindowClass parent_class;

    /* Padding for future expansion */
    gpointer padding[8];
};

/**
 * MateUiWindowFlags:
 * @MATE_UI_WINDOW_NONE: No special flags
 * @MATE_UI_WINDOW_USE_HEADERBAR: Use a GtkHeaderBar instead of traditional title bar
 * @MATE_UI_WINDOW_SHOW_MENUBAR: Show a menubar if the application has one
 * @MATE_UI_WINDOW_REMEMBER_SIZE: Remember window size in GSettings
 * @MATE_UI_WINDOW_REMEMBER_POSITION: Remember window position in GSettings
 *
 * Flags to control window behavior
 */
typedef enum
{
    MATE_UI_WINDOW_NONE             = 0,
    MATE_UI_WINDOW_USE_HEADERBAR    = 1 << 0,
    MATE_UI_WINDOW_SHOW_MENUBAR     = 1 << 1,
    MATE_UI_WINDOW_REMEMBER_SIZE    = 1 << 2,
    MATE_UI_WINDOW_REMEMBER_POSITION = 1 << 3,
} MateUiWindowFlags;

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
GtkWidget *mate_ui_window_new(GtkApplication    *app,
                               const gchar       *title,
                               MateUiWindowFlags  flags);

/**
 * mate_ui_window_set_menubar:
 * @window: A #MateUiWindow
 * @menubar: (nullable): A #GtkMenuBar or %NULL to remove
 *
 * Sets or removes the menubar for this window.
 */
void mate_ui_window_set_menubar(MateUiWindow *window,
                                 GtkWidget    *menubar);

/**
 * mate_ui_window_get_menubar:
 * @window: A #MateUiWindow
 *
 * Gets the menubar for this window.
 *
 * Returns: (transfer none) (nullable): The #GtkMenuBar or %NULL
 */
GtkWidget *mate_ui_window_get_menubar(MateUiWindow *window);

/**
 * mate_ui_window_set_toolbar:
 * @window: A #MateUiWindow
 * @toolbar: (nullable): A #GtkToolbar or %NULL to remove
 *
 * Sets or removes the toolbar for this window.
 */
void mate_ui_window_set_toolbar(MateUiWindow *window,
                                 GtkWidget    *toolbar);

/**
 * mate_ui_window_get_toolbar:
 * @window: A #MateUiWindow
 *
 * Gets the toolbar for this window.
 *
 * Returns: (transfer none) (nullable): The #GtkToolbar or %NULL
 */
GtkWidget *mate_ui_window_get_toolbar(MateUiWindow *window);

/**
 * mate_ui_window_set_content:
 * @window: A #MateUiWindow
 * @content: The main content widget
 *
 * Sets the main content widget for this window. The content
 * will be placed below any menubar/toolbar.
 */
void mate_ui_window_set_content(MateUiWindow *window,
                                 GtkWidget    *content);

/**
 * mate_ui_window_get_content:
 * @window: A #MateUiWindow
 *
 * Gets the main content widget.
 *
 * Returns: (transfer none) (nullable): The content widget or %NULL
 */
GtkWidget *mate_ui_window_get_content(MateUiWindow *window);

/**
 * mate_ui_window_set_statusbar:
 * @window: A #MateUiWindow
 * @statusbar: (nullable): A #GtkStatusbar or %NULL to remove
 *
 * Sets or removes the statusbar for this window.
 */
void mate_ui_window_set_statusbar(MateUiWindow *window,
                                   GtkWidget    *statusbar);

/**
 * mate_ui_window_get_statusbar:
 * @window: A #MateUiWindow
 *
 * Gets the statusbar for this window.
 *
 * Returns: (transfer none) (nullable): The #GtkStatusbar or %NULL
 */
GtkWidget *mate_ui_window_get_statusbar(MateUiWindow *window);

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
void mate_ui_window_bind_settings(MateUiWindow *window,
                                   GSettings    *settings,
                                   const gchar  *width_key,
                                   const gchar  *height_key,
                                   const gchar  *maximized_key);

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
void mate_ui_window_set_default_size_from_settings(MateUiWindow *window,
                                                    GSettings    *settings,
                                                    const gchar  *width_key,
                                                    const gchar  *height_key,
                                                    gint          default_width,
                                                    gint          default_height);

/**
 * mate_ui_window_center_on_parent:
 * @window: A #MateUiWindow
 *
 * Centers the window on its transient parent, if any.
 */
void mate_ui_window_center_on_parent(MateUiWindow *window);

/**
 * mate_ui_window_present_with_time:
 * @window: A #MateUiWindow
 *
 * Presents the window to the user with proper timestamp handling.
 */
void mate_ui_window_present_with_time(MateUiWindow *window);

G_END_DECLS

#endif /* MATE_UI_WINDOW_H */
