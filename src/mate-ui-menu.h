/*
 * mate-ui-menu.h - Menu building helpers for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef MATE_UI_MENU_H
#define MATE_UI_MENU_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * MateUiMenuEntry:
 * @label: The menu item label (with mnemonic, e.g., "_File")
 * @action_name: The action name (e.g., "app.quit" or "win.save")
 * @accel: The accelerator key (e.g., "<Control>q") or %NULL
 * @icon_name: An icon name or %NULL
 *
 * Structure describing a single menu item.
 */
typedef struct
{
    const gchar *label;
    const gchar *action_name;
    const gchar *accel;
    const gchar *icon_name;
} MateUiMenuEntry;

/**
 * MateUiSubmenu:
 * @label: The submenu label (with mnemonic)
 * @entries: Array of #MateUiMenuEntry items
 * @n_entries: Number of entries
 *
 * Structure describing a submenu.
 */
typedef struct
{
    const gchar            *label;
    const MateUiMenuEntry  *entries;
    gsize                   n_entries;
} MateUiSubmenu;

/**
 * MATE_UI_MENU_SEPARATOR:
 *
 * Use as a menu entry to insert a separator.
 */
#define MATE_UI_MENU_SEPARATOR { NULL, NULL, NULL, NULL }

/**
 * mate_ui_menu_bar_new_from_entries:
 * @submenus: Array of #MateUiSubmenu structures
 * @n_submenus: Number of submenus
 * @accel_group: (nullable): Accelerator group to attach, or %NULL
 *
 * Creates a GtkMenuBar from an array of submenu definitions.
 *
 * Returns: (transfer full): A new #GtkMenuBar
 */
GtkWidget *mate_ui_menu_bar_new_from_entries(const MateUiSubmenu *submenus,
                                              gsize                n_submenus,
                                              GtkAccelGroup       *accel_group);

/**
 * mate_ui_menu_new_from_entries:
 * @entries: Array of #MateUiMenuEntry structures
 * @n_entries: Number of entries
 * @accel_group: (nullable): Accelerator group to attach, or %NULL
 *
 * Creates a GtkMenu from an array of menu entry definitions.
 *
 * Returns: (transfer full): A new #GtkMenu
 */
GtkWidget *mate_ui_menu_new_from_entries(const MateUiMenuEntry *entries,
                                          gsize                  n_entries,
                                          GtkAccelGroup         *accel_group);

/**
 * mate_ui_menu_model_new_from_entries:
 * @submenus: Array of #MateUiSubmenu structures
 * @n_submenus: Number of submenus
 *
 * Creates a GMenuModel from an array of submenu definitions.
 * This is useful for GtkApplication menu integration.
 *
 * Returns: (transfer full): A new #GMenuModel
 */
GMenuModel *mate_ui_menu_model_new_from_entries(const MateUiSubmenu *submenus,
                                                 gsize                n_submenus);

/**
 * mate_ui_menu_item_new_with_action:
 * @label: The menu item label
 * @action_name: The action name
 * @accel_group: (nullable): Accelerator group
 * @accel: (nullable): Accelerator string (e.g., "<Control>s")
 *
 * Creates a menu item connected to an action with optional accelerator.
 *
 * Returns: (transfer full): A new #GtkMenuItem
 */
GtkWidget *mate_ui_menu_item_new_with_action(const gchar   *label,
                                              const gchar   *action_name,
                                              GtkAccelGroup *accel_group,
                                              const gchar   *accel);

/**
 * mate_ui_menu_item_new_with_icon:
 * @label: The menu item label
 * @icon_name: The icon name
 * @action_name: The action name
 *
 * Creates a menu item with an icon.
 *
 * Returns: (transfer full): A new #GtkMenuItem with icon
 */
GtkWidget *mate_ui_menu_item_new_with_icon(const gchar *label,
                                            const gchar *icon_name,
                                            const gchar *action_name);

/**
 * mate_ui_menu_add_recent_chooser:
 * @menu: A #GtkMenu
 * @label: The submenu label (e.g., "Open _Recent")
 * @filter: (nullable): A #GtkRecentFilter or %NULL
 * @callback: Callback when a recent item is activated
 * @user_data: User data for callback
 *
 * Adds a recent files submenu to the given menu.
 *
 * Returns: (transfer none): The recent chooser menu item
 */
GtkWidget *mate_ui_menu_add_recent_chooser(GtkMenu             *menu,
                                            const gchar         *label,
                                            GtkRecentFilter     *filter,
                                            GCallback            callback,
                                            gpointer             user_data);

/**
 * mate_ui_popup_menu_at_pointer:
 * @menu: A #GtkMenu
 * @event: (nullable): The triggering event or %NULL
 *
 * Shows a popup menu at the pointer position.
 */
void mate_ui_popup_menu_at_pointer(GtkMenu        *menu,
                                    const GdkEvent *event);

/**
 * mate_ui_popup_menu_at_widget:
 * @menu: A #GtkMenu
 * @widget: The widget to popup at
 * @widget_anchor: Anchor point on widget
 * @menu_anchor: Anchor point on menu
 *
 * Shows a popup menu anchored to a widget.
 */
void mate_ui_popup_menu_at_widget(GtkMenu         *menu,
                                   GtkWidget       *widget,
                                   GdkGravity       widget_anchor,
                                   GdkGravity       menu_anchor);

/**
 * mate_ui_context_menu_new:
 *
 * Creates a new context menu (GtkMenu).
 *
 * Returns: (transfer full): A new #GtkMenu
 */
GtkWidget *mate_ui_context_menu_new(void);

/**
 * mate_ui_context_menu_add_item:
 * @menu: A #GtkMenu
 * @label: The item label
 * @callback: Activation callback
 * @user_data: User data for callback
 *
 * Adds an item to a context menu.
 *
 * Returns: (transfer none): The new menu item
 */
GtkWidget *mate_ui_context_menu_add_item(GtkMenu     *menu,
                                          const gchar *label,
                                          GCallback    callback,
                                          gpointer     user_data);

/**
 * mate_ui_context_menu_add_separator:
 * @menu: A #GtkMenu
 *
 * Adds a separator to a context menu.
 */
void mate_ui_context_menu_add_separator(GtkMenu *menu);

G_END_DECLS

#endif /* MATE_UI_MENU_H */
