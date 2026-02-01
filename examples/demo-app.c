/*
 * demo-app.c - Demo application for libmateui
 *
 * This demonstrates all the major features of libmateui:
 * - MateUiApplication for application setup
 * - MateUiWindow for window management
 * - MateUiMenu for menubar construction
 * - MateUiDialogs for standard dialogs
 * - MateUiAccel for accelerator management
 * - MateUiSession for session management
 * - MateUiSettings for preferences binding
 * - MateUiUtil for utility functions
 *
 * Build with: meson build && ninja -C build
 * Run with: ./build/examples/mate-ui-demo
 */

/* For building within the project, include directly */
#include "mate-ui.h"

/* Application metadata */
static const gchar *authors[] = {
    "MATE Desktop Team",
    "Demo Author",
    NULL
};

/* Menu entries for File menu */
static const MateUiMenuEntry file_menu_entries[] = {
    { "_New",        "app.new",    "<Control>n", "document-new" },
    { "_Open...",    "app.open",   "<Control>o", "document-open" },
    { "_Save",       "win.save",   "<Control>s", "document-save" },
    { "Save _As...", "win.save-as", "<Control><Shift>s", "document-save-as" },
    MATE_UI_MENU_SEPARATOR,
    { "_Quit",       "app.quit",   "<Control>q", "application-exit" },
};

/* Menu entries for Edit menu */
static const MateUiMenuEntry edit_menu_entries[] = {
    { "_Undo",       "win.undo",   "<Control>z", "edit-undo" },
    { "_Redo",       "win.redo",   "<Control><Shift>z", "edit-redo" },
    MATE_UI_MENU_SEPARATOR,
    { "Cu_t",        "win.cut",    "<Control>x", "edit-cut" },
    { "_Copy",       "win.copy",   "<Control>c", "edit-copy" },
    { "_Paste",      "win.paste",  "<Control>v", "edit-paste" },
    MATE_UI_MENU_SEPARATOR,
    { "_Preferences", "app.preferences", "<Control>comma", "preferences-system" },
};

/* Menu entries for Help menu */
static const MateUiMenuEntry help_menu_entries[] = {
    { "_Contents",   "app.help",   "F1", "help-contents" },
    MATE_UI_MENU_SEPARATOR,
    { "_About",      "app.about",  NULL, "help-about" },
};

/* All submenus */
static const MateUiSubmenu submenus[] = {
    { "_File", file_menu_entries, G_N_ELEMENTS(file_menu_entries) },
    { "_Edit", edit_menu_entries, G_N_ELEMENTS(edit_menu_entries) },
    { "_Help", help_menu_entries, G_N_ELEMENTS(help_menu_entries) },
};

/* Accelerator entries */
static const MateUiAccelEntry accel_entries[] = {
    { "app.new",     "<Control>n" },
    { "app.open",    "<Control>o" },
    { "app.quit",    "<Control>q" },
    { "app.help",    "F1" },
    { "win.save",    "<Control>s" },
    { "win.save-as", "<Control><Shift>s" },
    { "win.undo",    "<Control>z" },
    { "win.redo",    "<Control><Shift>z" },
    { "win.cut",     "<Control>x" },
    { "win.copy",    "<Control>c" },
    { "win.paste",   "<Control>v" },
};

/* Forward declarations */
static void startup_cb(GApplication *app, gpointer user_data);
static void activate_cb(GApplication *app, gpointer user_data);
static void new_action_cb(GSimpleAction *action, GVariant *param, gpointer user_data);
static void open_action_cb(GSimpleAction *action, GVariant *param, gpointer user_data);
static void save_action_cb(GSimpleAction *action, GVariant *param, gpointer user_data);
static void preferences_action_cb(GSimpleAction *action, GVariant *param, gpointer user_data);
static void on_error_btn_clicked(GtkButton *btn, gpointer user_data);
static void on_warning_btn_clicked(GtkButton *btn, gpointer user_data);
static void on_question_btn_clicked(GtkButton *btn, gpointer user_data);
static void on_confirm_btn_clicked(GtkButton *btn, gpointer user_data);
static void on_inhibit_btn_clicked(GtkButton *btn, gpointer user_data);

/* Application actions */
static const GActionEntry app_actions[] = {
    { "new",         new_action_cb,         NULL, NULL, NULL, { 0 } },
    { "open",        open_action_cb,        NULL, NULL, NULL, { 0 } },
    { "preferences", preferences_action_cb, NULL, NULL, NULL, { 0 } },
};

