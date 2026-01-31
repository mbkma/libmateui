/*
 * mate-ui-application.c - Application helpers for MATE
 *
 * Copyright (C) 2024 MATE Desktop Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "config.h"
#include "mate-ui-application.h"
#include "mate-ui-dialogs.h"

typedef struct
{
    gchar      *app_name;
    gchar      *version;
    gchar      *comments;
    gchar      *copyright;
    gchar      *website;
    gchar      *website_label;
    gchar      *help_uri;
    gchar      *icon_name;
    gchar     **authors;
    gchar     **documenters;
    gchar     **artists;
    gchar      *translator_credits;
    GtkLicense  license_type;
} MateUiApplicationPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(MateUiApplication, mate_ui_application, GTK_TYPE_APPLICATION)

enum
{
    PROP_0,
    PROP_APP_NAME,
    PROP_VERSION,
    PROP_COMMENTS,
    PROP_COPYRIGHT,
    PROP_WEBSITE,
    PROP_HELP_URI,
    PROP_ICON_NAME,
    PROP_LICENSE_TYPE,
    N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES] = { NULL, };

static void
mate_ui_application_finalize(GObject *object)
{
    MateUiApplication *app = MATE_UI_APPLICATION(object);
    MateUiApplicationPrivate *priv = mate_ui_application_get_instance_private(app);

    g_free(priv->app_name);
    g_free(priv->version);
    g_free(priv->comments);
    g_free(priv->copyright);
    g_free(priv->website);
    g_free(priv->website_label);
    g_free(priv->help_uri);
    g_free(priv->icon_name);
    g_strfreev(priv->authors);
    g_strfreev(priv->documenters);
    g_strfreev(priv->artists);
    g_free(priv->translator_credits);

    G_OBJECT_CLASS(mate_ui_application_parent_class)->finalize(object);
}

static void
mate_ui_application_set_property(GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
    MateUiApplication *app = MATE_UI_APPLICATION(object);

    switch (prop_id)
    {
        case PROP_APP_NAME:
            mate_ui_application_set_app_name(app, g_value_get_string(value));
            break;
        case PROP_VERSION:
            mate_ui_application_set_version(app, g_value_get_string(value));
            break;
        case PROP_COMMENTS:
            mate_ui_application_set_comments(app, g_value_get_string(value));
            break;
        case PROP_COPYRIGHT:
            mate_ui_application_set_copyright(app, g_value_get_string(value));
            break;
        case PROP_WEBSITE:
            mate_ui_application_set_website(app, g_value_get_string(value));
            break;
        case PROP_HELP_URI:
            mate_ui_application_set_help_uri(app, g_value_get_string(value));
            break;
        case PROP_ICON_NAME:
            mate_ui_application_set_icon_name(app, g_value_get_string(value));
            break;
        case PROP_LICENSE_TYPE:
            mate_ui_application_set_license_type(app, g_value_get_enum(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void
mate_ui_application_get_property(GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
    MateUiApplication *app = MATE_UI_APPLICATION(object);
    MateUiApplicationPrivate *priv = mate_ui_application_get_instance_private(app);

    switch (prop_id)
    {
        case PROP_APP_NAME:
            g_value_set_string(value, priv->app_name);
            break;
        case PROP_VERSION:
            g_value_set_string(value, priv->version);
            break;
        case PROP_COMMENTS:
            g_value_set_string(value, priv->comments);
            break;
        case PROP_COPYRIGHT:
            g_value_set_string(value, priv->copyright);
            break;
        case PROP_WEBSITE:
            g_value_set_string(value, priv->website);
            break;
        case PROP_HELP_URI:
            g_value_set_string(value, priv->help_uri);
            break;
        case PROP_ICON_NAME:
            g_value_set_string(value, priv->icon_name);
            break;
        case PROP_LICENSE_TYPE:
            g_value_set_enum(value, priv->license_type);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

/* Action callbacks */
static void
action_about_cb(GSimpleAction *action G_GNUC_UNUSED,
                GVariant      *parameter G_GNUC_UNUSED,
                gpointer       user_data)
{
    MateUiApplication *app = MATE_UI_APPLICATION(user_data);
    mate_ui_application_show_about(app);
}

