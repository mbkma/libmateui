/*
 * mate-ui-accel.c - Keyboard accelerator management for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "config.h"
#include "mate-ui-accel.h"

struct _MateUiAccelMap
{
    GHashTable *accels;  /* action_name -> accel string */
};

/**
 * mate_ui_accel_map_new:
 *
 * Creates a new accelerator map.
 *
 * Returns: (transfer full): A new #MateUiAccelMap
 */
MateUiAccelMap *
mate_ui_accel_map_new(void)
{
    MateUiAccelMap *map = g_new0(MateUiAccelMap, 1);
    map->accels = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    return map;
}

/**
 * mate_ui_accel_map_free:
 * @map: A #MateUiAccelMap
 *
 * Frees an accelerator map.
 */
void
mate_ui_accel_map_free(MateUiAccelMap *map)
{
    if (map == NULL)
        return;

    g_hash_table_unref(map->accels);
    g_free(map);
}

/**
 * mate_ui_accel_map_add:
 * @map: A #MateUiAccelMap
 * @action_name: The action name
 * @accel: The accelerator string
 *
 * Adds an accelerator to the map.
 */
void
mate_ui_accel_map_add(MateUiAccelMap *map,
                       const gchar    *action_name,
                       const gchar    *accel)
{
    g_return_if_fail(map != NULL);
    g_return_if_fail(action_name != NULL);
    g_return_if_fail(accel != NULL);

    g_hash_table_insert(map->accels, g_strdup(action_name), g_strdup(accel));
}

/**
 * mate_ui_accel_map_add_entries:
 * @map: A #MateUiAccelMap
 * @entries: Array of #MateUiAccelEntry structures
 * @n_entries: Number of entries
 *
 * Adds multiple accelerators to the map.
 */
void
mate_ui_accel_map_add_entries(MateUiAccelMap         *map,
                               const MateUiAccelEntry *entries,
                               gsize                   n_entries)
{
    g_return_if_fail(map != NULL);
    g_return_if_fail(entries != NULL || n_entries == 0);

    for (gsize i = 0; i < n_entries; i++)
    {
        mate_ui_accel_map_add(map, entries[i].action_name, entries[i].accel);
    }
}

/**
 * mate_ui_accel_map_remove:
 * @map: A #MateUiAccelMap
 * @action_name: The action name to remove
 *
 * Removes an accelerator from the map.
 */
void
mate_ui_accel_map_remove(MateUiAccelMap *map,
                          const gchar    *action_name)
{
    g_return_if_fail(map != NULL);
    g_return_if_fail(action_name != NULL);

    g_hash_table_remove(map->accels, action_name);
}

/**
 * mate_ui_accel_map_get:
 * @map: A #MateUiAccelMap
 * @action_name: The action name
 *
 * Gets the accelerator for an action.
 *
 * Returns: (transfer none) (nullable): The accelerator string or %NULL
 */
const gchar *
mate_ui_accel_map_get(MateUiAccelMap *map,
                       const gchar    *action_name)
{
    g_return_val_if_fail(map != NULL, NULL);
    g_return_val_if_fail(action_name != NULL, NULL);

    return g_hash_table_lookup(map->accels, action_name);
}

/**
 * mate_ui_accel_map_apply_to_app:
 * @map: A #MateUiAccelMap
 * @app: A #GtkApplication
 *
 * Applies the accelerator map to an application.
 */
void
mate_ui_accel_map_apply_to_app(MateUiAccelMap *map,
                                GtkApplication *app)
{
    g_return_if_fail(map != NULL);
    g_return_if_fail(GTK_IS_APPLICATION(app));

    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, map->accels);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        const gchar *action_name = key;
        const gchar *accel = value;
        const gchar *accels[] = { accel, NULL };

        gtk_application_set_accels_for_action(app, action_name, accels);
    }
}

/**
 * mate_ui_accel_map_load:
 * @map: A #MateUiAccelMap
 * @filename: Path to the accelerator file
 * @error: Return location for error
 *
 * Loads accelerators from a file. File format is one entry per line:
 * action_name=<accelerator>
 *
 * Returns: %TRUE on success
 */
gboolean
mate_ui_accel_map_load(MateUiAccelMap  *map,
                        const gchar     *filename,
                        GError         **error)
{
    g_return_val_if_fail(map != NULL, FALSE);
    g_return_val_if_fail(filename != NULL, FALSE);

    gchar *contents = NULL;
    gsize length;

    if (!g_file_get_contents(filename, &contents, &length, error))
        return FALSE;

    gchar **lines = g_strsplit(contents, "\n", -1);
    g_free(contents);

    for (gint i = 0; lines[i] != NULL; i++)
    {
        gchar *line = g_strstrip(lines[i]);

        /* Skip empty lines and comments */
        if (*line == '\0' || *line == '#')
            continue;

        gchar **parts = g_strsplit(line, "=", 2);
        if (parts[0] != NULL && parts[1] != NULL)
        {
            gchar *action = g_strstrip(parts[0]);
            gchar *accel = g_strstrip(parts[1]);

            if (*action != '\0' && *accel != '\0')
            {
                mate_ui_accel_map_add(map, action, accel);
            }
        }
        g_strfreev(parts);
    }

    g_strfreev(lines);
    return TRUE;
}