/* Window actions */
static const GActionEntry win_actions[] = {
    { "save",    save_action_cb, NULL, NULL, NULL, { 0 } },
    { "save-as", save_action_cb, NULL, NULL, NULL, { 0 } },
    { "undo",    NULL, NULL, NULL, NULL, { 0 } },
    { "redo",    NULL, NULL, NULL, NULL, { 0 } },
    { "cut",     NULL, NULL, NULL, NULL, { 0 } },
    { "copy",    NULL, NULL, NULL, NULL, { 0 } },
    { "paste",   NULL, NULL, NULL, NULL, { 0 } },
};

/* Session inhibitor (global for demo) */
static MateUiSessionInhibitor *inhibitor = NULL;

/* Dialog button callbacks */
static void
on_error_btn_clicked(GtkButton *btn G_GNUC_UNUSED,
                      gpointer   user_data)
{
    GtkWindow *window = GTK_WINDOW(user_data);
    mate_ui_dialog_error(window, "Error Occurred",
                          "This is a demo error message.");
}

static void
on_warning_btn_clicked(GtkButton *btn G_GNUC_UNUSED,
                        gpointer   user_data)
{
    GtkWindow *window = GTK_WINDOW(user_data);
    mate_ui_dialog_warning(window, "Warning",
                            "This is a demo warning message.");
}

static void
on_question_btn_clicked(GtkButton *btn G_GNUC_UNUSED,
                         gpointer   user_data)
{
    GtkWindow *window = GTK_WINDOW(user_data);
    gboolean result = mate_ui_dialog_question(window,
                                               "Confirm Action",
                                               "Do you want to proceed?");
    mate_ui_dialog_info(window, "Result",
                         result ? "You clicked Yes" : "You clicked No");
}

static void
on_confirm_btn_clicked(GtkButton *btn G_GNUC_UNUSED,
                        gpointer   user_data)
{
    GtkWindow *window = GTK_WINDOW(user_data);
    gboolean result = mate_ui_dialog_confirm(window,
                                              "Delete File?",
                                              "This action cannot be undone.",
                                              "_Delete",
                                              TRUE);
    if (result)
        mate_ui_dialog_info(window, "Action", "File would be deleted!");
}

static void
on_inhibit_btn_clicked(GtkButton *btn,
                        gpointer   user_data)
{
    MateUiApplication *app = MATE_UI_APPLICATION(user_data);
    if (inhibitor == NULL)
    {
        inhibitor = mate_ui_session_inhibit(GTK_APPLICATION(app),
                                             NULL,
                                             MATE_UI_INHIBIT_SUSPEND | MATE_UI_INHIBIT_IDLE,
                                             "Demo application is running");
        if (inhibitor != NULL)
            gtk_button_set_label(btn, "Release Session Inhibit");
    }
    else
    {
        mate_ui_session_uninhibit(inhibitor);
        inhibitor = NULL;
        gtk_button_set_label(btn, "Toggle Session Inhibit");
    }
}

/* New action callback */
static void
new_action_cb(GSimpleAction *action G_GNUC_UNUSED,
              GVariant      *param G_GNUC_UNUSED,
              gpointer       user_data)
{
    MateUiApplication *app = MATE_UI_APPLICATION(user_data);
    GtkWidget *window = mate_ui_window_new(GTK_APPLICATION(app),
                                            "New Document - Demo App",
                                            MATE_UI_WINDOW_SHOW_MENUBAR);

    /* Create content */
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *textview = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled), textview);
    mate_ui_window_set_content(MATE_UI_WINDOW(window), scrolled);

    gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
    gtk_widget_show_all(window);
}

/* Open action callback */
static void
open_action_cb(GSimpleAction *action G_GNUC_UNUSED,
               GVariant      *param G_GNUC_UNUSED,
               gpointer       user_data)
{
    GtkApplication *app = GTK_APPLICATION(user_data);
    GtkWindow *window = gtk_application_get_active_window(app);

    gchar *filename = mate_ui_dialog_file_chooser_open(window,
                                                        "Open File",
                                                        "Text files",
                                                        "*.txt");
    if (filename != NULL)
    {
        mate_ui_dialog_info(window,
                            "File Selected",
                            filename);
        g_free(filename);
    }
}

/* Save action callback */
static void
save_action_cb(GSimpleAction *action G_GNUC_UNUSED,
               GVariant      *param G_GNUC_UNUSED,
               gpointer       user_data)
{
    GtkWindow *window = GTK_WINDOW(user_data);

    gchar *filename = mate_ui_dialog_file_chooser_save(window,
                                                        "Save File",
                                                        "untitled.txt",
                                                        "Text files",
                                                        "*.txt");
    if (filename != NULL)
    {
        mate_ui_dialog_info(window,
                            "File Saved",
                            filename);
        g_free(filename);
    }
}

