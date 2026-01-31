/*
 * mate-ui-accel.h - Keyboard accelerator management for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef MATE_UI_ACCEL_H
#define MATE_UI_ACCEL_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * MateUiAccelEntry:
 * @action_name: The action name (e.g., "app.quit" or "win.save")
 * @accel: The accelerator string (e.g., "<Control>q")
 *
 * Structure describing an accelerator binding.
 */
typedef struct
{
    const gchar *action_name;
    const gchar *accel;
} MateUiAccelEntry;

/**
 * MateUiAccelMap:
 *
 * Opaque structure managing accelerator mappings.
 */
typedef struct _MateUiAccelMap MateUiAccelMap;

/**
 * mate_ui_accel_map_new:
 *
 * Creates a new accelerator map.
 *
 * Returns: (transfer full): A new #MateUiAccelMap
 */
MateUiAccelMap *mate_ui_accel_map_new(void);

/**
 * mate_ui_accel_map_free:
 * @map: A #MateUiAccelMap
 *
 * Frees an accelerator map.
 */
void mate_ui_accel_map_free(MateUiAccelMap *map);

/**
 * mate_ui_accel_map_add:
 * @map: A #MateUiAccelMap
 * @action_name: The action name
 * @accel: The accelerator string
 *
 * Adds an accelerator to the map.
 */
void mate_ui_accel_map_add(MateUiAccelMap *map,
                            const gchar    *action_name,
                            const gchar    *accel);

/**
 * mate_ui_accel_map_add_entries:
 * @map: A #MateUiAccelMap
 * @entries: Array of #MateUiAccelEntry structures
 * @n_entries: Number of entries
 *
 * Adds multiple accelerators to the map.
 */
void mate_ui_accel_map_add_entries(MateUiAccelMap         *map,
                                    const MateUiAccelEntry *entries,
                                    gsize                   n_entries);

/**
 * mate_ui_accel_map_remove:
 * @map: A #MateUiAccelMap
 * @action_name: The action name to remove
 *
 * Removes an accelerator from the map.
 */
void mate_ui_accel_map_remove(MateUiAccelMap *map,
                               const gchar    *action_name);

/**
 * mate_ui_accel_map_get:
 * @map: A #MateUiAccelMap
 * @action_name: The action name
 *
 * Gets the accelerator for an action.
 *
 * Returns: (transfer none) (nullable): The accelerator string or %NULL
 */
const gchar *mate_ui_accel_map_get(MateUiAccelMap *map,
                                    const gchar    *action_name);

/**
 * mate_ui_accel_map_apply_to_app:
 * @map: A #MateUiAccelMap
 * @app: A #GtkApplication
 *
 * Applies the accelerator map to an application.
 */
void mate_ui_accel_map_apply_to_app(MateUiAccelMap *map,
                                     GtkApplication *app);

/**
 * mate_ui_accel_map_load:
 * @map: A #MateUiAccelMap
 * @filename: Path to the accelerator file
 * @error: Return location for error
 *
 * Loads accelerators from a file.
 *
 * Returns: %TRUE on success
 */
gboolean mate_ui_accel_map_load(MateUiAccelMap  *map,
                                 const gchar     *filename,
                                 GError         **error);

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
gboolean mate_ui_accel_map_save(MateUiAccelMap  *map,
                                 const gchar     *filename,
                                 GError         **error);

/**
 * mate_ui_accel_group_new:
 *
 * Creates a new accelerator group.
 *
 * Returns: (transfer full): A new #GtkAccelGroup
 */
GtkAccelGroup *mate_ui_accel_group_new(void);

/**
 * mate_ui_accel_group_add:
 * @accel_group: A #GtkAccelGroup
 * @accel: The accelerator string (e.g., "<Control>s")
 * @callback: Callback function
 * @user_data: User data for callback
 *
 * Adds an accelerator to the group with a callback.
 *
 * Returns: %TRUE if successfully added
 */
gboolean mate_ui_accel_group_add(GtkAccelGroup *accel_group,
                                  const gchar   *accel,
                                  GCallback      callback,
                                  gpointer       user_data);

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
gboolean mate_ui_accel_group_add_action(GtkAccelGroup *accel_group,
                                         const gchar   *accel,
                                         GAction       *action,
                                         GVariant      *parameter);

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
gboolean mate_ui_accel_parse(const gchar     *accel,
                              guint           *key,
                              GdkModifierType *mods);

/**
 * mate_ui_accel_to_string:
 * @key: The key value
 * @mods: The modifier mask
 *
 * Converts an accelerator to a string.
 *
 * Returns: (transfer full): The accelerator string
 */
gchar *mate_ui_accel_to_string(guint           key,
                                GdkModifierType mods);

/**
 * mate_ui_accel_label_set_accel:
 * @label: A #GtkAccelLabel
 * @accel: The accelerator string
 *
 * Sets the accelerator shown by an accel label.
 */
void mate_ui_accel_label_set_accel(GtkAccelLabel *label,
                                    const gchar   *accel);

/**
 * mate_ui_accel_connect_to_widget:
 * @widget: A widget
 * @accel: The accelerator string
 * @signal_name: Signal to emit (e.g., "clicked")
 *
 * Connects an accelerator to emit a signal on a widget.
 */
void mate_ui_accel_connect_to_widget(GtkWidget   *widget,
                                      const gchar *accel,
                                      const gchar *signal_name);

/**
 * mate_ui_accel_set_app_accels:
 * @app: A #GtkApplication
 * @entries: Array of #MateUiAccelEntry
 * @n_entries: Number of entries
 *
 * Sets multiple application accelerators at once.
 */
void mate_ui_accel_set_app_accels(GtkApplication         *app,
                                   const MateUiAccelEntry *entries,
                                   gsize                   n_entries);

/**
 * mate_ui_accel_clear_app_accels:
 * @app: A #GtkApplication
 * @action_name: The action name
 *
 * Clears all accelerators for an action.
 */
void mate_ui_accel_clear_app_accels(GtkApplication *app,
                                     const gchar    *action_name);

G_END_DECLS

#endif /* MATE_UI_ACCEL_H */
