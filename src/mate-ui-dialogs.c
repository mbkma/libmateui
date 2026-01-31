/*
 * mate-ui-dialogs.c - Standard dialog helpers for MATE applications
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "config.h"
#include "mate-ui-dialogs.h"

/* Standard GPL 2.0 license text */
static const gchar *gpl_2_0_text =
    "This program is free software; you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation; either version 2 of the License, or "
    "(at your option) any later version.\n\n"
    "This program is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
    "GNU General Public License for more details.\n\n"
    "You should have received a copy of the GNU General Public License "
    "along with this program; if not, write to the Free Software Foundation, Inc., "
    "51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.";

static const gchar *gpl_3_0_text =
    "This program is free software: you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation, either version 3 of the License, or "
    "(at your option) any later version.\n\n"
    "This program is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
    "GNU General Public License for more details.\n\n"
    "You should have received a copy of the GNU General Public License "
    "along with this program. If not, see <https://www.gnu.org/licenses/>.";

static const gchar *lgpl_2_1_text =
    "This library is free software; you can redistribute it and/or "
    "modify it under the terms of the GNU Lesser General Public "
    "License as published by the Free Software Foundation; either "
    "version 2.1 of the License, or (at your option) any later version.\n\n"
    "This library is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU "
    "Lesser General Public License for more details.\n\n"
    "You should have received a copy of the GNU Lesser General Public "
    "License along with this library; if not, write to the Free Software "
    "Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.";

static const gchar *lgpl_3_0_text =
    "This library is free software: you can redistribute it and/or modify "
    "it under the terms of the GNU Lesser General Public License as published by "
    "the Free Software Foundation, either version 3 of the License, or "
    "(at your option) any later version.\n\n"
    "This library is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
    "GNU Lesser General Public License for more details.\n\n"
    "You should have received a copy of the GNU Lesser General Public License "
    "along with this library. If not, see <https://www.gnu.org/licenses/>.";

static const gchar *bsd_text =
    "Redistribution and use in source and binary forms, with or without "
    "modification, are permitted provided that the following conditions are met:\n\n"
    "1. Redistributions of source code must retain the above copyright notice, "
    "this list of conditions and the following disclaimer.\n\n"
    "2. Redistributions in binary form must reproduce the above copyright notice, "
    "this list of conditions and the following disclaimer in the documentation "
    "and/or other materials provided with the distribution.\n\n"
    "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" "
    "AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE "
    "IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE "
    "DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE "
    "FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL "
    "DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR "
    "SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER "
    "CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, "
    "OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE "
    "OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.";

static const gchar *mit_text =
    "Permission is hereby granted, free of charge, to any person obtaining a copy "
    "of this software and associated documentation files (the \"Software\"), to deal "
    "in the Software without restriction, including without limitation the rights "
    "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
    "copies of the Software, and to permit persons to whom the Software is "
    "furnished to do so, subject to the following conditions:\n\n"
    "The above copyright notice and this permission notice shall be included in all "
    "copies or substantial portions of the Software.\n\n"
    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
    "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
    "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE "
    "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
    "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
    "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
    "SOFTWARE.";

/**
 * mate_ui_get_license_text:
 * @license_type: The license type
 *
 * Gets the standard license text for a license type.
 *
 * Returns: (transfer none): The license text
 */
const gchar *
mate_ui_get_license_text(MateUiLicenseType license_type)
{
    switch (license_type)
    {
    case MATE_UI_LICENSE_GPL_2_0:
        return gpl_2_0_text;
    case MATE_UI_LICENSE_GPL_3_0:
        return gpl_3_0_text;
    case MATE_UI_LICENSE_LGPL_2_1:
        return lgpl_2_1_text;
    case MATE_UI_LICENSE_LGPL_3_0:
        return lgpl_3_0_text;
    case MATE_UI_LICENSE_BSD:
        return bsd_text;
    case MATE_UI_LICENSE_MIT:
        return mit_text;
    case MATE_UI_LICENSE_CUSTOM:
    default:
        return "";
    }
}

/**
 * mate_ui_dialog_about_new:
 * @parent: (nullable): Parent window
 * @info: About dialog information
 *
 * Creates and shows an About dialog.
 *
 * Returns: (transfer full): The #GtkAboutDialog
 */