static void
action_help_cb(GSimpleAction *action G_GNUC_UNUSED,
               GVariant      *parameter G_GNUC_UNUSED,
               gpointer       user_data)
{
    MateUiApplication *app = MATE_UI_APPLICATION(user_data);
    mate_ui_application_show_help(app, NULL);
}

static void
action_quit_cb(GSimpleAction *action G_GNUC_UNUSED,
               GVariant      *parameter G_GNUC_UNUSED,
               gpointer       user_data)
{
    GApplication *app = G_APPLICATION(user_data);
    g_application_quit(app);
}

static void
action_preferences_cb(GSimpleAction *action G_GNUC_UNUSED,
                      GVariant      *parameter G_GNUC_UNUSED,
                      gpointer       user_data G_GNUC_UNUSED)
{
    /* Default implementation does nothing - subclasses should override */
    g_debug("Preferences action triggered but no handler implemented");
}

static void
mate_ui_application_startup(GApplication *application)
{
    MateUiApplication *app = MATE_UI_APPLICATION(application);
    MateUiApplicationPrivate *priv = mate_ui_application_get_instance_private(app);

    G_APPLICATION_CLASS(mate_ui_application_parent_class)->startup(application);

    /* Set window icon if specified */
    if (priv->icon_name != NULL)
    {
        gtk_window_set_default_icon_name(priv->icon_name);
    }
}

