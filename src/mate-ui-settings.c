/*
 * mate-ui-settings.c - GSettings binding helpers for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "config.h"
#include "mate-ui-settings.h"

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
void
mate_ui_settings_bind(GSettings          *settings,
                       const gchar        *key,
                       GtkWidget          *widget,
                       const gchar        *property,
                       GSettingsBindFlags  flags)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(key != NULL);
    g_return_if_fail(GTK_IS_WIDGET(widget));
    g_return_if_fail(property != NULL);

    g_settings_bind(settings, key, widget, property, flags);
}

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
void
mate_ui_settings_bind_with_mapping(GSettings               *settings,
                                    const gchar             *key,
                                    GtkWidget               *widget,
                                    const gchar             *property,
                                    GSettingsBindFlags       flags,
                                    GSettingsBindGetMapping  get_mapping,
                                    GSettingsBindSetMapping  set_mapping,
                                    gpointer                 user_data,
                                    GDestroyNotify           destroy)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(key != NULL);
    g_return_if_fail(GTK_IS_WIDGET(widget));
    g_return_if_fail(property != NULL);

    g_settings_bind_with_mapping(settings, key, widget, property, flags,
                                  get_mapping, set_mapping, user_data, destroy);
}

/**
 * mate_ui_settings_bind_multiple:
 * @settings: A #GSettings object
 * @bindings: Array of #MateUiSettingsBinding structures
 * @n_bindings: Number of bindings
 *
 * Binds multiple widget properties to GSettings keys at once.
 */
void
mate_ui_settings_bind_multiple(GSettings                    *settings,
                                const MateUiSettingsBinding  *bindings,
                                gsize                         n_bindings)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(bindings != NULL || n_bindings == 0);

    for (gsize i = 0; i < n_bindings; i++)
    {
        const MateUiSettingsBinding *b = &bindings[i];
        g_settings_bind(settings, b->key, b->widget, b->property, b->flags);
    }
}

/**
 * mate_ui_settings_bind_spin_button:
 * @settings: A #GSettings object
 * @key: The settings key
 * @spin_button: A #GtkSpinButton
 *
 * Binds a GSettings integer/double key to a spin button value.
 */
void
mate_ui_settings_bind_spin_button(GSettings     *settings,
                                   const gchar   *key,
                                   GtkSpinButton *spin_button)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(key != NULL);
    g_return_if_fail(GTK_IS_SPIN_BUTTON(spin_button));

    g_settings_bind(settings, key, spin_button, "value",
                     G_SETTINGS_BIND_DEFAULT);
}

/**
 * mate_ui_settings_bind_switch:
 * @settings: A #GSettings object
 * @key: The settings key
 * @switch_widget: A #GtkSwitch
 *
 * Binds a GSettings boolean key to a switch widget.
 */
void
mate_ui_settings_bind_switch(GSettings   *settings,
                              const gchar *key,
                              GtkSwitch   *switch_widget)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(key != NULL);
    g_return_if_fail(GTK_IS_SWITCH(switch_widget));

    g_settings_bind(settings, key, switch_widget, "active",
                     G_SETTINGS_BIND_DEFAULT);
}

/**
 * mate_ui_settings_bind_check_button:
 * @settings: A #GSettings object
 * @key: The settings key
 * @check_button: A #GtkCheckButton
 *
 * Binds a GSettings boolean key to a check button.
 */
void
mate_ui_settings_bind_check_button(GSettings      *settings,
                                    const gchar    *key,
                                    GtkCheckButton *check_button)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(key != NULL);
    g_return_if_fail(GTK_IS_CHECK_BUTTON(check_button));

    g_settings_bind(settings, key, check_button, "active",
                     G_SETTINGS_BIND_DEFAULT);
}

/**
 * mate_ui_settings_bind_entry:
 * @settings: A #GSettings object
 * @key: The settings key
 * @entry: A #GtkEntry
 *
 * Binds a GSettings string key to an entry widget.
 */
void
mate_ui_settings_bind_entry(GSettings   *settings,
                             const gchar *key,
                             GtkEntry    *entry)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(key != NULL);
    g_return_if_fail(GTK_IS_ENTRY(entry));

    g_settings_bind(settings, key, entry, "text",
                     G_SETTINGS_BIND_DEFAULT);
}