/* Preferences action callback */
static void
preferences_action_cb(GSimpleAction *action G_GNUC_UNUSED,
                       GVariant      *param G_GNUC_UNUSED,
                       gpointer       user_data)
{
    GtkApplication *app = GTK_APPLICATION(user_data);
    GtkWindow *parent = gtk_application_get_active_window(app);

    /* Create a simple preferences dialog */
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Preferences",
                                                     parent,
                                                     GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                     "_Close", GTK_RESPONSE_CLOSE,
                                                     NULL);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    mate_ui_util_widget_set_margin(content, 18);
    gtk_box_set_spacing(GTK_BOX(content), 12);

    /* Add some example widgets */
    GtkWidget *label = gtk_label_new("Demo Preferences");
    gtk_box_pack_start(GTK_BOX(content), label, FALSE, FALSE, 0);

    GtkWidget *check = gtk_check_button_new_with_label("Enable feature");
    gtk_box_pack_start(GTK_BOX(content), check, FALSE, FALSE, 0);

    GtkWidget *spin_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *spin_label = gtk_label_new("Value:");
    GtkWidget *spin = gtk_spin_button_new_with_range(0, 100, 1);
    gtk_box_pack_start(GTK_BOX(spin_box), spin_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(spin_box), spin, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content), spin_box, FALSE, FALSE, 0);

    gtk_widget_show_all(content);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* Startup callback - called after GTK is initialized */
static void
startup_cb(GApplication *gapp,
           gpointer      user_data G_GNUC_UNUSED)
{
    MateUiApplication *app = MATE_UI_APPLICATION(gapp);

    /* Setup common actions (about, help, quit) - must be done after GTK init */
    mate_ui_application_setup_common_actions(app);

    /* Add additional application actions */
    g_action_map_add_action_entries(G_ACTION_MAP(app),
                                     app_actions,
                                     G_N_ELEMENTS(app_actions),
                                     app);

    /* Set up accelerators - must be done after GTK init */
    mate_ui_accel_set_app_accels(GTK_APPLICATION(app),
                                  accel_entries,
                                  G_N_ELEMENTS(accel_entries));
}

