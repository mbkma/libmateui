/*
 * mate-ui-util.c - Utility functions for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "config.h"
#include "mate-ui-util.h"

#include <gio/gio.h>

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#ifdef GDK_WINDOWING_WAYLAND
#include <gdk/gdkwayland.h>
#endif

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
gboolean
mate_ui_util_show_uri(GdkScreen   *screen,
                       const gchar *uri,
                       guint32      timestamp,
                       GError     **error)
{
    g_return_val_if_fail(uri != NULL, FALSE);

    if (screen == NULL)
        screen = gdk_screen_get_default();

    return gtk_show_uri_on_window(NULL, uri, timestamp, error);
}

/**
 * mate_ui_util_show_help:
 * @screen: (nullable): A #GdkScreen or %NULL
 * @doc_id: Help document ID
 * @link_id: (nullable): Section ID within the document
 * @error: Return location for error
 *
 * Opens help documentation.
 *
 * Returns: %TRUE on success
 */
gboolean
mate_ui_util_show_help(GdkScreen   *screen,
                        const gchar *doc_id,
                        const gchar *link_id,
                        GError     **error)
{
    g_return_val_if_fail(doc_id != NULL, FALSE);

    gchar *uri;
    if (link_id != NULL)
        uri = g_strdup_printf("help:%s/%s", doc_id, link_id);
    else
        uri = g_strdup_printf("help:%s", doc_id);

    guint32 timestamp = gtk_get_current_event_time();
    gboolean result = mate_ui_util_show_uri(screen, uri, timestamp, error);
    g_free(uri);

    return result;
}

/**
 * mate_ui_util_show_url:
 * @parent: (nullable): Parent window
 * @url: The URL to open
 *
 * Opens a URL in the default browser.
 *
 * Returns: %TRUE on success
 */
gboolean
mate_ui_util_show_url(GtkWindow   *parent,
                       const gchar *url)
{
    g_return_val_if_fail(url != NULL, FALSE);

    GError *error = NULL;
    guint32 timestamp = gtk_get_current_event_time();

    if (!gtk_show_uri_on_window(parent, url, timestamp, &error))
    {
        g_warning("Failed to open URL '%s': %s", url, error->message);
        g_error_free(error);
        return FALSE;
    }

    return TRUE;
}

/**
 * mate_ui_util_get_data_dir:
 * @app_id: Application ID
 *
 * Gets the data directory for an application.
 *
 * Returns: (transfer full): The data directory path
 */
gchar *
mate_ui_util_get_data_dir(const gchar *app_id)
{
    g_return_val_if_fail(app_id != NULL, NULL);
    return g_build_filename(g_get_user_data_dir(), app_id, NULL);
}

/**
 * mate_ui_util_get_config_dir:
 * @app_id: Application ID
 *
 * Gets the configuration directory for an application.
 *
 * Returns: (transfer full): The config directory path
 */
gchar *
mate_ui_util_get_config_dir(const gchar *app_id)
{
    g_return_val_if_fail(app_id != NULL, NULL);
    return g_build_filename(g_get_user_config_dir(), app_id, NULL);
}

/**
 * mate_ui_util_get_cache_dir:
 * @app_id: Application ID
 *
 * Gets the cache directory for an application.
 *
 * Returns: (transfer full): The cache directory path
 */
gchar *
mate_ui_util_get_cache_dir(const gchar *app_id)
{
    g_return_val_if_fail(app_id != NULL, NULL);
    return g_build_filename(g_get_user_cache_dir(), app_id, NULL);
}

/**
 * mate_ui_util_ensure_dir:
 * @path: Directory path
 * @error: Return location for error
 *
 * Ensures a directory exists, creating it if necessary.
 *
 * Returns: %TRUE on success
 */
gboolean
mate_ui_util_ensure_dir(const gchar  *path,
                         GError      **error)
{
    g_return_val_if_fail(path != NULL, FALSE);

    if (g_mkdir_with_parents(path, 0700) == -1)
    {
        int errsv = errno;
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errsv),
                     "Failed to create directory '%s': %s", path, g_strerror(errsv));
        return FALSE;
    }

    return TRUE;
}

/**
 * mate_ui_util_icon_name_for_mimetype:
 * @mimetype: A MIME type string
 *
 * Gets an icon name for a MIME type.
 *
 * Returns: (transfer full): An icon name (caller must free)
 */