/* Helper for combo box string binding */
typedef struct
{
    gint column;
} ComboBoxBindingData;

static gboolean
combo_box_get_mapping(GValue   *value,
                       GVariant *variant,
                       gpointer  user_data)
{
    ComboBoxBindingData *data = user_data;
    const gchar *str = g_variant_get_string(variant, NULL);

    GtkComboBox *combo = GTK_COMBO_BOX(g_value_get_object(value));
    GtkTreeModel *model = gtk_combo_box_get_model(combo);
    GtkTreeIter iter;
    gboolean valid;

    valid = gtk_tree_model_get_iter_first(model, &iter);
    gint index = 0;

    while (valid)
    {
        gchar *item_str = NULL;
        gtk_tree_model_get(model, &iter, data->column, &item_str, -1);

        if (g_strcmp0(item_str, str) == 0)
        {
            g_free(item_str);
            g_value_set_int(value, index);
            return TRUE;
        }

        g_free(item_str);
        valid = gtk_tree_model_iter_next(model, &iter);
        index++;
    }

    return FALSE;
}

static GVariant *
combo_box_set_mapping(const GValue       *value,
                       const GVariantType *expected_type G_GNUC_UNUSED,
                       gpointer            user_data)
{
    ComboBoxBindingData *data = user_data;
    GtkComboBox *combo = GTK_COMBO_BOX(g_value_get_object(value));
    GtkTreeModel *model = gtk_combo_box_get_model(combo);
    GtkTreeIter iter;

    if (gtk_combo_box_get_active_iter(combo, &iter))
    {
        gchar *str = NULL;
        gtk_tree_model_get(model, &iter, data->column, &str, -1);
        GVariant *result = g_variant_new_string(str ? str : "");
        g_free(str);
        return result;
    }

    return g_variant_new_string("");
}

/**
 * mate_ui_settings_bind_combo_box:
 * @settings: A #GSettings object
 * @key: The settings key
 * @combo_box: A #GtkComboBox
 * @model_column: The column in the model containing the value
 *
 * Binds a GSettings key to a combo box active item.
 */
void
mate_ui_settings_bind_combo_box(GSettings   *settings,
                                 const gchar *key,
                                 GtkComboBox *combo_box,
                                 gint         model_column)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(key != NULL);
    g_return_if_fail(GTK_IS_COMBO_BOX(combo_box));

    /* Check if this is a simple integer index binding */
    GVariant *value = g_settings_get_value(settings, key);
    if (g_variant_is_of_type(value, G_VARIANT_TYPE_INT32) ||
        g_variant_is_of_type(value, G_VARIANT_TYPE_UINT32))
    {
        g_variant_unref(value);
        g_settings_bind(settings, key, combo_box, "active",
                         G_SETTINGS_BIND_DEFAULT);
        return;
    }
    g_variant_unref(value);

    /* For string values, use mapping */
    ComboBoxBindingData *data = g_new0(ComboBoxBindingData, 1);
    data->column = model_column;

    g_settings_bind_with_mapping(settings, key, combo_box, "active",
                                  G_SETTINGS_BIND_DEFAULT,
                                  combo_box_get_mapping,
                                  combo_box_set_mapping,
                                  data, g_free);
}

/**
 * mate_ui_settings_bind_font_button:
 * @settings: A #GSettings object
 * @key: The settings key
 * @font_button: A #GtkFontButton
 *
 * Binds a GSettings string key to a font button.
 */
void
mate_ui_settings_bind_font_button(GSettings     *settings,
                                   const gchar   *key,
                                   GtkFontButton *font_button)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(key != NULL);
    g_return_if_fail(GTK_IS_FONT_BUTTON(font_button));

    g_settings_bind(settings, key, font_button, "font",
                     G_SETTINGS_BIND_DEFAULT);
}

/* Color button binding helpers */
static gboolean
color_get_mapping(GValue   *value,
                   GVariant *variant,
                   gpointer  user_data G_GNUC_UNUSED)
{
    const gchar *str = g_variant_get_string(variant, NULL);
    GdkRGBA color;

    if (gdk_rgba_parse(&color, str))
    {
        g_value_set_boxed(value, &color);
        return TRUE;
    }

    return FALSE;
}

