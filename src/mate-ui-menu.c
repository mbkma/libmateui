/*
 * mate-ui-menu.c - Menu building helpers for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "config.h"
#include "mate-ui-menu.h"


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
GtkWidget *
mate_ui_menu_item_new_with_action(const gchar   *label,
                                   const gchar   *action_name,
                                   GtkAccelGroup *accel_group,
                                   const gchar   *accel)
{
    g_return_val_if_fail(label != NULL, NULL);
    g_return_val_if_fail(action_name != NULL, NULL);

    GtkWidget *item = gtk_menu_item_new_with_mnemonic(label);
    gtk_actionable_set_action_name(GTK_ACTIONABLE(item), action_name);

    if (accel != NULL && accel_group != NULL)
    {
        guint key;
        GdkModifierType mods;
        gtk_accelerator_parse(accel, &key, &mods);
        if (key != 0)
        {
            gtk_widget_add_accelerator(item, "activate", accel_group,
                                        key, mods, GTK_ACCEL_VISIBLE);
        }
    }

    return item;
}

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
GtkWidget *
mate_ui_menu_item_new_with_icon(const gchar *label,
                                 const gchar *icon_name,
                                 const gchar *action_name)
{
    g_return_val_if_fail(label != NULL, NULL);
    g_return_val_if_fail(action_name != NULL, NULL);

    GtkWidget *item = gtk_menu_item_new();

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_margin_start(box, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_START);

    if (icon_name != NULL)
    {
        GtkWidget *image = gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_MENU);
        gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 0);
        gtk_widget_set_valign(image, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_start(image, 0);
        gtk_widget_set_margin_end(image, 8);
    }

    GtkWidget *lbl = gtk_label_new_with_mnemonic(label);
    gtk_label_set_xalign(GTK_LABEL(lbl), 0.0);
    gtk_widget_set_margin_start(lbl, 0);
    gtk_widget_set_margin_end(lbl, 0);
    gtk_box_pack_start(GTK_BOX(box), lbl, TRUE, TRUE, 0);

#if GTK_CHECK_VERSION(4,0,0)
    gtk_menu_item_set_child(GTK_MENU_ITEM(item), box);
#else
    gtk_container_add(GTK_CONTAINER(item), box);
#endif

    gtk_actionable_set_action_name(GTK_ACTIONABLE(item), action_name);
    gtk_widget_show_all(item);

    return item;
}

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
GtkWidget *
mate_ui_menu_new_from_entries(const MateUiMenuEntry *entries,
                               gsize                  n_entries,
                               GtkAccelGroup         *accel_group)
{
    g_return_val_if_fail(entries != NULL || n_entries == 0, NULL);

    GtkWidget *menu = gtk_menu_new();
#if GTK_CHECK_VERSION(4,0,0)
#else
    gtk_menu_set_reserve_toggle_size(GTK_MENU(menu), FALSE);
#endif

    for (gsize i = 0; i < n_entries; i++)
    {
        const MateUiMenuEntry *entry = &entries[i];
        GtkWidget *item;

        /* Check for separator */
        if (entry->label == NULL && entry->action_name == NULL)
        {
            item = gtk_separator_menu_item_new();
        }
        else if (entry->icon_name != NULL)
        {
            item = mate_ui_menu_item_new_with_icon(entry->label,
                                                    entry->icon_name,
                                                    entry->action_name);
            if (entry->accel != NULL && accel_group != NULL)
            {
                guint key;
                GdkModifierType mods;
                gtk_accelerator_parse(entry->accel, &key, &mods);
                if (key != 0)
                {
                    gtk_widget_add_accelerator(item, "activate", accel_group,
                                                key, mods, GTK_ACCEL_VISIBLE);
                }
            }
        }
        else
        {
            item = mate_ui_menu_item_new_with_action(entry->label,
                                                      entry->action_name,
                                                      accel_group,
                                                      entry->accel);
        }

        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
        gtk_widget_show(item);
    }

    return menu;
}

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
GtkWidget *
mate_ui_menu_bar_new_from_entries(const MateUiSubmenu *submenus,
                                   gsize                n_submenus,
                                   GtkAccelGroup       *accel_group)
{
    g_return_val_if_fail(submenus != NULL || n_submenus == 0, NULL);

    GtkWidget *menubar = gtk_menu_bar_new();

    for (gsize i = 0; i < n_submenus; i++)
    {
        const MateUiSubmenu *submenu = &submenus[i];

        GtkWidget *menu_item = gtk_menu_item_new_with_mnemonic(submenu->label);
        GtkWidget *menu = mate_ui_menu_new_from_entries(submenu->entries,
                                                         submenu->n_entries,
                                                         accel_group);

        gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menu_item);
        gtk_widget_show(menu_item);
    }

    return menubar;
}

/**
 * mate_ui_menu_model_new_from_entries:
 * @submenus: Array of #MateUiSubmenu structures
 * @n_submenus: Number of submenus
 *
 * Creates a GMenuModel from an array of submenu definitions.
 *
 * Returns: (transfer full): A new #GMenuModel
 */
