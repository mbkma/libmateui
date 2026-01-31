/*
 * mate-ui-dialogs.h - Standard dialog helpers for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef MATE_UI_DIALOGS_H
#define MATE_UI_DIALOGS_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * MateUiAboutInfo:
 * @program_name: The program name
 * @version: The version string
 * @copyright: Copyright notice
 * @comments: Short description
 * @license: License text or %NULL for GPL-2.0+
 * @website: Website URL or %NULL
 * @website_label: Website link label or %NULL
 * @authors: %NULL-terminated array of author strings
 * @documenters: %NULL-terminated array of documenter strings, or %NULL
 * @translator_credits: Translator credits string, or %NULL
 * @artists: %NULL-terminated array of artist strings, or %NULL
 * @logo_icon_name: Icon name for logo, or %NULL
 *
 * Structure containing information for the About dialog.
 */
typedef struct
{
    const gchar  *program_name;
    const gchar  *version;
    const gchar  *copyright;
    const gchar  *comments;
    const gchar  *license;
    const gchar  *website;
    const gchar  *website_label;
    const gchar **authors;
    const gchar **documenters;
    const gchar  *translator_credits;
    const gchar **artists;
    const gchar  *logo_icon_name;
} MateUiAboutInfo;

/**
 * MateUiLicenseType:
 * @MATE_UI_LICENSE_GPL_2_0: GPL version 2.0
 * @MATE_UI_LICENSE_GPL_3_0: GPL version 3.0
 * @MATE_UI_LICENSE_LGPL_2_1: LGPL version 2.1
 * @MATE_UI_LICENSE_LGPL_3_0: LGPL version 3.0
 * @MATE_UI_LICENSE_BSD: BSD license
 * @MATE_UI_LICENSE_MIT: MIT license
 * @MATE_UI_LICENSE_CUSTOM: Custom license text
 *
 * Pre-defined license types for About dialogs.
 */
typedef enum
{
    MATE_UI_LICENSE_GPL_2_0,
    MATE_UI_LICENSE_GPL_3_0,
    MATE_UI_LICENSE_LGPL_2_1,
    MATE_UI_LICENSE_LGPL_3_0,
    MATE_UI_LICENSE_BSD,
    MATE_UI_LICENSE_MIT,
    MATE_UI_LICENSE_CUSTOM,
} MateUiLicenseType;

/**
 * mate_ui_dialog_about_new:
 * @parent: (nullable): Parent window
 * @info: About dialog information
 *
 * Creates and shows an About dialog.
 *
 * Returns: (transfer full): The #GtkAboutDialog
 */
GtkWidget *mate_ui_dialog_about_new(GtkWindow            *parent,
                                     const MateUiAboutInfo *info);

/**
 * mate_ui_dialog_about_new_simple:
 * @parent: (nullable): Parent window
 * @program_name: Program name
 * @version: Version string
 * @copyright: Copyright notice
 * @comments: Short description
 * @logo_icon_name: (nullable): Logo icon name
 *
 * Creates a simple About dialog with minimal information.
 *
 * Returns: (transfer full): The #GtkAboutDialog
 */
GtkWidget *mate_ui_dialog_about_new_simple(GtkWindow   *parent,
                                            const gchar *program_name,
                                            const gchar *version,
                                            const gchar *copyright,
                                            const gchar *comments,
                                            const gchar *logo_icon_name);

/**
 * mate_ui_dialog_message:
 * @parent: (nullable): Parent window
 * @type: Message type
 * @buttons: Button set
 * @primary: Primary message
 * @secondary: (nullable): Secondary message
 *
 * Shows a message dialog and returns the response.
 *
 * Returns: The response ID
 */
gint mate_ui_dialog_message(GtkWindow      *parent,
                             GtkMessageType  type,
                             GtkButtonsType  buttons,
                             const gchar    *primary,
                             const gchar    *secondary);

/**
 * mate_ui_dialog_error:
 * @parent: (nullable): Parent window
 * @primary: Primary error message
 * @secondary: (nullable): Secondary message with details
 *
 * Shows an error dialog.
 */
void mate_ui_dialog_error(GtkWindow   *parent,
                           const gchar *primary,
                           const gchar *secondary);

/**
 * mate_ui_dialog_warning:
 * @parent: (nullable): Parent window
 * @primary: Primary warning message
 * @secondary: (nullable): Secondary message with details
 *
 * Shows a warning dialog.
 */