GtkWidget *
mate_ui_dialog_about_new(GtkWindow             *parent,
                          const MateUiAboutInfo *info)
{
    g_return_val_if_fail(info != NULL, NULL);
    g_return_val_if_fail(info->program_name != NULL, NULL);

    GtkWidget *dialog = gtk_about_dialog_new();
    GtkAboutDialog *about = GTK_ABOUT_DIALOG(dialog);

    gtk_about_dialog_set_program_name(about, info->program_name);

    if (info->version != NULL)
        gtk_about_dialog_set_version(about, info->version);

    if (info->copyright != NULL)
        gtk_about_dialog_set_copyright(about, info->copyright);

    if (info->comments != NULL)
        gtk_about_dialog_set_comments(about, info->comments);

    if (info->license != NULL)
    {
        gtk_about_dialog_set_license(about, info->license);
        gtk_about_dialog_set_wrap_license(about, TRUE);
    }
    else
    {
        gtk_about_dialog_set_license(about, gpl_2_0_text);
        gtk_about_dialog_set_wrap_license(about, TRUE);
    }

    if (info->website != NULL)
        gtk_about_dialog_set_website(about, info->website);

    if (info->website_label != NULL)
        gtk_about_dialog_set_website_label(about, info->website_label);

    if (info->authors != NULL)
        gtk_about_dialog_set_authors(about, info->authors);

    if (info->documenters != NULL)
        gtk_about_dialog_set_documenters(about, info->documenters);

    if (info->translator_credits != NULL)
        gtk_about_dialog_set_translator_credits(about, info->translator_credits);

    if (info->artists != NULL)
        gtk_about_dialog_set_artists(about, info->artists);

    if (info->logo_icon_name != NULL)
        gtk_about_dialog_set_logo_icon_name(about, info->logo_icon_name);

    if (parent != NULL)
    {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    }

    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    /* Auto-close on response */
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);

    return dialog;
}

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
GtkWidget *
mate_ui_dialog_about_new_simple(GtkWindow   *parent,
                                 const gchar *program_name,
                                 const gchar *version,
                                 const gchar *copyright,
                                 const gchar *comments,
                                 const gchar *logo_icon_name)
{
    MateUiAboutInfo info = {
        .program_name = program_name,
        .version = version,
        .copyright = copyright,
        .comments = comments,
        .logo_icon_name = logo_icon_name,
        .license = NULL,
        .website = NULL,
        .website_label = NULL,
        .authors = NULL,
        .documenters = NULL,
        .translator_credits = NULL,
        .artists = NULL,
    };

    return mate_ui_dialog_about_new(parent, &info);
}

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
gint
mate_ui_dialog_message(GtkWindow      *parent,
                        GtkMessageType  type,
                        GtkButtonsType  buttons,
                        const gchar    *primary,
                        const gchar    *secondary)
{
    g_return_val_if_fail(primary != NULL, GTK_RESPONSE_NONE);

    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                type,
                                                buttons,
                                                "%s", primary);

    if (secondary != NULL)
    {
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
                                                  "%s", secondary);
    }

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return response;
}

/**
 * mate_ui_dialog_error:
 * @parent: (nullable): Parent window
 * @primary: Primary error message
 * @secondary: (nullable): Secondary message with details
 *
 * Shows an error dialog.
 */
void
mate_ui_dialog_error(GtkWindow   *parent,
                      const gchar *primary,
                      const gchar *secondary)
{
    mate_ui_dialog_message(parent, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                            primary, secondary);
}

/**
 * mate_ui_dialog_warning:
 * @parent: (nullable): Parent window
 * @primary: Primary warning message
 * @secondary: (nullable): Secondary message with details
 *
 * Shows a warning dialog.
 */
void
mate_ui_dialog_warning(GtkWindow   *parent,
                        const gchar *primary,
                        const gchar *secondary)
{
    mate_ui_dialog_message(parent, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
                            primary, secondary);
}

/**
 * mate_ui_dialog_info:
 * @parent: (nullable): Parent window
 * @primary: Primary info message
 * @secondary: (nullable): Secondary message with details
 *
 * Shows an information dialog.
 */
void
mate_ui_dialog_info(GtkWindow   *parent,
                     const gchar *primary,
                     const gchar *secondary)
{
    mate_ui_dialog_message(parent, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                            primary, secondary);
}

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
gboolean
mate_ui_dialog_question(GtkWindow   *parent,
                         const gchar *primary,
                         const gchar *secondary)
{
    gint response = mate_ui_dialog_message(parent, GTK_MESSAGE_QUESTION,
                                            GTK_BUTTONS_YES_NO, primary, secondary);
    return (response == GTK_RESPONSE_YES);
}

/**
 * mate_ui_dialog_confirm:
 * @parent: (nullable): Parent window
 * @primary: Primary message
 * @secondary: (nullable): Secondary message
 * @confirm_label: Label for confirm button
 * @is_destructive: Whether the action is destructive
 *
 * Shows a confirmation dialog with custom button label.
 *
 * Returns: %TRUE if user confirmed, %FALSE otherwise
 */
gboolean
mate_ui_dialog_confirm(GtkWindow   *parent,
                        const gchar *primary,
                        const gchar *secondary,
                        const gchar *confirm_label,
                        gboolean     is_destructive)
{
    g_return_val_if_fail(primary != NULL, FALSE);
    g_return_val_if_fail(confirm_label != NULL, FALSE);

    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                is_destructive ? GTK_MESSAGE_WARNING : GTK_MESSAGE_QUESTION,
                                                GTK_BUTTONS_NONE,
                                                "%s", primary);

    if (secondary != NULL)
    {
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
                                                  "%s", secondary);
    }

    gtk_dialog_add_button(GTK_DIALOG(dialog), "_Cancel", GTK_RESPONSE_CANCEL);
    GtkWidget *confirm_btn = gtk_dialog_add_button(GTK_DIALOG(dialog),
                                                    confirm_label, GTK_RESPONSE_ACCEPT);

    if (is_destructive)
    {
        GtkStyleContext *ctx = gtk_widget_get_style_context(confirm_btn);
        gtk_style_context_add_class(ctx, "destructive-action");
    }

    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return (response == GTK_RESPONSE_ACCEPT);
}