static void
mate_ui_application_class_init(MateUiApplicationClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GApplicationClass *app_class = G_APPLICATION_CLASS(klass);

    object_class->finalize = mate_ui_application_finalize;
    object_class->set_property = mate_ui_application_set_property;
    object_class->get_property = mate_ui_application_get_property;

    app_class->startup = mate_ui_application_startup;

    properties[PROP_APP_NAME] =
        g_param_spec_string("app-name",
                            "Application Name",
                            "Human-readable application name",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    properties[PROP_VERSION] =
        g_param_spec_string("version",
                            "Version",
                            "Application version string",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    properties[PROP_COMMENTS] =
        g_param_spec_string("comments",
                            "Comments",
                            "Brief application description",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    properties[PROP_COPYRIGHT] =
        g_param_spec_string("copyright",
                            "Copyright",
                            "Copyright notice",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    properties[PROP_WEBSITE] =
        g_param_spec_string("website",
                            "Website",
                            "Application website URL",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    properties[PROP_HELP_URI] =
        g_param_spec_string("help-uri",
                            "Help URI",
                            "URI for application help",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    properties[PROP_ICON_NAME] =
        g_param_spec_string("icon-name",
                            "Icon Name",
                            "Application icon name from theme",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    properties[PROP_LICENSE_TYPE] =
        g_param_spec_enum("license-type",
                          "License Type",
                          "Application license type",
                          GTK_TYPE_LICENSE,
                          GTK_LICENSE_UNKNOWN,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    g_object_class_install_properties(object_class, N_PROPERTIES, properties);
}

static void
mate_ui_application_init(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv = mate_ui_application_get_instance_private(app);

    priv->app_name = NULL;
    priv->version = NULL;
    priv->comments = NULL;
    priv->copyright = NULL;
    priv->website = NULL;
    priv->website_label = NULL;
    priv->help_uri = NULL;
    priv->icon_name = NULL;
    priv->authors = NULL;
    priv->documenters = NULL;
    priv->artists = NULL;
    priv->translator_credits = NULL;
    priv->license_type = GTK_LICENSE_UNKNOWN;
}

MateUiApplication *
mate_ui_application_new(const gchar       *application_id,
                        GApplicationFlags  flags)
{
    g_return_val_if_fail(g_application_id_is_valid(application_id), NULL);

    return g_object_new(MATE_UI_TYPE_APPLICATION,
                        "application-id", application_id,
                        "flags", flags,
                        NULL);
}

void
mate_ui_application_set_app_name(MateUiApplication *app,
                                 const gchar       *name)
{
    MateUiApplicationPrivate *priv;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);

    g_free(priv->app_name);
    priv->app_name = g_strdup(name);

    g_object_notify_by_pspec(G_OBJECT(app), properties[PROP_APP_NAME]);
}

const gchar *
mate_ui_application_get_app_name(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv;

    g_return_val_if_fail(MATE_UI_IS_APPLICATION(app), NULL);

    priv = mate_ui_application_get_instance_private(app);
    return priv->app_name;
}

void
mate_ui_application_set_version(MateUiApplication *app,
                                const gchar       *version)
{
    MateUiApplicationPrivate *priv;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);

    g_free(priv->version);
    priv->version = g_strdup(version);

    g_object_notify_by_pspec(G_OBJECT(app), properties[PROP_VERSION]);
}

const gchar *
mate_ui_application_get_version(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv;

    g_return_val_if_fail(MATE_UI_IS_APPLICATION(app), NULL);

    priv = mate_ui_application_get_instance_private(app);
    return priv->version;
}

void
mate_ui_application_set_comments(MateUiApplication *app,
                                 const gchar       *comments)
{
    MateUiApplicationPrivate *priv;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);

    g_free(priv->comments);
    priv->comments = g_strdup(comments);

    g_object_notify_by_pspec(G_OBJECT(app), properties[PROP_COMMENTS]);
}

const gchar *
mate_ui_application_get_comments(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv;

    g_return_val_if_fail(MATE_UI_IS_APPLICATION(app), NULL);

    priv = mate_ui_application_get_instance_private(app);
    return priv->comments;
}

void
mate_ui_application_set_copyright(MateUiApplication *app,
                                  const gchar       *copyright)
{
    MateUiApplicationPrivate *priv;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);

    g_free(priv->copyright);
    priv->copyright = g_strdup(copyright);

    g_object_notify_by_pspec(G_OBJECT(app), properties[PROP_COPYRIGHT]);
}

const gchar *
mate_ui_application_get_copyright(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv;

    g_return_val_if_fail(MATE_UI_IS_APPLICATION(app), NULL);

    priv = mate_ui_application_get_instance_private(app);
    return priv->copyright;
}

void
mate_ui_application_set_website(MateUiApplication *app,
                                const gchar       *website)
{
    MateUiApplicationPrivate *priv;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);

    g_free(priv->website);
    priv->website = g_strdup(website);

    g_object_notify_by_pspec(G_OBJECT(app), properties[PROP_WEBSITE]);
}

const gchar *
mate_ui_application_get_website(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv;

    g_return_val_if_fail(MATE_UI_IS_APPLICATION(app), NULL);

    priv = mate_ui_application_get_instance_private(app);
    return priv->website;
}

void
mate_ui_application_set_help_uri(MateUiApplication *app,
                                 const gchar       *help_uri)
{
    MateUiApplicationPrivate *priv;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);

    g_free(priv->help_uri);
    priv->help_uri = g_strdup(help_uri);

    g_object_notify_by_pspec(G_OBJECT(app), properties[PROP_HELP_URI]);
}

const gchar *
mate_ui_application_get_help_uri(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv;

    g_return_val_if_fail(MATE_UI_IS_APPLICATION(app), NULL);

    priv = mate_ui_application_get_instance_private(app);
    return priv->help_uri;
}

void
mate_ui_application_set_icon_name(MateUiApplication *app,
                                  const gchar       *icon_name)
{
    MateUiApplicationPrivate *priv;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);

    g_free(priv->icon_name);
    priv->icon_name = g_strdup(icon_name);

    g_object_notify_by_pspec(G_OBJECT(app), properties[PROP_ICON_NAME]);
}

const gchar *
mate_ui_application_get_icon_name(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv;

    g_return_val_if_fail(MATE_UI_IS_APPLICATION(app), NULL);

    priv = mate_ui_application_get_instance_private(app);
    return priv->icon_name;
}

void
mate_ui_application_set_authors(MateUiApplication  *app,
                                const gchar *const *authors)
{
    MateUiApplicationPrivate *priv;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);

    g_strfreev(priv->authors);
    priv->authors = g_strdupv((gchar **)authors);
}