gchar *
mate_ui_util_icon_name_for_mimetype(const gchar *mimetype)
{
    g_return_val_if_fail(mimetype != NULL, NULL);

    GIcon *icon = g_content_type_get_icon(mimetype);
    if (icon == NULL)
        return g_strdup("application-octet-stream");

    gchar *icon_name = NULL;
    if (G_IS_THEMED_ICON(icon))
    {
        const gchar * const *names = g_themed_icon_get_names(G_THEMED_ICON(icon));
        if (names != NULL && names[0] != NULL)
            icon_name = g_strdup(names[0]);
    }

    g_object_unref(icon);

    if (icon_name == NULL)
        icon_name = g_strdup("application-octet-stream");

    return icon_name;
}

/**
 * mate_ui_util_set_widget_tooltip:
 * @widget: A #GtkWidget
 * @format: Printf-style format string
 * @...: Arguments for format
 *
 * Sets a tooltip on a widget with printf-style formatting.
 */
void
mate_ui_util_set_widget_tooltip(GtkWidget   *widget,
                                 const gchar *format,
                                 ...)
{
    g_return_if_fail(GTK_IS_WIDGET(widget));
    g_return_if_fail(format != NULL);

    va_list args;
    va_start(args, format);
    gchar *tooltip = g_strdup_vprintf(format, args);
    va_end(args);

    gtk_widget_set_tooltip_text(widget, tooltip);
    g_free(tooltip);
}

/**
 * mate_ui_util_widget_set_margin:
 * @widget: A #GtkWidget
 * @margin: Margin in pixels
 *
 * Sets all margins on a widget to the same value.
 */
void
mate_ui_util_widget_set_margin(GtkWidget *widget,
                                gint       margin)
{
    g_return_if_fail(GTK_IS_WIDGET(widget));

    gtk_widget_set_margin_top(widget, margin);
    gtk_widget_set_margin_bottom(widget, margin);
    gtk_widget_set_margin_start(widget, margin);
    gtk_widget_set_margin_end(widget, margin);
}

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
void
mate_ui_util_widget_set_margins(GtkWidget *widget,
                                 gint       top,
                                 gint       bottom,
                                 gint       start,
                                 gint       end)
{
    g_return_if_fail(GTK_IS_WIDGET(widget));

    gtk_widget_set_margin_top(widget, top);
    gtk_widget_set_margin_bottom(widget, bottom);
    gtk_widget_set_margin_start(widget, start);
    gtk_widget_set_margin_end(widget, end);
}

/**
 * mate_ui_util_load_css:
 * @css_data: CSS data string
 * @priority: Style provider priority
 *
 * Loads CSS into the default screen's style context.
 *
 * Returns: (transfer full): The #GtkCssProvider (unref when done)
 */
GtkCssProvider *
mate_ui_util_load_css(const gchar *css_data,
                       guint        priority)
{
    g_return_val_if_fail(css_data != NULL, NULL);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);

    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
                                               GTK_STYLE_PROVIDER(provider),
                                               priority);

    return provider;
}

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
GtkCssProvider *
mate_ui_util_load_css_file(const gchar  *filename,
                            guint         priority,
                            GError      **error)
{
    g_return_val_if_fail(filename != NULL, NULL);

    GtkCssProvider *provider = gtk_css_provider_new();
    GFile *file = g_file_new_for_path(filename);

    if (!gtk_css_provider_load_from_file(provider, file, error))
    {
        g_object_unref(file);
        g_object_unref(provider);
        return NULL;
    }

    g_object_unref(file);

    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
                                               GTK_STYLE_PROVIDER(provider),
                                               priority);

    return provider;
}

/**
 * mate_ui_util_get_icon:
 * @icon_name: Icon name
 * @size: Icon size in pixels
 *
 * Gets a pixbuf for an icon.
 *
 * Returns: (transfer full) (nullable): A #GdkPixbuf or %NULL
 */
GdkPixbuf *
mate_ui_util_get_icon(const gchar *icon_name,
                       gint         size)
{
    g_return_val_if_fail(icon_name != NULL, NULL);
    g_return_val_if_fail(size > 0, NULL);

    GtkIconTheme *theme = gtk_icon_theme_get_default();
    GError *error = NULL;

    GdkPixbuf *pixbuf = gtk_icon_theme_load_icon(theme, icon_name, size,
                                                   GTK_ICON_LOOKUP_FORCE_SIZE, &error);
    if (error != NULL)
    {
        g_warning("Failed to load icon '%s': %s", icon_name, error->message);
        g_error_free(error);
    }

    return pixbuf;
}

/**
 * mate_ui_util_create_label_with_mnemonic:
 * @text: Label text with mnemonic
 * @target: (nullable): Widget to activate with mnemonic
 *
 * Creates a label with mnemonic and optional target widget.
 *
 * Returns: (transfer full): A new #GtkLabel
 */
