/*
 * mate-ui-settings.h - GSettings binding helpers for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef MATE_UI_SETTINGS_H
#define MATE_UI_SETTINGS_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * MateUiSettingsBinding:
 * @widget: The widget to bind
 * @property: The widget property name
 * @key: The GSettings key name
 * @flags: Binding flags
 *
 * Structure describing a widget-to-settings binding.
 */
typedef struct
{
    GtkWidget       *widget;
    const gchar     *property;
    const gchar     *key;
    GSettingsBindFlags flags;
} MateUiSettingsBinding;

/**
 * mate_ui_settings_bind:
 * @settings: A #GSettings object
 * @key: The settings key
 * @widget: The widget to bind
 * @property: The widget property
 * @flags: Binding flags
 *
 * Binds a GSettings key to a widget property.
 */
void mate_ui_settings_bind(GSettings          *settings,
                            const gchar        *key,
                            GtkWidget          *widget,
                            const gchar        *property,
                            GSettingsBindFlags  flags);

/**
 * mate_ui_settings_bind_with_mapping:
 * @settings: A #GSettings object
 * @key: The settings key
 * @widget: The widget to bind
 * @property: The widget property
 * @flags: Binding flags
 * @get_mapping: Function to transform from settings to property
 * @set_mapping: Function to transform from property to settings
 * @user_data: User data for mapping functions
 * @destroy: Destroy notify for user data
 *
 * Binds a GSettings key to a widget property with custom mapping functions.
 */
void mate_ui_settings_bind_with_mapping(GSettings               *settings,
                                         const gchar             *key,
                                         GtkWidget               *widget,
                                         const gchar             *property,
                                         GSettingsBindFlags       flags,
                                         GSettingsBindGetMapping  get_mapping,
                                         GSettingsBindSetMapping  set_mapping,
                                         gpointer                 user_data,
                                         GDestroyNotify           destroy);

/**
 * mate_ui_settings_bind_multiple:
 * @settings: A #GSettings object
 * @bindings: Array of #MateUiSettingsBinding structures
 * @n_bindings: Number of bindings
 *
 * Binds multiple widget properties to GSettings keys at once.
 */
void mate_ui_settings_bind_multiple(GSettings                    *settings,
                                     const MateUiSettingsBinding  *bindings,
                                     gsize                         n_bindings);

/**
 * mate_ui_settings_bind_spin_button:
 * @settings: A #GSettings object
 * @key: The settings key
 * @spin_button: A #GtkSpinButton
 *
 * Binds a GSettings integer/double key to a spin button value.
 */
void mate_ui_settings_bind_spin_button(GSettings     *settings,
                                        const gchar   *key,
                                        GtkSpinButton *spin_button);

/**
 * mate_ui_settings_bind_switch:
 * @settings: A #GSettings object
 * @key: The settings key
 * @switch_widget: A #GtkSwitch
 *
 * Binds a GSettings boolean key to a switch widget.
 */
void mate_ui_settings_bind_switch(GSettings   *settings,
                                   const gchar *key,
                                   GtkSwitch   *switch_widget);

/**
 * mate_ui_settings_bind_check_button:
 * @settings: A #GSettings object
 * @key: The settings key
 * @check_button: A #GtkCheckButton
 *
 * Binds a GSettings boolean key to a check button.
 */
void mate_ui_settings_bind_check_button(GSettings      *settings,
                                         const gchar    *key,
                                         GtkCheckButton *check_button);

/**
 * mate_ui_settings_bind_entry:
 * @settings: A #GSettings object
 * @key: The settings key
 * @entry: A #GtkEntry
 *
 * Binds a GSettings string key to an entry widget.
 */
void mate_ui_settings_bind_entry(GSettings   *settings,
                                  const gchar *key,
                                  GtkEntry    *entry);

/**
 * mate_ui_settings_bind_combo_box:
 * @settings: A #GSettings object
 * @key: The settings key
 * @combo_box: A #GtkComboBox
 * @model_column: The column in the model containing the value
 *
 * Binds a GSettings key to a combo box active item.
 */
void mate_ui_settings_bind_combo_box(GSettings   *settings,
                                      const gchar *key,
                                      GtkComboBox *combo_box,
                                      gint         model_column);

/**
 * mate_ui_settings_bind_font_button:
 * @settings: A #GSettings object
 * @key: The settings key
 * @font_button: A #GtkFontButton
 *
 * Binds a GSettings string key to a font button.
 */
void mate_ui_settings_bind_font_button(GSettings     *settings,
                                        const gchar   *key,
                                        GtkFontButton *font_button);

/**
 * mate_ui_settings_bind_color_button:
 * @settings: A #GSettings object
 * @key: The settings key (expects string like "#RRGGBB")
 * @color_button: A #GtkColorButton
 *
 * Binds a GSettings string key to a color button.
 */
void mate_ui_settings_bind_color_button(GSettings      *settings,
                                         const gchar    *key,
                                         GtkColorButton *color_button);

/**
 * mate_ui_settings_bind_file_chooser_button:
 * @settings: A #GSettings object
 * @key: The settings key
 * @file_chooser: A #GtkFileChooserButton
 *
 * Binds a GSettings string key to a file chooser button.
 */
void mate_ui_settings_bind_file_chooser_button(GSettings            *settings,
                                                const gchar          *key,
                                                GtkFileChooserButton *file_chooser);

/**
 * mate_ui_settings_bind_scale:
 * @settings: A #GSettings object
 * @key: The settings key
 * @scale: A #GtkScale
 *
 * Binds a GSettings numeric key to a scale widget.
 */
void mate_ui_settings_bind_scale(GSettings   *settings,
                                  const gchar *key,
                                  GtkScale    *scale);

/**
 * mate_ui_settings_delayed_apply:
 * @settings: A #GSettings object
 *
 * Puts settings into delayed mode where changes are cached
 * until mate_ui_settings_apply() is called.
 */
void mate_ui_settings_delayed_apply(GSettings *settings);

/**
 * mate_ui_settings_apply:
 * @settings: A #GSettings object
 *
 * Applies all pending changes from delayed mode.
 */
void mate_ui_settings_apply(GSettings *settings);

/**
 * mate_ui_settings_revert:
 * @settings: A #GSettings object
 *
 * Reverts all pending changes in delayed mode.
 */
void mate_ui_settings_revert(GSettings *settings);

G_END_DECLS

#endif /* MATE_UI_SETTINGS_H */