void mate_ui_dialog_warning(GtkWindow   *parent,
                             const gchar *primary,
                             const gchar *secondary);

/**
 * mate_ui_dialog_info:
 * @parent: (nullable): Parent window
 * @primary: Primary info message
 * @secondary: (nullable): Secondary message with details
 *
 * Shows an information dialog.
 */
void mate_ui_dialog_info(GtkWindow   *parent,
                          const gchar *primary,
                          const gchar *secondary);

/**
 * mate_ui_dialog_question:
 * @parent: (nullable): Parent window
 * @primary: Primary question
 * @secondary: (nullable): Secondary message with details
 *
 * Shows a Yes/No question dialog.
 *
 * Returns: %TRUE if user clicked Yes, %FALSE otherwise
 */
gboolean mate_ui_dialog_question(GtkWindow   *parent,
                                  const gchar *primary,
                                  const gchar *secondary);

/**
 * mate_ui_dialog_confirm:
 * @parent: (nullable): Parent window
 * @primary: Primary message
 * @secondary: (nullable): Secondary message
 * @confirm_label: Label for confirm button
 * @is_destructive: Whether the action is destructive (colors button red)
 *
 * Shows a confirmation dialog with custom button label.
 *
 * Returns: %TRUE if user confirmed, %FALSE otherwise
 */
gboolean mate_ui_dialog_confirm(GtkWindow   *parent,
                                 const gchar *primary,
                                 const gchar *secondary,
                                 const gchar *confirm_label,
                                 gboolean     is_destructive);

/**
 * mate_ui_dialog_file_chooser_open:
 * @parent: (nullable): Parent window
 * @title: Dialog title
 * @filter_name: (nullable): Filter name (e.g., "Text files")
 * @filter_pattern: (nullable): Filter pattern (e.g., "*.txt")
 *
 * Shows a file open dialog.
 *
 * Returns: (transfer full) (nullable): Selected filename or %NULL if cancelled
 */
gchar *mate_ui_dialog_file_chooser_open(GtkWindow   *parent,
                                         const gchar *title,
                                         const gchar *filter_name,
                                         const gchar *filter_pattern);

/**
 * mate_ui_dialog_file_chooser_save:
 * @parent: (nullable): Parent window
 * @title: Dialog title
 * @default_name: (nullable): Default filename
 * @filter_name: (nullable): Filter name
 * @filter_pattern: (nullable): Filter pattern
 *
 * Shows a file save dialog.
 *
 * Returns: (transfer full) (nullable): Selected filename or %NULL if cancelled
 */
gchar *mate_ui_dialog_file_chooser_save(GtkWindow   *parent,
                                         const gchar *title,
                                         const gchar *default_name,
                                         const gchar *filter_name,
                                         const gchar *filter_pattern);

/**
 * mate_ui_dialog_folder_chooser:
 * @parent: (nullable): Parent window
 * @title: Dialog title
 *
 * Shows a folder selection dialog.
 *
 * Returns: (transfer full) (nullable): Selected folder path or %NULL if cancelled
 */
gchar *mate_ui_dialog_folder_chooser(GtkWindow   *parent,
                                      const gchar *title);

/**
 * mate_ui_get_license_text:
 * @license_type: The license type
 *
 * Gets the standard license text for a license type.
 *
 * Returns: (transfer none): The license text
 */
const gchar *mate_ui_get_license_text(MateUiLicenseType license_type);

/**
 * mate_ui_dialogs_show_about:
 * @parent: (nullable): Parent window
 * @program_name: (nullable): Program name
 * @version: (nullable): Version string
 * @copyright: (nullable): Copyright notice
 * @comments: (nullable): Brief description
 * @website: (nullable): Website URL
 * @logo_icon_name: (nullable): Logo icon name
 * @authors: (nullable): NULL-terminated array of authors
 * @documenters: (nullable): NULL-terminated array of documenters
 * @translator_credits: (nullable): Translator credits
 * @license_type: License type
 *
 * Shows an About dialog with the given information.
 */
void mate_ui_dialogs_show_about(GtkWindow          *parent,
                                 const gchar        *program_name,
                                 const gchar        *version,
                                 const gchar        *copyright,
                                 const gchar        *comments,
                                 const gchar        *website,
                                 const gchar        *logo_icon_name,
                                 const gchar *const *authors,
                                 const gchar *const *documenters,
                                 const gchar        *translator_credits,
                                 GtkLicense          license_type);

G_END_DECLS

#endif /* MATE_UI_DIALOGS_H */