/**
 * mate_ui_accel_map_save:
 * @map: A #MateUiAccelMap
 * @filename: Path to save to
 * @error: Return location for error
 *
 * Saves accelerators to a file.
 *
 * Returns: %TRUE on success
 */
gboolean
mate_ui_accel_map_save(MateUiAccelMap  *map,
                        const gchar     *filename,
                        GError         **error)
{
    g_return_val_if_fail(map != NULL, FALSE);
    g_return_val_if_fail(filename != NULL, FALSE);

    GString *content = g_string_new("# MATE UI Accelerator Map\n");
    g_string_append(content, "# Format: action_name=accelerator\n\n");

    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, map->accels);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        g_string_append_printf(content, "%s=%s\n", (gchar *)key, (gchar *)value);
    }

    gboolean result = g_file_set_contents(filename, content->str, content->len, error);
    g_string_free(content, TRUE);

    return result;
}

/**
 * mate_ui_accel_group_new:
 *
 * Creates a new accelerator group.
 *
 * Returns: (transfer full): A new #GtkAccelGroup
 */
GtkAccelGroup *
mate_ui_accel_group_new(void)
{
    return gtk_accel_group_new();
}

/* Callback data for accel group callbacks */
typedef struct
{
    GCallback callback;
    gpointer  user_data;
} AccelCallbackData;

static gboolean
accel_callback_wrapper(GtkAccelGroup   *accel_group G_GNUC_UNUSED,
                        GObject         *acceleratable G_GNUC_UNUSED,
                        guint            keyval G_GNUC_UNUSED,
                        GdkModifierType  modifier G_GNUC_UNUSED,
                        gpointer         user_data)
{
    AccelCallbackData *data = user_data;
    void (*cb)(gpointer) = (void (*)(gpointer))data->callback;
    cb(data->user_data);
    return TRUE;
}

static void
accel_callback_data_free(gpointer data, GClosure *closure G_GNUC_UNUSED)
{
    g_free(data);
}

/**
 * mate_ui_accel_group_add:
 * @accel_group: A #GtkAccelGroup
 * @accel: The accelerator string
 * @callback: Callback function
 * @user_data: User data for callback
 *
 * Adds an accelerator to the group with a callback.
 *
 * Returns: %TRUE if successfully added
 */
gboolean
mate_ui_accel_group_add(GtkAccelGroup *accel_group,
                         const gchar   *accel,
                         GCallback      callback,
                         gpointer       user_data)
{
    g_return_val_if_fail(GTK_IS_ACCEL_GROUP(accel_group), FALSE);
    g_return_val_if_fail(accel != NULL, FALSE);

    guint key;
    GdkModifierType mods;
    gtk_accelerator_parse(accel, &key, &mods);

    if (key == 0)
        return FALSE;

    AccelCallbackData *data = g_new0(AccelCallbackData, 1);
    data->callback = callback;
    data->user_data = user_data;

    GClosure *closure = g_cclosure_new(G_CALLBACK(accel_callback_wrapper),
                                        data, accel_callback_data_free);

    gtk_accel_group_connect(accel_group, key, mods, GTK_ACCEL_VISIBLE, closure);

    return TRUE;
}

/* Action callback data */
typedef struct
{
    GAction  *action;
    GVariant *parameter;
} ActionAccelData;

static gboolean
action_accel_callback(GtkAccelGroup   *accel_group G_GNUC_UNUSED,
                       GObject         *acceleratable G_GNUC_UNUSED,
                       guint            keyval G_GNUC_UNUSED,
                       GdkModifierType  modifier G_GNUC_UNUSED,
                       gpointer         user_data)
{
    ActionAccelData *data = user_data;
    g_action_activate(data->action, data->parameter);
    return TRUE;
}

static void
action_accel_data_free(gpointer data, GClosure *closure G_GNUC_UNUSED)
{
    ActionAccelData *d = data;
    g_object_unref(d->action);
    if (d->parameter)
        g_variant_unref(d->parameter);
    g_free(d);
}

/**
 * mate_ui_accel_group_add_action:
 * @accel_group: A #GtkAccelGroup
 * @accel: The accelerator string
 * @action: A #GAction to activate
 * @parameter: (nullable): Parameter for the action
 *
 * Adds an accelerator that activates an action.
 *
 * Returns: %TRUE if successfully added
 */