GtkWidget *
mate_ui_util_create_label_with_mnemonic(const gchar *text,
                                         GtkWidget   *target)
{
    g_return_val_if_fail(text != NULL, NULL);

    GtkWidget *label = gtk_label_new_with_mnemonic(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);

    if (target != NULL)
        gtk_label_set_mnemonic_widget(GTK_LABEL(label), target);

    return label;
}

/**
 * mate_ui_util_add_style_class:
 * @widget: A #GtkWidget
 * @class_name: CSS class name to add
 *
 * Adds a CSS class to a widget.
 */
void
mate_ui_util_add_style_class(GtkWidget   *widget,
                              const gchar *class_name)
{
    g_return_if_fail(GTK_IS_WIDGET(widget));
    g_return_if_fail(class_name != NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(context, class_name);
}

/**
 * mate_ui_util_remove_style_class:
 * @widget: A #GtkWidget
 * @class_name: CSS class name to remove
 *
 * Removes a CSS class from a widget.
 */
void
mate_ui_util_remove_style_class(GtkWidget   *widget,
                                 const gchar *class_name)
{
    g_return_if_fail(GTK_IS_WIDGET(widget));
    g_return_if_fail(class_name != NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_remove_class(context, class_name);
}

/**
 * mate_ui_util_format_size:
 * @size: Size in bytes
 *
 * Formats a file size for display.
 *
 * Returns: (transfer full): Formatted size string
 */
gchar *
mate_ui_util_format_size(guint64 size)
{
    return g_format_size(size);
}

/**
 * mate_ui_util_format_time:
 * @seconds: Time in seconds
 *
 * Formats a duration for display.
 *
 * Returns: (transfer full): Formatted time string
 */
gchar *
mate_ui_util_format_time(guint seconds)
{
    if (seconds < 60)
        return g_strdup_printf("%u sec", seconds);
    else if (seconds < 3600)
        return g_strdup_printf("%u min %u sec", seconds / 60, seconds % 60);
    else
    {
        guint hours = seconds / 3600;
        guint mins = (seconds % 3600) / 60;
        guint secs = seconds % 60;
        return g_strdup_printf("%u hr %u min %u sec", hours, mins, secs);
    }
}

/**
 * mate_ui_util_spawn_command_async:
 * @command: Command to spawn
 * @error: Return location for error
 *
 * Spawns a command asynchronously.
 *
 * Returns: %TRUE on success
 */
gboolean
mate_ui_util_spawn_command_async(const gchar  *command,
                                  GError      **error)
{
    g_return_val_if_fail(command != NULL, FALSE);

    gint argc;
    gchar **argv;

    if (!g_shell_parse_argv(command, &argc, &argv, error))
        return FALSE;

    gboolean result = g_spawn_async(NULL, argv, NULL,
                                     G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                                     NULL, NULL, NULL, error);

    g_strfreev(argv);
    return result;
}

/**
 * mate_ui_util_get_window_position:
 * @window: A #GtkWindow
 * @x: (out): Return location for X position
 * @y: (out): Return location for Y position
 *
 * Gets the current window position.
 */
void
mate_ui_util_get_window_position(GtkWindow *window,
                                  gint      *x,
                                  gint      *y)
{
    g_return_if_fail(GTK_IS_WINDOW(window));
    g_return_if_fail(x != NULL);
    g_return_if_fail(y != NULL);

    gtk_window_get_position(window, x, y);
}

/**
 * mate_ui_util_set_window_position:
 * @window: A #GtkWindow
 * @x: X position
 * @y: Y position
 *
 * Sets the window position.
 */
void
mate_ui_util_set_window_position(GtkWindow *window,
                                  gint       x,
                                  gint       y)
{
    g_return_if_fail(GTK_IS_WINDOW(window));

    gtk_window_move(window, x, y);
}

/**
 * mate_ui_util_is_wayland:
 *
 * Checks if running on Wayland.
 *
 * Returns: %TRUE if running on Wayland
 */
gboolean
mate_ui_util_is_wayland(void)
{
#ifdef GDK_WINDOWING_WAYLAND
    GdkDisplay *display = gdk_display_get_default();
    return GDK_IS_WAYLAND_DISPLAY(display);
#else
    return FALSE;
#endif
}

/**
 * mate_ui_util_is_x11:
 *
 * Checks if running on X11.
 *
 * Returns: %TRUE if running on X11
 */
gboolean
mate_ui_util_is_x11(void)
{
#ifdef GDK_WINDOWING_X11
    GdkDisplay *display = gdk_display_get_default();
    return GDK_IS_X11_DISPLAY(display);
#else
    return FALSE;
#endif
}