GMenuModel *
mate_ui_menu_model_new_from_entries(const MateUiSubmenu *submenus,
                                     gsize                n_submenus)
{
    g_return_val_if_fail(submenus != NULL || n_submenus == 0, NULL);

    GMenu *menubar = g_menu_new();

    for (gsize i = 0; i < n_submenus; i++)
    {
        const MateUiSubmenu *submenu = &submenus[i];
        GMenu *menu = g_menu_new();

        for (gsize j = 0; j < submenu->n_entries; j++)
        {
            const MateUiMenuEntry *entry = &submenu->entries[j];

            /* Check for separator */
            if (entry->label == NULL && entry->action_name == NULL)
            {
                /* GMenu doesn't have explicit separators; create a section break */
                GMenu *section = g_menu_new();
                g_menu_append_section(menu, NULL, G_MENU_MODEL(section));
                g_object_unref(section);
            }
            else
            {
                /* Remove mnemonic underscore for GMenu */
                gchar *label = g_strdup(entry->label);
                gchar *p = label;
                gchar *q = label;
                while (*p)
                {
                    if (*p != '_')
                        *q++ = *p;
                    p++;
                }
                *q = '\0';

                GMenuItem *item = g_menu_item_new(label, entry->action_name);
                g_free(label);

                if (entry->icon_name != NULL)
                {
                    GIcon *icon = g_themed_icon_new(entry->icon_name);
                    g_menu_item_set_icon(item, icon);
                    g_object_unref(icon);
                }

                if (entry->accel != NULL)
                {
                    g_menu_item_set_attribute(item, "accel", "s", entry->accel);
                }

                g_menu_append_item(menu, item);
                g_object_unref(item);
            }
        }

        /* Remove mnemonic underscore from submenu label */
        gchar *sublabel = g_strdup(submenu->label);
        gchar *p = sublabel;
        gchar *q = sublabel;
        while (*p)
        {
            if (*p != '_')
                *q++ = *p;
            p++;
        }
        *q = '\0';

        g_menu_append_submenu(menubar, sublabel, G_MENU_MODEL(menu));
        g_free(sublabel);
        g_object_unref(menu);
    }

    return G_MENU_MODEL(menubar);
}

/**
 * mate_ui_menu_add_recent_chooser:
 * @menu: A #GtkMenu
 * @label: The submenu label
 * @filter: (nullable): A #GtkRecentFilter or %NULL
 * @callback: Callback when a recent item is activated
 * @user_data: User data for callback
 *
 * Adds a recent files submenu to the given menu.
 *
 * Returns: (transfer none): The recent chooser menu item
 */
GtkWidget *
mate_ui_menu_add_recent_chooser(GtkMenu         *menu,
                                 const gchar     *label,
                                 GtkRecentFilter *filter,
                                 GCallback        callback,
                                 gpointer         user_data)
{
    g_return_val_if_fail(GTK_IS_MENU(menu), NULL);
    g_return_val_if_fail(label != NULL, NULL);

    GtkWidget *item = gtk_menu_item_new_with_mnemonic(label);

    GtkRecentManager *manager = gtk_recent_manager_get_default();
    GtkWidget *recent_menu = gtk_recent_chooser_menu_new_for_manager(manager);

    gtk_recent_chooser_set_show_tips(GTK_RECENT_CHOOSER(recent_menu), TRUE);
    gtk_recent_chooser_set_sort_type(GTK_RECENT_CHOOSER(recent_menu),
                                      GTK_RECENT_SORT_MRU);
    gtk_recent_chooser_set_limit(GTK_RECENT_CHOOSER(recent_menu), 10);
    gtk_recent_chooser_set_local_only(GTK_RECENT_CHOOSER(recent_menu), FALSE);

    if (filter != NULL)
    {
        gtk_recent_chooser_add_filter(GTK_RECENT_CHOOSER(recent_menu), filter);
        gtk_recent_chooser_set_filter(GTK_RECENT_CHOOSER(recent_menu), filter);
    }

    if (callback != NULL)
    {
        g_signal_connect(recent_menu, "item-activated", callback, user_data);
    }

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), recent_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);

    return item;
}

/**
 * mate_ui_popup_menu_at_pointer:
 * @menu: A #GtkMenu
 * @event: (nullable): The triggering event or %NULL
 *
 * Shows a popup menu at the pointer position.
 */
void
mate_ui_popup_menu_at_pointer(GtkMenu        *menu,
                               const GdkEvent *event)
{
    g_return_if_fail(GTK_IS_MENU(menu));

    gtk_menu_popup_at_pointer(menu, event);
}

/**
 * mate_ui_popup_menu_at_widget:
 * @menu: A #GtkMenu
 * @widget: The widget to popup at
 * @widget_anchor: Anchor point on widget
 * @menu_anchor: Anchor point on menu
 *
 * Shows a popup menu anchored to a widget.
 */
void
mate_ui_popup_menu_at_widget(GtkMenu    *menu,
                              GtkWidget  *widget,
                              GdkGravity  widget_anchor,
                              GdkGravity  menu_anchor)
{
    g_return_if_fail(GTK_IS_MENU(menu));
    g_return_if_fail(GTK_IS_WIDGET(widget));

    gtk_menu_popup_at_widget(menu, widget, widget_anchor, menu_anchor, NULL);
}

/**
 * mate_ui_context_menu_new:
 *
 * Creates a new context menu (GtkMenu).
 *
 * Returns: (transfer full): A new #GtkMenu
 */
GtkWidget *
mate_ui_context_menu_new(void)
{
    return gtk_menu_new();
}

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
GtkWidget *
mate_ui_context_menu_add_item(GtkMenu     *menu,
                               const gchar *label,
                               GCallback    callback,
                               gpointer     user_data)
{
    g_return_val_if_fail(GTK_IS_MENU(menu), NULL);
    g_return_val_if_fail(label != NULL, NULL);

    GtkWidget *item = gtk_menu_item_new_with_mnemonic(label);

    if (callback != NULL)
    {
        g_signal_connect(item, "activate", callback, user_data);
    }

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);

    return item;
}

/**
 * mate_ui_context_menu_add_separator:
 * @menu: A #GtkMenu
 *
 * Adds a separator to a context menu.
 */
void
mate_ui_context_menu_add_separator(GtkMenu *menu)
{
    g_return_if_fail(GTK_IS_MENU(menu));

    GtkWidget *sep = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), sep);
    gtk_widget_show(sep);
}