gboolean
mate_ui_accel_group_add_action(GtkAccelGroup *accel_group,
                                const gchar   *accel,
                                GAction       *action,
                                GVariant      *parameter)
{
    g_return_val_if_fail(GTK_IS_ACCEL_GROUP(accel_group), FALSE);
    g_return_val_if_fail(accel != NULL, FALSE);
    g_return_val_if_fail(G_IS_ACTION(action), FALSE);

    guint key;
    GdkModifierType mods;
    gtk_accelerator_parse(accel, &key, &mods);

    if (key == 0)
        return FALSE;

    ActionAccelData *data = g_new0(ActionAccelData, 1);
    data->action = g_object_ref(action);
    data->parameter = parameter ? g_variant_ref(parameter) : NULL;

    GClosure *closure = g_cclosure_new(G_CALLBACK(action_accel_callback),
                                        data, action_accel_data_free);

    gtk_accel_group_connect(accel_group, key, mods, GTK_ACCEL_VISIBLE, closure);

    return TRUE;
}

/**
 * mate_ui_accel_parse:
 * @accel: The accelerator string
 * @key: (out): Return location for key
 * @mods: (out): Return location for modifiers
 *
 * Parses an accelerator string.
 *
 * Returns: %TRUE if parsing succeeded
 */
gboolean
mate_ui_accel_parse(const gchar     *accel,
                     guint           *key,
                     GdkModifierType *mods)
{
    g_return_val_if_fail(accel != NULL, FALSE);
    g_return_val_if_fail(key != NULL, FALSE);
    g_return_val_if_fail(mods != NULL, FALSE);

    gtk_accelerator_parse(accel, key, mods);
    return (*key != 0);
}

/**
 * mate_ui_accel_to_string:
 * @key: The key value
 * @mods: The modifier mask
 *
 * Converts an accelerator to a string.
 *
 * Returns: (transfer full): The accelerator string
 */
gchar *
mate_ui_accel_to_string(guint           key,
                         GdkModifierType mods)
{
    return gtk_accelerator_name(key, mods);
}

/**
 * mate_ui_accel_label_set_accel:
 * @label: A #GtkAccelLabel
 * @accel: The accelerator string
 *
 * Sets the accelerator shown by an accel label.
 */
void
mate_ui_accel_label_set_accel(GtkAccelLabel *label,
                               const gchar   *accel)
{
    g_return_if_fail(GTK_IS_ACCEL_LABEL(label));
    g_return_if_fail(accel != NULL);

    guint key;
    GdkModifierType mods;
    gtk_accelerator_parse(accel, &key, &mods);

    if (key != 0)
    {
        gtk_accel_label_set_accel(label, key, mods);
    }
}

/**
 * mate_ui_accel_connect_to_widget:
 * @widget: A widget
 * @accel: The accelerator string
 * @signal_name: Signal to emit
 *
 * Connects an accelerator to emit a signal on a widget.
 */
void
mate_ui_accel_connect_to_widget(GtkWidget   *widget,
                                 const gchar *accel,
                                 const gchar *signal_name)
{
    g_return_if_fail(GTK_IS_WIDGET(widget));
    g_return_if_fail(accel != NULL);
    g_return_if_fail(signal_name != NULL);

    guint key;
    GdkModifierType mods;
    gtk_accelerator_parse(accel, &key, &mods);

    if (key == 0)
        return;

    GtkWidget *toplevel = gtk_widget_get_toplevel(widget);
    if (!GTK_IS_WINDOW(toplevel))
        return;

    GtkAccelGroup *accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(toplevel), accel_group);

    gtk_widget_add_accelerator(widget, signal_name, accel_group,
                                key, mods, GTK_ACCEL_VISIBLE);

    g_object_unref(accel_group);
}

/**
 * mate_ui_accel_set_app_accels:
 * @app: A #GtkApplication
 * @entries: Array of #MateUiAccelEntry
 * @n_entries: Number of entries
 *
 * Sets multiple application accelerators at once.
 */
void
mate_ui_accel_set_app_accels(GtkApplication         *app,
                              const MateUiAccelEntry *entries,
                              gsize                   n_entries)
{
    g_return_if_fail(GTK_IS_APPLICATION(app));
    g_return_if_fail(entries != NULL || n_entries == 0);

    for (gsize i = 0; i < n_entries; i++)
    {
        const gchar *accels[] = { entries[i].accel, NULL };
        gtk_application_set_accels_for_action(app, entries[i].action_name, accels);
    }
}

/**
 * mate_ui_accel_clear_app_accels:
 * @app: A #GtkApplication
 * @action_name: The action name
 *
 * Clears all accelerators for an action.
 */
void
mate_ui_accel_clear_app_accels(GtkApplication *app,
                                const gchar    *action_name)
{
    g_return_if_fail(GTK_IS_APPLICATION(app));
    g_return_if_fail(action_name != NULL);

    const gchar *accels[] = { NULL };
    gtk_application_set_accels_for_action(app, action_name, accels);
}
