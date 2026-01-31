/*
 * mate-ui-application.h - Application helpers for MATE
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef __MATE_UI_APPLICATION_H__
#define __MATE_UI_APPLICATION_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MATE_UI_TYPE_APPLICATION (mate_ui_application_get_type())
G_DECLARE_DERIVABLE_TYPE(MateUiApplication, mate_ui_application, MATE_UI, APPLICATION, GtkApplication)

/**
 * MateUiApplicationClass:
 * @parent_class: Parent class
 *
 * Class structure for MateUiApplication
 */
struct _MateUiApplicationClass
{
    GtkApplicationClass parent_class;

    /* Virtual methods */
    void (*show_about)       (MateUiApplication *app);
    void (*show_preferences) (MateUiApplication *app);
    void (*show_help)        (MateUiApplication *app,
                              const gchar       *section);

    /* Padding for future expansion */
    gpointer padding[8];
};

/**
 * mate_ui_application_new:
 * @application_id: The application ID (e.g., "org.mate.Calculator")
 * @flags: Application flags
 *
 * Creates a new MateUiApplication instance.
 *
 * Returns: (transfer full): A new MateUiApplication
 */
MateUiApplication *mate_ui_application_new(const gchar         *application_id,
                                           GApplicationFlags    flags);

/**
 * mate_ui_application_set_app_name:
 * @app: A MateUiApplication
 * @name: Human-readable application name
 *
 * Sets the human-readable application name.
 */
void mate_ui_application_set_app_name(MateUiApplication *app,
                                      const gchar       *name);

/**
 * mate_ui_application_get_app_name:
 * @app: A MateUiApplication
 *
 * Gets the human-readable application name.
 *
 * Returns: (transfer none): The application name
 */
const gchar *mate_ui_application_get_app_name(MateUiApplication *app);

/**
 * mate_ui_application_set_version:
 * @app: A MateUiApplication
 * @version: Version string (e.g., "1.28.0")
 *
 * Sets the application version.
 */
void mate_ui_application_set_version(MateUiApplication *app,
                                     const gchar       *version);

/**
 * mate_ui_application_get_version:
 * @app: A MateUiApplication
 *
 * Gets the application version.
 *
 * Returns: (transfer none): The version string
 */
const gchar *mate_ui_application_get_version(MateUiApplication *app);

/**
 * mate_ui_application_set_comments:
 * @app: A MateUiApplication
 * @comments: Brief description of the application
 *
 * Sets a brief description of the application.
 */
void mate_ui_application_set_comments(MateUiApplication *app,
                                      const gchar       *comments);

/**
 * mate_ui_application_get_comments:
 * @app: A MateUiApplication
 *
 * Gets the application description.
 *
 * Returns: (transfer none): The description
 */
const gchar *mate_ui_application_get_comments(MateUiApplication *app);

/**
 * mate_ui_application_set_copyright:
 * @app: A MateUiApplication
 * @copyright: Copyright notice
 *
 * Sets the copyright notice.
 */
void mate_ui_application_set_copyright(MateUiApplication *app,
                                       const gchar       *copyright);

/**
 * mate_ui_application_get_copyright:
 * @app: A MateUiApplication
 *
 * Gets the copyright notice.
 *
 * Returns: (transfer none): The copyright notice
 */
const gchar *mate_ui_application_get_copyright(MateUiApplication *app);

/**
 * mate_ui_application_set_website:
 * @app: A MateUiApplication
 * @website: Website URL
 *
 * Sets the application website.
 */
void mate_ui_application_set_website(MateUiApplication *app,
                                     const gchar       *website);

/**
 * mate_ui_application_get_website:
 * @app: A MateUiApplication
 *
 * Gets the application website.
 *
 * Returns: (transfer none): The website URL
 */
const gchar *mate_ui_application_get_website(MateUiApplication *app);

/**
 * mate_ui_application_set_help_uri:
 * @app: A MateUiApplication
 * @help_uri: Help URI (e.g., "help:mate-calculator")
 *
 * Sets the help URI for the application.
 */
void mate_ui_application_set_help_uri(MateUiApplication *app,
                                      const gchar       *help_uri);

/**
 * mate_ui_application_get_help_uri:
 * @app: A MateUiApplication
 *
 * Gets the help URI.
 *
 * Returns: (transfer none): The help URI
 */
const gchar *mate_ui_application_get_help_uri(MateUiApplication *app);

/**
 * mate_ui_application_set_icon_name:
 * @app: A MateUiApplication
 * @icon_name: Icon name from the icon theme
 *
 * Sets the application icon name.
 */
void mate_ui_application_set_icon_name(MateUiApplication *app,
                                       const gchar       *icon_name);

/**
 * mate_ui_application_get_icon_name:
 * @app: A MateUiApplication
 *
 * Gets the application icon name.
 *
 * Returns: (transfer none): The icon name
 */
const gchar *mate_ui_application_get_icon_name(MateUiApplication *app);

/**
 * mate_ui_application_set_authors:
 * @app: A MateUiApplication
 * @authors: (array zero-terminated=1): Array of author names
 *
 * Sets the list of authors.
 */
void mate_ui_application_set_authors(MateUiApplication  *app,
                                     const gchar *const *authors);

/**
 * mate_ui_application_get_authors:
 * @app: A MateUiApplication
 *
 * Gets the list of authors.
 *
 * Returns: (transfer none) (array zero-terminated=1): The authors array
 */
const gchar *const *mate_ui_application_get_authors(MateUiApplication *app);

/**
 * mate_ui_application_set_documenters:
 * @app: A MateUiApplication
 * @documenters: (array zero-terminated=1): Array of documenter names
 *
 * Sets the list of documenters.
 */
void mate_ui_application_set_documenters(MateUiApplication  *app,
                                         const gchar *const *documenters);

/**
 * mate_ui_application_get_documenters:
 * @app: A MateUiApplication
 *
 * Gets the list of documenters.
 *
 * Returns: (transfer none) (array zero-terminated=1): The documenters array
 */
const gchar *const *mate_ui_application_get_documenters(MateUiApplication *app);

/**
 * mate_ui_application_set_license_type:
 * @app: A MateUiApplication
 * @license: A GtkLicense value
 *
 * Sets the license type.
 */
void mate_ui_application_set_license_type(MateUiApplication *app,
                                          GtkLicense         license);

/**
 * mate_ui_application_get_license_type:
 * @app: A MateUiApplication
 *
 * Gets the license type.
 *
 * Returns: The license type
 */
GtkLicense mate_ui_application_get_license_type(MateUiApplication *app);

/**
 * mate_ui_application_show_about:
 * @app: A MateUiApplication
 *
 * Shows the About dialog using the application's metadata.
 */
void mate_ui_application_show_about(MateUiApplication *app);

/**
 * mate_ui_application_show_help:
 * @app: A MateUiApplication
 * @section: (nullable): Help section to open, or NULL for main help
 *
 * Opens the help viewer for this application.
 */
void mate_ui_application_show_help(MateUiApplication *app,
                                   const gchar       *section);

/**
 * mate_ui_application_setup_common_actions:
 * @app: A MateUiApplication
 *
 * Sets up common application actions (about, help, quit, preferences).
 * These actions will be available as "app.about", "app.help", "app.quit",
 * and "app.preferences".
 */
void mate_ui_application_setup_common_actions(MateUiApplication *app);

G_END_DECLS

#endif /* __MATE_UI_APPLICATION_H__ */
