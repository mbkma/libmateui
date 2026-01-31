/*
 * mate-ui-util.h - Utility functions for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef MATE_UI_UTIL_H
#define MATE_UI_UTIL_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * mate_ui_util_show_uri:
 * @screen: (nullable): A #GdkScreen or %NULL for default
 * @uri: The URI to show
 * @timestamp: Event timestamp
 * @error: Return location for error
 *
 * Opens a URI with the default application.
 *
 * Returns: %TRUE on success
 */
gboolean mate_ui_util_show_uri(GdkScreen   *screen,
                                const gchar *uri,
                                guint32      timestamp,
                                GError     **error);

/**
 * mate_ui_util_show_help:
 * @screen: (nullable): A #GdkScreen or %NULL
 * @doc_id: Help document ID (e.g., "mate-terminal")
 * @link_id: (nullable): Section ID within the document
 * @error: Return location for error
 *
 * Opens help documentation.
 *
 * Returns: %TRUE on success
 */
gboolean mate_ui_util_show_help(GdkScreen   *screen,
                                 const gchar *doc_id,
                                 const gchar *link_id,
                                 GError     **error);

/**
 * mate_ui_util_show_url:
 * @parent: (nullable): Parent window
 * @url: The URL to open
 *
 * Opens a URL in the default browser.
 *
 * Returns: %TRUE on success
 */
gboolean mate_ui_util_show_url(GtkWindow   *parent,
                                const gchar *url);

/**
 * mate_ui_util_get_data_dir:
 * @app_id: Application ID
 *
 * Gets the data directory for an application.
 *
 * Returns: (transfer full): The data directory path
 */
gchar *mate_ui_util_get_data_dir(const gchar *app_id);

/**
 * mate_ui_util_get_config_dir:
 * @app_id: Application ID
 *
 * Gets the configuration directory for an application.
 *
 * Returns: (transfer full): The config directory path
 */
gchar *mate_ui_util_get_config_dir(const gchar *app_id);

/**
 * mate_ui_util_get_cache_dir:
 * @app_id: Application ID
 *
 * Gets the cache directory for an application.
 *
 * Returns: (transfer full): The cache directory path
 */
gchar *mate_ui_util_get_cache_dir(const gchar *app_id);

/**
 * mate_ui_util_ensure_dir:
 * @path: Directory path
 * @error: Return location for error
 *
 * Ensures a directory exists, creating it if necessary.
 *
 * Returns: %TRUE on success
 */
gboolean mate_ui_util_ensure_dir(const gchar  *path,
                                  GError      **error);

/**
 * mate_ui_util_icon_name_for_mimetype:
 * @mimetype: A MIME type string
 *
 * Gets an icon name for a MIME type.
 *
 * Returns: (transfer full): An icon name (caller must free)
 */
gchar *mate_ui_util_icon_name_for_mimetype(const gchar *mimetype);

/**
 * mate_ui_util_set_widget_tooltip:
 * @widget: A #GtkWidget
 * @format: Printf-style format string
 * @...: Arguments for format
 *
 * Sets a tooltip on a widget with printf-style formatting.
 */
void mate_ui_util_set_widget_tooltip(GtkWidget   *widget,
                                      const gchar *format,
                                      ...) G_GNUC_PRINTF(2, 3);

/**
 * mate_ui_util_widget_set_margin:
 * @widget: A #GtkWidget
 * @margin: Margin in pixels
 *
 * Sets all margins on a widget to the same value.
 */
void mate_ui_util_widget_set_margin(GtkWidget *widget,
                                     gint       margin);

/**
 * mate_ui_util_widget_set_margins:
 * @widget: A #GtkWidget
 * @top: Top margin
 * @bottom: Bottom margin
 * @start: Start (left in LTR) margin
 * @end: End (right in LTR) margin
 *
 * Sets individual margins on a widget.
 */
void mate_ui_util_widget_set_margins(GtkWidget *widget,
                                      gint       top,
                                      gint       bottom,
                                      gint       start,
                                      gint       end);