/**
 * mate_ui_dialog_file_chooser_open:
 * @parent: (nullable): Parent window
 * @title: Dialog title
 * @filter_name: (nullable): Filter name
 * @filter_pattern: (nullable): Filter pattern
 *
 * Shows a file open dialog.
 *
 * Returns: (transfer full) (nullable): Selected filename or %NULL if cancelled
 */
gchar *
mate_ui_dialog_file_chooser_open(GtkWindow   *parent,
                                  const gchar *title,
                                  const gchar *filter_name,
                                  const gchar *filter_pattern)
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new(title,
                                                     parent,
                                                     GTK_FILE_CHOOSER_ACTION_OPEN,
                                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                                     "_Open", GTK_RESPONSE_ACCEPT,
                                                     NULL);

    if (filter_name != NULL && filter_pattern != NULL)
    {
        GtkFileFilter *filter = gtk_file_filter_new();
        gtk_file_filter_set_name(filter, filter_name);
        gtk_file_filter_add_pattern(filter, filter_pattern);
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

        /* Also add "All files" filter */
        GtkFileFilter *all_filter = gtk_file_filter_new();
        gtk_file_filter_set_name(all_filter, "All files");
        gtk_file_filter_add_pattern(all_filter, "*");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), all_filter);
    }

    gchar *filename = NULL;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    }

    gtk_widget_destroy(dialog);
    return filename;
}

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
gchar *
mate_ui_dialog_file_chooser_save(GtkWindow   *parent,
                                  const gchar *title,
                                  const gchar *default_name,
                                  const gchar *filter_name,
                                  const gchar *filter_pattern)
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new(title,
                                                     parent,
                                                     GTK_FILE_CHOOSER_ACTION_SAVE,
                                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                                     "_Save", GTK_RESPONSE_ACCEPT,
                                                     NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    if (default_name != NULL)
    {
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), default_name);
    }

    if (filter_name != NULL && filter_pattern != NULL)
    {
        GtkFileFilter *filter = gtk_file_filter_new();
        gtk_file_filter_set_name(filter, filter_name);
        gtk_file_filter_add_pattern(filter, filter_pattern);
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

        GtkFileFilter *all_filter = gtk_file_filter_new();
        gtk_file_filter_set_name(all_filter, "All files");
        gtk_file_filter_add_pattern(all_filter, "*");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), all_filter);
    }

    gchar *filename = NULL;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    }

    gtk_widget_destroy(dialog);
    return filename;
}

/**
 * mate_ui_dialog_folder_chooser:
 * @parent: (nullable): Parent window
 * @title: Dialog title
 *
 * Shows a folder selection dialog.
 *
 * Returns: (transfer full) (nullable): Selected folder path or %NULL if cancelled
 */
gchar *
mate_ui_dialog_folder_chooser(GtkWindow   *parent,
                               const gchar *title)
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new(title,
                                                     parent,
                                                     GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                                     "_Select", GTK_RESPONSE_ACCEPT,
                                                     NULL);

    gchar *folder = NULL;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        folder = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    }

    gtk_widget_destroy(dialog);
    return folder;
}

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
void
mate_ui_dialogs_show_about(GtkWindow          *parent,
                            const gchar        *program_name,
                            const gchar        *version,
                            const gchar        *copyright,
                            const gchar        *comments,
                            const gchar        *website,
                            const gchar        *logo_icon_name,
                            const gchar *const *authors,
                            const gchar *const *documenters,
                            const gchar        *translator_credits,
                            GtkLicense          license_type)
{
    GtkWidget *dialog = gtk_about_dialog_new();
    GtkAboutDialog *about = GTK_ABOUT_DIALOG(dialog);

    if (program_name != NULL)
        gtk_about_dialog_set_program_name(about, program_name);

    if (version != NULL)
        gtk_about_dialog_set_version(about, version);

    if (copyright != NULL)
        gtk_about_dialog_set_copyright(about, copyright);

    if (comments != NULL)
        gtk_about_dialog_set_comments(about, comments);

    if (website != NULL)
        gtk_about_dialog_set_website(about, website);

    if (logo_icon_name != NULL)
        gtk_about_dialog_set_logo_icon_name(about, logo_icon_name);

    if (authors != NULL)
        gtk_about_dialog_set_authors(about, (const gchar **)authors);

    if (documenters != NULL)
        gtk_about_dialog_set_documenters(about, (const gchar **)documenters);

    if (translator_credits != NULL)
        gtk_about_dialog_set_translator_credits(about, translator_credits);

    gtk_about_dialog_set_license_type(about, license_type);
    gtk_about_dialog_set_wrap_license(about, TRUE);

    if (parent != NULL)
    {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    }

    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
    gtk_widget_show(dialog);
}