static GVariant *
color_set_mapping(const GValue       *value,
                   const GVariantType *expected_type G_GNUC_UNUSED,
                   gpointer            user_data G_GNUC_UNUSED)
{
    GdkRGBA *color = g_value_get_boxed(value);
    if (color != NULL)
    {
        gchar *str = gdk_rgba_to_string(color);
        GVariant *result = g_variant_new_string(str);
        g_free(str);
        return result;
    }

    return g_variant_new_string("#000000");
}

/**
 * mate_ui_settings_bind_color_button:
 * @settings: A #GSettings object
 * @key: The settings key
 * @color_button: A #GtkColorButton
 *
 * Binds a GSettings string key to a color button.
 */
void
mate_ui_settings_bind_color_button(GSettings      *settings,
                                    const gchar    *key,
                                    GtkColorButton *color_button)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(key != NULL);
    g_return_if_fail(GTK_IS_COLOR_BUTTON(color_button));

    g_settings_bind_with_mapping(settings, key, color_button, "rgba",
                                  G_SETTINGS_BIND_DEFAULT,
                                  color_get_mapping,
                                  color_set_mapping,
                                  NULL, NULL);
}

/* File chooser button binding helpers */
static gboolean
file_chooser_get_mapping(GValue   *value,
                          GVariant *variant,
                          gpointer  user_data G_GNUC_UNUSED)
{
    const gchar *path = g_variant_get_string(variant, NULL);
    if (path != NULL && *path != '\0')
    {
        g_value_take_object(value, g_file_new_for_path(path));
        return TRUE;
    }
    return FALSE;
}

static GVariant *
file_chooser_set_mapping(const GValue       *value,
                          const GVariantType *expected_type G_GNUC_UNUSED,
                          gpointer            user_data G_GNUC_UNUSED)
{
    GFile *file = g_value_get_object(value);
    if (file != NULL)
    {
        gchar *path = g_file_get_path(file);
        GVariant *result = g_variant_new_string(path ? path : "");
        g_free(path);
        return result;
    }

    return g_variant_new_string("");
}

/**
 * mate_ui_settings_bind_file_chooser_button:
 * @settings: A #GSettings object
 * @key: The settings key
 * @file_chooser: A #GtkFileChooserButton
 *
 * Binds a GSettings string key to a file chooser button.
 */
void
mate_ui_settings_bind_file_chooser_button(GSettings            *settings,
                                           const gchar          *key,
                                           GtkFileChooserButton *file_chooser)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(key != NULL);
    g_return_if_fail(GTK_IS_FILE_CHOOSER_BUTTON(file_chooser));

    g_settings_bind_with_mapping(settings, key, file_chooser, "file",
                                  G_SETTINGS_BIND_DEFAULT,
                                  file_chooser_get_mapping,
                                  file_chooser_set_mapping,
                                  NULL, NULL);
}

/**
 * mate_ui_settings_bind_scale:
 * @settings: A #GSettings object
 * @key: The settings key
 * @scale: A #GtkScale
 *
 * Binds a GSettings numeric key to a scale widget.
 */
void
mate_ui_settings_bind_scale(GSettings   *settings,
                             const gchar *key,
                             GtkScale    *scale)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_return_if_fail(key != NULL);
    g_return_if_fail(GTK_IS_SCALE(scale));

    GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(scale));
    g_settings_bind(settings, key, adj, "value", G_SETTINGS_BIND_DEFAULT);
}

/**
 * mate_ui_settings_delayed_apply:
 * @settings: A #GSettings object
 *
 * Puts settings into delayed mode.
 */
void
mate_ui_settings_delayed_apply(GSettings *settings)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_settings_delay(settings);
}

/**
 * mate_ui_settings_apply:
 * @settings: A #GSettings object
 *
 * Applies all pending changes from delayed mode.
 */
void
mate_ui_settings_apply(GSettings *settings)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_settings_apply(settings);
}

/**
 * mate_ui_settings_revert:
 * @settings: A #GSettings object
 *
 * Reverts all pending changes in delayed mode.
 */
void
mate_ui_settings_revert(GSettings *settings)
{
    g_return_if_fail(G_IS_SETTINGS(settings));
    g_settings_revert(settings);
}