/**
 * mate_ui_util_load_css:
 * @css_data: CSS data string
 * @priority: Style provider priority
 *
 * Loads CSS into the default screen's style context.
 *
 * Returns: (transfer full): The #GtkCssProvider (unref when done)
 */
GtkCssProvider *mate_ui_util_load_css(const gchar *css_data,
                                       guint        priority);

/**
 * mate_ui_util_load_css_file:
 * @filename: Path to CSS file
 * @priority: Style provider priority
 * @error: Return location for error
 *
 * Loads CSS from a file into the default screen's style context.
 *
 * Returns: (transfer full) (nullable): The #GtkCssProvider or %NULL on error
 */
GtkCssProvider *mate_ui_util_load_css_file(const gchar  *filename,
                                            guint         priority,
                                            GError      **error);

/**
 * mate_ui_util_get_icon:
 * @icon_name: Icon name
 * @size: Icon size in pixels
 *
 * Gets a pixbuf for an icon.
 *
 * Returns: (transfer full) (nullable): A #GdkPixbuf or %NULL
 */
GdkPixbuf *mate_ui_util_get_icon(const gchar *icon_name,
                                  gint         size);

/**
 * mate_ui_util_create_label_with_mnemonic:
 * @text: Label text with mnemonic
 * @target: (nullable): Widget to activate with mnemonic
 *
 * Creates a label with mnemonic and optional target widget.
 *
 * Returns: (transfer full): A new #GtkLabel
 */
GtkWidget *mate_ui_util_create_label_with_mnemonic(const gchar *text,
                                                    GtkWidget   *target);

/**
 * mate_ui_util_add_style_class:
 * @widget: A #GtkWidget
 * @class_name: CSS class name to add
 *
 * Adds a CSS class to a widget.
 */
void mate_ui_util_add_style_class(GtkWidget   *widget,
                                   const gchar *class_name);

/**
 * mate_ui_util_remove_style_class:
 * @widget: A #GtkWidget
 * @class_name: CSS class name to remove
 *
 * Removes a CSS class from a widget.
 */
void mate_ui_util_remove_style_class(GtkWidget   *widget,
                                      const gchar *class_name);

/**
 * mate_ui_util_format_size:
 * @size: Size in bytes
 *
 * Formats a file size for display.
 *
 * Returns: (transfer full): Formatted size string
 */
gchar *mate_ui_util_format_size(guint64 size);

/**
 * mate_ui_util_format_time:
 * @seconds: Time in seconds
 *
 * Formats a duration for display.
 *
 * Returns: (transfer full): Formatted time string
 */
gchar *mate_ui_util_format_time(guint seconds);

/**
 * mate_ui_util_spawn_command_async:
 * @command: Command to spawn
 * @error: Return location for error
 *
 * Spawns a command asynchronously.
 *
 * Returns: %TRUE on success
 */
gboolean mate_ui_util_spawn_command_async(const gchar  *command,
                                           GError      **error);

/**
 * mate_ui_util_get_window_position:
 * @window: A #GtkWindow
 * @x: (out): Return location for X position
 * @y: (out): Return location for Y position
 *
 * Gets the current window position.
 */
void mate_ui_util_get_window_position(GtkWindow *window,
                                       gint      *x,
                                       gint      *y);

/**
 * mate_ui_util_set_window_position:
 * @window: A #GtkWindow
 * @x: X position
 * @y: Y position
 *
 * Sets the window position.
 */
void mate_ui_util_set_window_position(GtkWindow *window,
                                       gint       x,
                                       gint       y);

/**
 * mate_ui_util_is_wayland:
 *
 * Checks if running on Wayland.
 *
 * Returns: %TRUE if running on Wayland
 */
gboolean mate_ui_util_is_wayland(void);

/**
 * mate_ui_util_is_x11:
 *
 * Checks if running on X11.
 *
 * Returns: %TRUE if running on X11
 */
gboolean mate_ui_util_is_x11(void);

G_END_DECLS

#endif /* MATE_UI_UTIL_H */