const gchar *const *
mate_ui_application_get_authors(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv;

    g_return_val_if_fail(MATE_UI_IS_APPLICATION(app), NULL);

    priv = mate_ui_application_get_instance_private(app);
    return (const gchar *const *)priv->authors;
}

void
mate_ui_application_set_documenters(MateUiApplication  *app,
                                    const gchar *const *documenters)
{
    MateUiApplicationPrivate *priv;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);

    g_strfreev(priv->documenters);
    priv->documenters = g_strdupv((gchar **)documenters);
}

const gchar *const *
mate_ui_application_get_documenters(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv;

    g_return_val_if_fail(MATE_UI_IS_APPLICATION(app), NULL);

    priv = mate_ui_application_get_instance_private(app);
    return (const gchar *const *)priv->documenters;
}

void
mate_ui_application_set_license_type(MateUiApplication *app,
                                     GtkLicense         license)
{
    MateUiApplicationPrivate *priv;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);

    priv->license_type = license;

    g_object_notify_by_pspec(G_OBJECT(app), properties[PROP_LICENSE_TYPE]);
}

GtkLicense
mate_ui_application_get_license_type(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv;

    g_return_val_if_fail(MATE_UI_IS_APPLICATION(app), GTK_LICENSE_UNKNOWN);

    priv = mate_ui_application_get_instance_private(app);
    return priv->license_type;
}

void
mate_ui_application_show_about(MateUiApplication *app)
{
    MateUiApplicationPrivate *priv;
    GtkWindow *parent;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);
    parent = gtk_application_get_active_window(GTK_APPLICATION(app));

    mate_ui_dialogs_show_about(parent,
                               priv->app_name,
                               priv->version,
                               priv->copyright,
                               priv->comments,
                               priv->website,
                               priv->icon_name,
                               (const gchar *const *)priv->authors,
                               (const gchar *const *)priv->documenters,
                               priv->translator_credits,
                               priv->license_type);
}

void
mate_ui_application_show_help(MateUiApplication *app,
                              const gchar       *section)
{
    MateUiApplicationPrivate *priv;
    GtkWindow *parent;
    gchar *uri;
    GError *error = NULL;

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    priv = mate_ui_application_get_instance_private(app);

    if (priv->help_uri == NULL)
    {
        g_warning("No help URI set for application");
        return;
    }

    parent = gtk_application_get_active_window(GTK_APPLICATION(app));

    if (section != NULL)
    {
        uri = g_strdup_printf("%s#%s", priv->help_uri, section);
    }
    else
    {
        uri = g_strdup(priv->help_uri);
    }

    if (!gtk_show_uri_on_window(parent, uri, GDK_CURRENT_TIME, &error))
    {
        g_warning("Failed to open help: %s", error->message);
        g_error_free(error);
    }

    g_free(uri);
}

void
mate_ui_application_setup_common_actions(MateUiApplication *app)
{
    static const GActionEntry app_actions[] = {
        { "about", action_about_cb, NULL, NULL, NULL, { 0 } },
        { "help", action_help_cb, NULL, NULL, NULL, { 0 } },
        { "preferences", action_preferences_cb, NULL, NULL, NULL, { 0 } },
        { "quit", action_quit_cb, NULL, NULL, NULL, { 0 } },
    };

    g_return_if_fail(MATE_UI_IS_APPLICATION(app));

    g_action_map_add_action_entries(G_ACTION_MAP(app),
                                    app_actions,
                                    G_N_ELEMENTS(app_actions),
                                    app);

    /* Set up common accelerators */
    const gchar *quit_accels[] = { "<Primary>q", NULL };
    const gchar *help_accels[] = { "F1", NULL };

    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.quit", quit_accels);
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.help", help_accels);
}