/* Create the main window */
static void
activate_cb(GApplication *gapp,
            gpointer      user_data G_GNUC_UNUSED)
{
    MateUiApplication *app = MATE_UI_APPLICATION(gapp);

    /* Create window with menubar */
    GtkWidget *window = mate_ui_window_new(GTK_APPLICATION(app),
                                            "Demo Application - libmateui",
                                            MATE_UI_WINDOW_SHOW_MENUBAR);

    /* Add window actions */
    g_action_map_add_action_entries(G_ACTION_MAP(window),
                                     win_actions,
                                     G_N_ELEMENTS(win_actions),
                                     window);

    /* Create accelerator group and menubar */
    GtkAccelGroup *accel_group = mate_ui_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    GtkWidget *menubar = mate_ui_menu_bar_new_from_entries(submenus,
                                                            G_N_ELEMENTS(submenus),
                                                            accel_group);
    mate_ui_window_set_menubar(MATE_UI_WINDOW(window), menubar);

    /* Create toolbar */
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

    GtkToolItem *new_btn = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(new_btn), "document-new");
    gtk_actionable_set_action_name(GTK_ACTIONABLE(new_btn), "app.new");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new_btn, -1);

    GtkToolItem *open_btn = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(open_btn), "document-open");
    gtk_actionable_set_action_name(GTK_ACTIONABLE(open_btn), "app.open");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open_btn, -1);

    GtkToolItem *save_btn = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(save_btn), "document-save");
    gtk_actionable_set_action_name(GTK_ACTIONABLE(save_btn), "win.save");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save_btn, -1);

    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_separator_tool_item_new(), -1);

    GtkToolItem *about_btn = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(about_btn), "help-about");
    gtk_actionable_set_action_name(GTK_ACTIONABLE(about_btn), "app.about");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), about_btn, -1);

    mate_ui_window_set_toolbar(MATE_UI_WINDOW(window), toolbar);

    /* Create main content area */
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    mate_ui_util_widget_set_margin(box, 12);

    /* Add a header */
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header),
                          "<big><b>Welcome to libmateui Demo</b></big>");
    gtk_box_pack_start(GTK_BOX(box), header, FALSE, FALSE, 0);

    /* Add feature buttons */
    GtkWidget *frame = gtk_frame_new("Features Demo");
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    mate_ui_util_widget_set_margin(grid, 12);
    gtk_container_add(GTK_CONTAINER(frame), grid);

    /* Error dialog button */
    GtkWidget *error_btn = gtk_button_new_with_label("Show Error Dialog");
    g_signal_connect(error_btn, "clicked", G_CALLBACK(on_error_btn_clicked), window);
    gtk_grid_attach(GTK_GRID(grid), error_btn, 0, 0, 1, 1);

    /* Warning dialog button */
    GtkWidget *warn_btn = gtk_button_new_with_label("Show Warning Dialog");
    g_signal_connect(warn_btn, "clicked", G_CALLBACK(on_warning_btn_clicked), window);
    gtk_grid_attach(GTK_GRID(grid), warn_btn, 1, 0, 1, 1);

    /* Question dialog button */
    GtkWidget *question_btn = gtk_button_new_with_label("Show Question Dialog");
    g_signal_connect(question_btn, "clicked", G_CALLBACK(on_question_btn_clicked), window);
    gtk_grid_attach(GTK_GRID(grid), question_btn, 0, 1, 1, 1);

    /* Confirm dialog button */
    GtkWidget *confirm_btn = gtk_button_new_with_label("Show Confirm (Destructive)");
    g_signal_connect(confirm_btn, "clicked", G_CALLBACK(on_confirm_btn_clicked), window);
    gtk_grid_attach(GTK_GRID(grid), confirm_btn, 1, 1, 1, 1);

    /* Session inhibit button */
    GtkWidget *inhibit_btn = gtk_button_new_with_label("Toggle Session Inhibit");
    g_signal_connect(inhibit_btn, "clicked", G_CALLBACK(on_inhibit_btn_clicked), gapp);
    gtk_grid_attach(GTK_GRID(grid), inhibit_btn, 0, 2, 2, 1);

    gtk_box_pack_start(GTK_BOX(box), frame, FALSE, FALSE, 0);

    /* Add a text view */
    GtkWidget *text_frame = gtk_frame_new("Editor Area");
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 150);
    GtkWidget *textview = gtk_text_view_new();
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gtk_text_buffer_set_text(buffer,
                              "Welcome to the libmateui demo application!\n\n"
                              "This demonstrates the various components of libmateui:\n\n"
                              "• MateUiApplication - Application setup and metadata\n"
                              "• MateUiWindow - Window management with menubar/toolbar\n"
                              "• MateUiMenu - Menu construction from data structures\n"
                              "• MateUiDialogs - Standard dialogs (About, Error, etc.)\n"
                              "• MateUiAccel - Keyboard accelerator management\n"
                              "• MateUiSettings - GSettings binding helpers\n"
                              "• MateUiSession - Session management and inhibit\n"
                              "• MateUiUtil - Common utility functions\n",
                              -1);
    gtk_container_add(GTK_CONTAINER(scrolled), textview);
    gtk_container_add(GTK_CONTAINER(text_frame), scrolled);
    gtk_box_pack_start(GTK_BOX(box), text_frame, TRUE, TRUE, 0);

    mate_ui_window_set_content(MATE_UI_WINDOW(window), box);

    /* Add statusbar */
    GtkWidget *statusbar = gtk_statusbar_new();
    guint context = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "main");
    gtk_statusbar_push(GTK_STATUSBAR(statusbar), context, "Ready");
    mate_ui_window_set_statusbar(MATE_UI_WINDOW(window), statusbar);

    /* Set default size and show */
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_widget_show_all(window);
}

int
main(int    argc,
     char **argv)
{
    /* Create application */
    MateUiApplication *app = mate_ui_application_new("org.mate.UiDemo",
                                                       G_APPLICATION_DEFAULT_FLAGS);

    /* Set application metadata (safe to do before run) */
    mate_ui_application_set_app_name(app, "MATE UI Demo");
    mate_ui_application_set_version(app, "1.0.0");
    mate_ui_application_set_comments(app, "A demonstration of libmateui capabilities");
    mate_ui_application_set_copyright(app, "Copyright © 2024 MATE Desktop Team");
    mate_ui_application_set_website(app, "https://mate-desktop.org");
    mate_ui_application_set_icon_name(app, "applications-development");
    mate_ui_application_set_authors(app, authors);
    mate_ui_application_set_license_type(app, GTK_LICENSE_GPL_2_0);
    mate_ui_application_set_help_uri(app, "help:mate-ui-demo");

    /* Connect signals */
    g_signal_connect(app, "startup", G_CALLBACK(startup_cb), NULL);
    g_signal_connect(app, "activate", G_CALLBACK(activate_cb), NULL);

    /* Run application */
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    /* Cleanup */
    if (inhibitor != NULL)
        mate_ui_session_uninhibit(inhibitor);

    g_object_unref(app);

    return status;
}
