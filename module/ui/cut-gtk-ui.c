/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2012  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gi18n-lib.h>
#include <gmodule.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-listener.h>
#include <cutter/cut-ui.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-verbose-level.h>
#include <cutter/cut-enum-types.h>
#include <cutter/cut-pipeline.h>
#include <cutter/cut-backtrace-entry.h>

#define CUT_TYPE_GTK_UI            cut_type_gtk_ui
#define CUT_GTK_UI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_GTK_UI, CutGtkUI))
#define CUT_GTK_UI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_GTK_UI, CutGtkUIClass))
#define CUT_IS_GTK_UI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_GTK_UI))
#define CUT_IS_GTK_UI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_GTK_UI))
#define CUT_GTK_UI_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_GTK_UI, CutGtkUIClass))

typedef struct _CutGtkUI CutGtkUI;
typedef struct _CutGtkUIClass CutGtkUIClass;

typedef struct _RowInfo RowInfo;
struct _RowInfo
{
    RowInfo *parent_row_info;
    CutGtkUI *ui;
    gchar *path;
    guint n_tests;
    guint n_completed_tests;
    gint pulse;
    guint update_pulse_id;
    CutTestResultStatus status;
};

typedef struct _TestCaseRowInfo
{
    RowInfo row_info;
    CutTestCase *test_case;
} TestCaseRowInfo;

typedef struct _TestIteratorRowInfo
{
    RowInfo row_info;
    TestCaseRowInfo *test_case_row_info;
    CutTestIterator *test_iterator;
} TestIteratorRowInfo;

typedef struct TestRowInfo
{
    RowInfo row_info;
    TestCaseRowInfo *test_case_row_info;
    CutTest *test;
} TestRowInfo;

typedef struct IteratedTestRowInfo
{
    RowInfo row_info;
    TestIteratorRowInfo *test_iterator_row_info;
    CutIteratedTest *iterated_test;
    gchar *data_name;
} IteratedTestRowInfo;

struct _CutGtkUI
{
    GObject        object;

    GtkWidget     *window;
    GtkProgressBar *progress_bar;
    GtkTreeView   *tree_view;
    GtkTreeStore  *logs;
    GtkStatusbar  *statusbar;
    GtkLabel      *summary;
    GtkWidget     *cancel_or_restart_button;

    CutRunContext *run_context;

    gboolean       running;
    guint          n_tests;
    guint          n_completed_tests;

    CutTestResultStatus status;
};

struct _CutGtkUIClass
{
    GObjectClass parent_class;
};

enum
{
    PROP_0
};

enum
{
    COLUMN_COLOR,
    COLUMN_TEST_STATUS,
    COLUMN_STATUS_ICON,
    COLUMN_PROGRESS_TEXT,
    COLUMN_PROGRESS_VALUE,
    COLUMN_PROGRESS_PULSE,
    COLUMN_PROGRESS_VISIBLE,
    COLUMN_NAME,
    COLUMN_DESCRIPTION,
    N_COLUMN
};

static GType cut_type_gtk_ui = 0;
static GObjectClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void     attach_to_run_context   (CutListener   *listener,
                                         CutRunContext *run_context);
static void     detach_from_run_context (CutListener   *listener,
                                         CutRunContext *run_context);
static gboolean run                     (CutUI         *ui,
                                         CutRunContext *run_context);

static void
class_init (CutGtkUIClass *klass)
{
    GObjectClass *gobject_class;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;
}

static void
setup_progress_bar (GtkBox *box, CutGtkUI *ui)
{
    GtkWidget *progress_bar;

    progress_bar = gtk_progress_bar_new();
    gtk_box_pack_start(box, progress_bar, TRUE, TRUE, 0);

    ui->progress_bar = GTK_PROGRESS_BAR(progress_bar);
    gtk_progress_bar_set_pulse_step(ui->progress_bar, 0.01);
}

static void
push_message (CutGtkUI *ui, const gchar *context, const gchar *format, ...)
{
    guint context_id;
    gchar *message;
    va_list args;

    context_id = gtk_statusbar_get_context_id(ui->statusbar, context);
    va_start(args, format);
    message = g_strdup_vprintf(format, args);
    va_end(args);
    gtk_statusbar_push(ui->statusbar, context_id, message);
    g_free(message);
}

static void
pop_message (CutGtkUI *ui, const gchar *context)
{
    guint context_id;

    context_id = gtk_statusbar_get_context_id(ui->statusbar, context);
    gtk_statusbar_pop(ui->statusbar, context_id);
}

static void
remove_all_messages (CutGtkUI *ui, const gchar *context)
{
#if GTK_CHECK_VERSION(2, 22, 0)
    guint context_id;

    context_id = gtk_statusbar_get_context_id(ui->statusbar, context);
    gtk_statusbar_remove_all(ui->statusbar, context_id);
#endif
}

static void
run_test (CutGtkUI *ui)
{
    ui->n_tests = 0;
    ui->n_completed_tests = 0;
    ui->status = CUT_TEST_RESULT_SUCCESS;

    gtk_tree_store_clear(ui->logs);

    remove_all_messages(ui, "test");
    remove_all_messages(ui, "iterated-test");
    remove_all_messages(ui, "test-suite");

    cut_run_context_add_listener(ui->run_context, CUT_LISTENER(ui));
    cut_run_context_start_async(ui->run_context);
}

static void
cb_cancel_or_restart (GtkToolButton *button, gpointer data)
{
    CutGtkUI *ui = data;

    if (strcmp(gtk_tool_button_get_stock_id(button), GTK_STOCK_CANCEL) == 0) {
        gtk_tool_button_set_stock_id(button, GTK_STOCK_REDO);
        cut_run_context_cancel(ui->run_context);
    } else {
        CutRunContext *pipeline;

        gtk_tool_button_set_stock_id(button, GTK_STOCK_CANCEL);

        pipeline = cut_pipeline_new_from_run_context(ui->run_context);
        g_object_unref(ui->run_context);
        ui->run_context = pipeline;
        run_test(ui);
    }
}

static void
cb_quit (GtkWidget *widget, gpointer data)
{
    CutGtkUI *ui = CUT_GTK_UI(data);

    gtk_widget_destroy(ui->window);
}

static void
cb_run_all_tests (GtkWidget *widget, gpointer data)
{
    CutGtkUI *ui = CUT_GTK_UI(data);

    cb_cancel_or_restart(GTK_TOOL_BUTTON(ui->cancel_or_restart_button),
                         (gpointer)ui);
}

#define CUT_WEBSITE_EN "http://cutter.sourceforge.net/"
#define CUT_WEBSITE_JA "http://cutter.sourceforge.net/index.html.ja"
#define CUT_TUTORIAL_EN "http://cutter.sourceforge.net/reference/tutorial.html"
#define CUT_TUTORIAL_JA "http://cutter.sourceforge.net/reference/ja/tutorial.html"
#define CUT_REFERENCE_EN "http://cutter.sourceforge.net/reference/"
#define CUT_REFERENCE_JA "http://cutter.sourceforge.net/reference/ja/"

static void
cb_open_uri (GtkWidget *widget, gpointer data)
{
    GError *error = NULL;

    GtkAction *action = GTK_ACTION(widget);

    gchar *uri = NULL;
    const gchar *name = gtk_action_get_name(GTK_ACTION(action));
    if (strcmp(name, "WebsiteEn") == 0) {
        uri = CUT_WEBSITE_EN;
    } else if (strcmp(name, "WebsiteJa") == 0) {
        uri = CUT_WEBSITE_JA;
    } else if (strcmp(name, "TutorialEn") == 0) {
        uri = CUT_TUTORIAL_EN;
    } else if (strcmp(name, "TutorialJa") == 0) {
        uri = CUT_TUTORIAL_JA;
    } else if (strcmp(name, "ReferenceEn") == 0) {
        uri = CUT_REFERENCE_EN;
    } else if (strcmp(name, "ReferenceJa") == 0) {
        uri = CUT_REFERENCE_JA;
    }

    if (uri) {
        gboolean status;
        status = gtk_show_uri(NULL, uri, gtk_get_current_event_time(), &error);
        if (status != TRUE) {
            /* fallback */
            GPtrArray *args = g_ptr_array_new();
            g_ptr_array_add(args, "chrome");
            g_ptr_array_add(args, uri);
            g_ptr_array_add(args, NULL);

            g_spawn_async(NULL, (gchar **)args->pdata, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error);
        }
    }
}

static GtkActionEntry menu_entries[] = {
    /* name, stock_id,label, accelerator, tooltip, callback */
    {"FileMenu", NULL, N_("_File"), NULL, "", NULL},
    {"Quit", GTK_STOCK_QUIT, N_("_Quit"), NULL, "", G_CALLBACK(cb_quit)},
    {"TestMenu", NULL, N_("_Test"), NULL, "", NULL},
    {"RunAllTests", GTK_STOCK_REDO, N_("_Run all tests"), NULL, "",
      G_CALLBACK(cb_run_all_tests)},
    {"HelpMenu", NULL, N_("_Help"), NULL, "", NULL},
    {"Website", NULL, N_("_Website"), NULL, "", NULL},
    {"WebsiteEn", NULL, N_("_Website English"), NULL, "",
     G_CALLBACK(cb_open_uri)},
    {"WebsiteJa", NULL, N_("_Website Japanese"), NULL, "",
     G_CALLBACK(cb_open_uri)},
    {"Tutorial", NULL, N_("_Tutorial"), NULL, "", NULL},
    {"TutorialEn", NULL, N_("_Tutorial English"), NULL, "",
     G_CALLBACK(cb_open_uri)},
    {"TutorialJa", NULL, N_("_Tutorial Japanese"), NULL, "",
     G_CALLBACK(cb_open_uri)},
    {"Reference", NULL, N_("_Reference"), NULL, "", NULL},
    {"ReferenceEn", NULL, N_("_Reference English"), NULL, "",
     G_CALLBACK(cb_open_uri)},
    {"ReferenceJa", NULL, N_("_Reference Japanese"), NULL, "",
     G_CALLBACK(cb_open_uri)}
};

static void
load_actions (CutGtkUI *ui, GtkUIManager *ui_manager)
{
    GtkActionGroup *action_group;
    gint i;

    action_group = gtk_action_group_new("MenuActions");

    for (i = 0; i < G_N_ELEMENTS(menu_entries); i++) {
        gtk_action_group_add_actions(action_group, &(menu_entries[i]), 1, ui);
    }
    gtk_ui_manager_insert_action_group(ui_manager, action_group, 0);

    g_object_unref(action_group);
}

static void
load_ui_file (GtkUIManager *ui_manager)
{
    const gchar *ui_data_dir;
    gchar *ui_file;

    ui_data_dir = g_getenv("CUT_UI_DATA_DIR");
    if (!ui_data_dir) {
#ifdef G_OS_WIN32
        ui_data_dir = cut_win32_ui_data_dir();
#else
        ui_data_dir = UI_DATA_DIR;
#endif
    }

    ui_file = g_build_filename(ui_data_dir, "gtk-menu.ui", NULL);
    gtk_ui_manager_add_ui_from_file(ui_manager, ui_file, NULL);
    g_free(ui_file);
}

static void
setup_menu_bar (GtkBox *box, CutGtkUI *ui)
{
    GtkUIManager *ui_manager;
    GtkWidget *menu_bar;

    ui_manager = gtk_ui_manager_new();
    load_actions(ui, ui_manager);
    load_ui_file(ui_manager);
    gtk_window_add_accel_group(GTK_WINDOW(ui->window),
                               gtk_ui_manager_get_accel_group(ui_manager));

    menu_bar = gtk_ui_manager_get_widget(ui_manager, "/menu-bar");
    if (menu_bar) {
        gtk_box_pack_start(GTK_BOX(box), menu_bar, FALSE, FALSE, 0);
    }

    g_object_unref(ui_manager);
}

static void
setup_cancel_or_restart_button (GtkToolbar *toolbar, CutGtkUI *ui)
{
    GtkToolItem *button;

    button = gtk_tool_button_new_from_stock(GTK_STOCK_CANCEL);
    gtk_toolbar_insert(toolbar, button, -1);

    g_signal_connect(button, "clicked",
                     G_CALLBACK(cb_cancel_or_restart), ui);

    ui->cancel_or_restart_button = GTK_WIDGET(button);
}

static void
setup_top_bar (GtkBox *box, CutGtkUI *ui)
{
    GtkWidget *hbox, *toolbar;

    toolbar = gtk_toolbar_new();
    gtk_toolbar_set_show_arrow(GTK_TOOLBAR(toolbar), FALSE);
    gtk_box_pack_start(GTK_BOX(box), toolbar, FALSE, TRUE, 0);
    setup_cancel_or_restart_button(GTK_TOOLBAR(toolbar), ui);

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(box, hbox, FALSE, TRUE, 0);

    setup_progress_bar(GTK_BOX(hbox), ui);
}

static void
setup_summary_label (GtkBox *box, CutGtkUI *ui)
{
    GtkWidget *summary;

    summary = gtk_label_new(_("Ready"));
    gtk_box_pack_start(box, summary, FALSE, TRUE, 0);

    ui->summary = GTK_LABEL(summary);
}

static void
setup_tree_view_columns (GtkTreeView *tree_view)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    column = gtk_tree_view_column_new();

    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_set_title(column, _("Name"));
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_add_attribute(column, renderer,
                                       "pixbuf", COLUMN_STATUS_ICON);

    renderer = gtk_cell_renderer_progress_new();
    gtk_tree_view_column_pack_end(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer,
                                        "text", COLUMN_PROGRESS_TEXT,
                                        "value", COLUMN_PROGRESS_VALUE,
                                        "pulse", COLUMN_PROGRESS_PULSE,
                                        "visible", COLUMN_PROGRESS_VISIBLE,
                                        NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_set_attributes(column, renderer,
                                        "text", COLUMN_NAME,
                                        "background", COLUMN_COLOR,
                                        NULL);
    gtk_tree_view_column_set_sort_column_id(column, COLUMN_NAME);
    gtk_tree_view_append_column(tree_view, column);

    renderer = g_object_new(GTK_TYPE_CELL_RENDERER_TEXT,
                            "font", "Monospace",
                            NULL);
    column = gtk_tree_view_column_new_with_attributes(_("Description"), renderer,
                                                      "text", COLUMN_DESCRIPTION,
                                                      "background", COLUMN_COLOR,
                                                      NULL);
    gtk_tree_view_append_column(tree_view, column);
}

static void
setup_tree_view (GtkBox *box, CutGtkUI *ui)
{
    GtkWidget *tree_view, *scrolled_window;
    GtkTreeStore *tree_store;

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(box, scrolled_window, TRUE, TRUE, 0);

    tree_store = gtk_tree_store_new(N_COLUMN,
                                    G_TYPE_STRING,
                                    G_TYPE_INT,
                                    GDK_TYPE_PIXBUF,
                                    G_TYPE_STRING,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_BOOLEAN,
                                    G_TYPE_STRING, G_TYPE_STRING);
    ui->logs = tree_store;

    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(tree_store));
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
    ui->tree_view = GTK_TREE_VIEW(tree_view);
    setup_tree_view_columns(ui->tree_view);
}

static void
setup_statusbar (GtkBox *box, CutGtkUI *ui)
{
    GtkWidget *statusbar;

    statusbar = gtk_statusbar_new();
    gtk_box_pack_start(box, statusbar, FALSE, FALSE, 0);
    ui->statusbar = GTK_STATUSBAR(statusbar);
}

static gboolean
cb_destroy (GtkWidget *widget, gpointer data)
{
    CutGtkUI *ui = data;

    ui->window = NULL;
    gtk_main_quit();

    return TRUE;
}

static gboolean
cb_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    if (event->state == GDK_CONTROL_MASK && event->keyval == GDK_q) {
        gtk_widget_destroy(widget);
        return TRUE;
    }

    return FALSE;
}

static void
setup_window (CutGtkUI *ui)
{
    GtkWidget *window, *vbox;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    gtk_window_set_title(GTK_WINDOW(window), "Cutter");

    g_signal_connect(window, "destroy",
                     G_CALLBACK(cb_destroy), ui);
    g_signal_connect(window, "key-press-event",
                     G_CALLBACK(cb_key_press_event), NULL);

    ui->window = window;

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    setup_menu_bar(GTK_BOX(vbox), ui);
    setup_top_bar(GTK_BOX(vbox), ui);
    setup_summary_label(GTK_BOX(vbox), ui);
    setup_tree_view(GTK_BOX(vbox), ui);
    setup_statusbar(GTK_BOX(vbox), ui);

    gtk_window_set_focus(GTK_WINDOW(window), GTK_WIDGET(ui->tree_view));
}

static void
init (CutGtkUI *ui)
{
    ui->run_context = NULL;
    ui->n_tests = 0;
    ui->n_completed_tests = 0;
    ui->status = CUT_TEST_RESULT_SUCCESS;

    setup_window(ui);
}

static void
listener_init (CutListenerClass *listener)
{
    listener->attach_to_run_context   = attach_to_run_context;
    listener->detach_from_run_context = detach_from_run_context;
}

static void
ui_init (CutUIClass *ui)
{
    ui->run = run;
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutGtkUIClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutGtkUI),
            0,
            (GInstanceInitFunc) init,
        };

    static const GInterfaceInfo ui_info =
        {
            (GInterfaceInitFunc) ui_init,
            NULL,
            NULL
        };

    static const GInterfaceInfo listener_info =
        {
            (GInterfaceInitFunc) listener_init,
            NULL,
            NULL
        };

    cut_type_gtk_ui = g_type_module_register_type(type_module,
                                                  G_TYPE_OBJECT,
                                                  "CutGtkUI",
                                                  &info, 0);

    g_type_module_add_interface(type_module,
                                cut_type_gtk_ui,
                                CUT_TYPE_UI,
                                &ui_info);

    g_type_module_add_interface(type_module,
                                cut_type_gtk_ui,
                                CUT_TYPE_LISTENER,
                                &listener_info);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_gtk_ui)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_gtk_ui));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_GTK_UI, first_property, var_args);
}

static void
dispose (GObject *object)
{
    CutGtkUI *ui = CUT_GTK_UI(object);

    if (ui->logs) {
        g_object_unref(ui->logs);
        ui->logs = NULL;
    }
    if (ui->window) {
        gtk_widget_destroy(ui->window);
        ui->window = NULL;
    }

    if (ui->run_context) {
        g_object_unref(ui->run_context);
        ui->run_context = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static const gchar *
status_to_color (CutTestResultStatus status, gboolean only_if_not_success)
{
    const gchar *color = "white";

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        if (only_if_not_success)
            color = NULL;
        else
            color = "light green";
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        color = "light blue";
        break;
      case CUT_TEST_RESULT_OMISSION:
        color = "blue";
        break;
      case CUT_TEST_RESULT_PENDING:
        color = "yellow";
        break;
      case CUT_TEST_RESULT_FAILURE:
        color = "red";
        break;
      case CUT_TEST_RESULT_ERROR:
        color = "purple";
        break;
      case CUT_TEST_RESULT_CRASH:
        color = "red";
        break;
      default:
        break;
    }

    return color;
}

static GdkPixbuf *
get_status_icon_by_id (GtkTreeView *tree_view, const gchar *stock_id)
{
    return gtk_widget_render_icon(GTK_WIDGET(tree_view),
                                  stock_id, GTK_ICON_SIZE_MENU,
                                  NULL);
}

static GdkPixbuf *
get_status_icon (GtkTreeView *tree_view, CutTestResultStatus status)
{
    const gchar *stock_id = "";

    switch (status) {
    case CUT_TEST_RESULT_SUCCESS:
        stock_id = GTK_STOCK_YES;
        break;
    case CUT_TEST_RESULT_NOTIFICATION:
        stock_id = GTK_STOCK_DIALOG_WARNING;
        break;
    case CUT_TEST_RESULT_OMISSION:
        stock_id = GTK_STOCK_DIALOG_ERROR;
        break;
    case CUT_TEST_RESULT_PENDING:
        stock_id = GTK_STOCK_DIALOG_ERROR;
        break;
    case CUT_TEST_RESULT_FAILURE:
        stock_id = GTK_STOCK_STOP;
        break;
    case CUT_TEST_RESULT_ERROR:
        stock_id = GTK_STOCK_CANCEL;
        break;
    case CUT_TEST_RESULT_CRASH:
        stock_id = GTK_STOCK_STOP;
        break;
    default:
        stock_id = GTK_STOCK_INFO;
        break;
    }

    return get_status_icon_by_id(tree_view, stock_id);
}

static gchar *
generate_summary_message (CutRunContext *run_context)
{
    guint n_tests, n_assertions, n_failures, n_errors;
    guint n_pendings, n_notifications, n_omissions;

    n_tests = cut_run_context_get_n_tests(run_context);
    n_assertions = cut_run_context_get_n_assertions(run_context);
    n_failures = cut_run_context_get_n_failures(run_context);
    n_errors = cut_run_context_get_n_errors(run_context);
    n_pendings = cut_run_context_get_n_pendings(run_context);
    n_notifications = cut_run_context_get_n_notifications(run_context);
    n_omissions = cut_run_context_get_n_omissions(run_context);

    return g_strdup_printf(_("%d test(s), %d assertion(s), %d failure(s), "
                             "%d error(s), %d pending(s), %d omission(s), "
                             "%d notification(s)"),
                           n_tests, n_assertions, n_failures, n_errors,
                           n_pendings, n_omissions, n_notifications);
}

static gchar *
generate_short_summary_message (CutRunContext *run_context)
{
    guint n_tests, n_assertions, n_failures, n_errors;
    guint n_pendings, n_notifications, n_omissions;

    n_tests = cut_run_context_get_n_tests(run_context);
    n_assertions = cut_run_context_get_n_assertions(run_context);
    n_failures = cut_run_context_get_n_failures(run_context);
    n_errors = cut_run_context_get_n_errors(run_context);
    n_pendings = cut_run_context_get_n_pendings(run_context);
    n_notifications = cut_run_context_get_n_notifications(run_context);
    n_omissions = cut_run_context_get_n_omissions(run_context);

    return g_strdup_printf(_("%dT:%dA:%dF:%d:E:%dP:%dO:%dN"),
                           n_tests, n_assertions, n_failures, n_errors,
                           n_pendings, n_omissions, n_notifications);
}

static void
update_button_sensitive (CutGtkUI *ui)
{
    GtkToolButton *button;

    button = GTK_TOOL_BUTTON(ui->cancel_or_restart_button);
    if (ui->running) {
        gtk_tool_button_set_stock_id(button, GTK_STOCK_CANCEL);
    } else {
        gtk_tool_button_set_stock_id(button, GTK_STOCK_REDO);
    }
}

static void
update_progress_color (GtkProgressBar *bar, CutTestResultStatus status)
{
    GtkStyle *style;

    style = gtk_style_new();
    gdk_color_parse(status_to_color(status, FALSE),
                    &(style->bg[GTK_STATE_PRELIGHT]));
    gtk_widget_set_style(GTK_WIDGET(bar), style);
    g_object_unref(style);
}

static void
update_progress_bar (CutGtkUI *ui)
{
    GtkProgressBar *bar;
    guint n_tests, n_completed_tests;

    n_tests = ui->n_tests;
    n_completed_tests = ui->n_completed_tests;
    bar = ui->progress_bar;

    update_progress_color(bar, ui->status);

    if (n_tests > 0) {
        gdouble fraction;
        gchar *text;

        fraction = n_completed_tests / (gdouble)n_tests;
        gtk_progress_bar_set_fraction(ui->progress_bar, fraction);

        text = g_strdup_printf(_("%u/%u (%u%%): %.1fs"),
                               n_completed_tests, n_tests,
                               (guint)(fraction * 100),
                               cut_run_context_get_elapsed(ui->run_context));
        gtk_progress_bar_set_text(bar, text);
        g_free(text);
    } else {
        gtk_progress_bar_pulse(bar);
    }
}

static void
cb_ready_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                     guint n_test_cases, guint n_tests, CutGtkUI *ui)
{
    ui->running = TRUE;
    ui->n_tests = n_tests;

    update_button_sensitive(ui);
    push_message(ui, "test-suite",
                 _("Starting test suite %s..."),
                 cut_test_get_name(CUT_TEST(test_suite)));
}

static void
update_row_status (RowInfo *row_info)
{
    CutGtkUI *ui;
    GtkTreeIter iter;

    ui = row_info->ui;
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &iter, row_info->path)) {
        GdkPixbuf *icon;
        icon = get_status_icon(ui->tree_view, row_info->status);
        gtk_tree_store_set(ui->logs, &iter,
                           COLUMN_STATUS_ICON, icon,
                           -1);
        g_object_unref(icon);
    }
}

static void
free_test_case_row_info (TestCaseRowInfo *info)
{
    RowInfo *row_info;

    g_object_unref(info->test_case);

    row_info = &(info->row_info);
    g_object_unref(row_info->ui);
    g_free(row_info->path);

    g_free(info);
}

static void
disable_progress (RowInfo *row_info)
{
    CutGtkUI *ui;
    GtkTreeIter iter;

    if (row_info->update_pulse_id) {
        g_source_remove(row_info->update_pulse_id);
        row_info->update_pulse_id = 0;
    }

    ui = row_info->ui;
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &iter, row_info->path)) {
        gtk_tree_store_set(ui->logs, &iter,
                           COLUMN_PROGRESS_VISIBLE, FALSE,
                           COLUMN_PROGRESS_PULSE, -1,
                           -1);
    }
}

static void
free_test_row_info (TestRowInfo *info)
{
    RowInfo *row_info;

    row_info = &(info->row_info);
    disable_progress(row_info);

    g_object_unref(info->test);
    g_object_unref(row_info->ui);
    g_free(row_info->path);

    g_free(info);
}

static void
update_status (RowInfo *row_info, CutTestResultStatus status)
{
    RowInfo *parent_row_info;
    CutGtkUI *ui;

    ui = row_info->ui;
    row_info->status = status;

    for (parent_row_info = row_info->parent_row_info;
         parent_row_info;
         parent_row_info = parent_row_info->parent_row_info) {
        if (parent_row_info->status < status) {
            parent_row_info->status = status;
        } else {
            break;
        }
    }

    if (ui->status < status)
        ui->status = status;
}

static gchar *
append_row (CutGtkUI *ui, const gchar *parent_path,
            const gchar *name, const gchar *description)
{
    GtkTreeIter iter;
    GdkPixbuf *icon;

    if (parent_path) {
        GtkTreeIter parent_iter;
        gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &parent_iter,
                                            parent_path);
        gtk_tree_store_append(ui->logs, &iter, &parent_iter);
    } else {
        gtk_tree_store_append(ui->logs, &iter, NULL);
    }

    icon = get_status_icon_by_id(ui->tree_view, GTK_STOCK_MEDIA_PLAY);
    gtk_tree_store_set(ui->logs, &iter,
                       COLUMN_NAME, name,
                       COLUMN_DESCRIPTION, description,
                       COLUMN_PROGRESS_PULSE, -1,
                       COLUMN_PROGRESS_VISIBLE, TRUE,
                       COLUMN_STATUS_ICON, icon,
                       -1);
    g_object_unref(icon);

    return gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(ui->logs),
                                               &iter);
}

static void
update_row (CutGtkUI *ui, RowInfo *row_info)
{
    GtkTreeIter iter;

    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &iter, row_info->path)) {
        gdouble fraction;
        gint percent;
        gchar *text;
        GdkPixbuf *icon;

        fraction = row_info->n_completed_tests / (gdouble)(row_info->n_tests);
        percent = (gint)(fraction * 100);
        text = g_strdup_printf("%d/%d (%d%%)",
                               row_info->n_completed_tests,
                               row_info->n_tests, percent);
        icon = get_status_icon(ui->tree_view, row_info->status);
        gtk_tree_store_set(ui->logs, &iter,
                           COLUMN_PROGRESS_TEXT, text,
                           COLUMN_PROGRESS_VALUE, percent,
                           COLUMN_STATUS_ICON, icon,
                           COLUMN_COLOR, status_to_color(row_info->status, TRUE),
                           -1);
        g_free(text);
        g_object_unref(icon);
    }
}

static gboolean
timeout_cb_pulse_test (gpointer data)
{
    RowInfo *row_info = data;
    CutGtkUI *ui;
    GtkTreeIter iter;

    ui = row_info->ui;
    row_info->pulse++;
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &iter, row_info->path)) {
        gtk_tree_store_set(ui->logs, &iter,
                           COLUMN_PROGRESS_PULSE, row_info->pulse,
                           -1);
    }
    return ui->running;
}

static void
expand_row (CutGtkUI *ui, const gchar *path)
{
    GtkTreePath *tree_path;

    tree_path = gtk_tree_path_new_from_string(path);
    gtk_tree_view_expand_to_path(ui->tree_view, tree_path);
    gtk_tree_view_scroll_to_cell(ui->tree_view, tree_path, NULL, TRUE, 0, 0.5);
    gtk_tree_path_free(tree_path);
}

static void
update_test_row_status (RowInfo *row_info)
{
    CutGtkUI *ui;
    GtkTreeIter iter;

    ui = row_info->ui;

    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &iter, row_info->path)) {
        GdkPixbuf *icon;
        icon = get_status_icon(ui->tree_view, row_info->status);
        gtk_tree_store_set(ui->logs, &iter,
                           COLUMN_STATUS_ICON, icon,
                           COLUMN_PROGRESS_VISIBLE, FALSE,
                           COLUMN_COLOR, status_to_color(row_info->status, TRUE),
                           -1);
        g_object_unref(icon);

        if (row_info->status != CUT_TEST_RESULT_SUCCESS) {
            GtkTreePath *path;
            path = gtk_tree_model_get_path(GTK_TREE_MODEL(ui->logs), &iter);
            gtk_tree_view_expand_to_path(ui->tree_view, path);
            gtk_tree_view_scroll_to_cell(ui->tree_view, path, NULL,
                                         TRUE, 0, 0.5);
            gtk_tree_path_free(path);
        }
    }
}


static void
append_test_result_row_under (CutGtkUI *ui, CutTestResult *result,
                              GtkTreeIter *test_row_iter,
                              GtkTreeIter *result_row_iter)
{
    CutTestResultStatus status;
    const GList *node;
    GString *name;
    const gchar *message;
    GdkPixbuf *icon;

    status = cut_test_result_get_status(result);
    message = cut_test_result_get_message(result);

    name = g_string_new(NULL);
    for (node = cut_test_result_get_backtrace(result);
         node;
         node = g_list_next(node)) {
        CutBacktraceEntry *entry = node->data;

        cut_backtrace_entry_format_string(entry, name);
        if (g_list_next(node))
            g_string_append(name, "\n");
    }

    icon = get_status_icon(ui->tree_view, status);

    gtk_tree_store_append(ui->logs, result_row_iter, test_row_iter);
    gtk_tree_store_set(ui->logs, result_row_iter,
                       COLUMN_NAME, name->str,
                       COLUMN_DESCRIPTION, message,
                       COLUMN_STATUS_ICON, icon,
                       COLUMN_COLOR, status_to_color(status, TRUE),
                       -1);
    g_string_free(name, TRUE);
    g_object_unref(icon);
}

static void
append_test_result_row (RowInfo *row_info, CutTestResult *result)
{
    CutGtkUI *ui;
    GtkTreeIter test_row_iter;

    ui = row_info->ui;
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &test_row_iter, row_info->path)) {
        GtkTreePath *path;
        GtkTreeIter iter;

        append_test_result_row_under(ui, result, &test_row_iter, &iter);

        path = gtk_tree_model_get_path(GTK_TREE_MODEL(ui->logs), &iter);
        gtk_tree_view_expand_to_path(ui->tree_view, path);
        gtk_tree_view_scroll_to_cell(ui->tree_view, path, NULL,
                                     TRUE, 0, 0.5);
        gtk_tree_path_free(path);
    }

    g_object_unref(result);
}

static void
update_summary (CutGtkUI *ui)
{
    gchar *summary, *short_summary, *title;

    summary = generate_summary_message(ui->run_context);
    gtk_label_set_text(ui->summary, summary);
    g_free(summary);

    short_summary = generate_short_summary_message(ui->run_context);
    title = g_strdup_printf("%s - Cutter", short_summary);
    gtk_window_set_title(GTK_WINDOW(ui->window), title);
    g_free(short_summary);
    g_free(title);
}

static void
cb_pass_assertion (CutRunContext *run_context,
                   CutTest *test, CutTestContext *test_context,
                   gpointer data)
{
    RowInfo *row_info = data;

    /* slow */
    if (g_random_int_range(0, 1000) == 0) {
        update_summary(row_info->ui);
    }
}

static void
cb_success_test (CutRunContext *run_context,
                 CutTest *test, CutTestContext *context, CutTestResult *result,
                 gpointer data)
{
    RowInfo *row_info = data;

    if (row_info->status == -1) {
        row_info->status = CUT_TEST_RESULT_SUCCESS;

        update_test_row_status(row_info);
    }
}

static void
cb_failure_test (CutRunContext *run_context,
                 CutTest *test, CutTestContext *context, CutTestResult *result,
                 gpointer data)
{
    RowInfo *row_info = data;

    update_status(row_info, CUT_TEST_RESULT_FAILURE);

    update_test_row_status(row_info);
    append_test_result_row(row_info, result);
}

static void
cb_error_test (CutRunContext *run_context,
               CutTest *test, CutTestContext *context, CutTestResult *result,
               gpointer data)
{
    RowInfo *row_info = data;

    update_status(row_info, CUT_TEST_RESULT_ERROR);

    update_test_row_status(row_info);
    append_test_result_row(row_info, result);
}

static void
cb_pending_test (CutRunContext *run_context,
                 CutTest *test, CutTestContext *context, CutTestResult *result,
                 gpointer data)
{
    RowInfo *row_info = data;

    update_status(row_info, CUT_TEST_RESULT_PENDING);

    update_test_row_status(row_info);
    append_test_result_row(row_info, result);
}

static void
cb_notification_test (CutRunContext *run_context,
                      CutTest *test, CutTestContext *context,
                      CutTestResult *result, gpointer data)
{
    RowInfo *row_info = data;

    update_status(row_info, CUT_TEST_RESULT_NOTIFICATION);

    update_test_row_status(row_info);
    append_test_result_row(row_info, result);
}

static void
cb_omission_test (CutRunContext *run_context,
                  CutTest *test, CutTestContext *context,
                  CutTestResult *result, gpointer data)
{
    RowInfo *row_info = data;

    update_status(row_info, CUT_TEST_RESULT_OMISSION);

    update_test_row_status(row_info);
    append_test_result_row(row_info, result);
}

static void
cb_crash_test (CutRunContext *run_context,
               CutTest *test, CutTestContext *context,
               CutTestResult *result, gpointer data)
{
    RowInfo *row_info = data;

    update_status(row_info, CUT_TEST_RESULT_CRASH);

    update_test_row_status(row_info);
    append_test_result_row(row_info, result);
}

static void
increment_n_completed_tests (RowInfo *row_info)
{
    CutGtkUI *ui;
    RowInfo *parent_row_info;

    ui = row_info->ui;
    for (parent_row_info = row_info->parent_row_info;
         parent_row_info;
         parent_row_info = parent_row_info->parent_row_info) {
        parent_row_info->n_completed_tests++;
        update_row(ui, parent_row_info);
    }

    ui->n_completed_tests++;
    update_summary(ui);
}

static void
cb_complete_test (CutRunContext *run_context,
                  CutTest *test, CutTestContext *test_context,
                  gboolean success, gpointer data)
{
    RowInfo *row_info;
    TestRowInfo *info = data;
    CutGtkUI *ui;

    row_info = &(info->row_info);
    increment_n_completed_tests(row_info);
    ui = row_info->ui;
    pop_message(ui, "test");
    free_test_row_info(info);

    update_progress_bar(ui);

#define DISCONNECT(name, user_data)                                     \
    g_signal_handlers_disconnect_by_func(run_context,                   \
                                         G_CALLBACK(cb_ ## name),       \
                                         user_data)
    DISCONNECT(pass_assertion, row_info);
    DISCONNECT(success_test, row_info);
    DISCONNECT(failure_test, row_info);
    DISCONNECT(error_test, row_info);
    DISCONNECT(pending_test, row_info);
    DISCONNECT(notification_test, row_info);
    DISCONNECT(omission_test, row_info);
    DISCONNECT(crash_test, row_info);
    DISCONNECT(complete_test, info);
#undef DISCONNECT
}

static void
cb_start_test (CutRunContext *run_context,
               CutTest *test,
               CutTestContext *test_context, gpointer data)
{
    RowInfo *row_info;
    TestRowInfo *info;
    CutGtkUI *ui;

    info = g_new0(TestRowInfo, 1);
    info->test_case_row_info = data;
    info->test = g_object_ref(test);

    row_info = &(info->row_info);
    row_info->parent_row_info = &(info->test_case_row_info->row_info);
    ui = row_info->parent_row_info->ui;
    row_info->ui = g_object_ref(ui);
    row_info->status = -1;
    row_info->pulse = 0;
    row_info->update_pulse_id = 0;
    row_info->path = append_row(ui, info->test_case_row_info->row_info.path,
                                cut_test_get_name(test),
                                cut_test_get_description(test));

    push_message(ui, "test",
                 _("Running test: %s"), cut_test_get_name(test));
    /* Always expand running test case row. Is it OK? */
    expand_row(ui, row_info->path);
    row_info->update_pulse_id = g_timeout_add(10, timeout_cb_pulse_test,
                                              row_info);


#define CONNECT(name, user_data)                                        \
    g_signal_connect(run_context, #name, G_CALLBACK(cb_ ## name), user_data)

    CONNECT(pass_assertion, row_info);
    CONNECT(success_test, row_info);
    CONNECT(failure_test, row_info);
    CONNECT(error_test, row_info);
    CONNECT(pending_test, row_info);
    CONNECT(notification_test, row_info);
    CONNECT(omission_test, row_info);
    CONNECT(crash_test, row_info);
    CONNECT(complete_test, info);
#undef CONNECT
}

static void
free_iterated_test_row_info (IteratedTestRowInfo *info)
{
    RowInfo *row_info;

    row_info = &(info->row_info);
    disable_progress(row_info);

    g_object_unref(info->iterated_test);
    g_free(info->data_name);
    g_object_unref(row_info->ui);
    g_free(row_info->path);

    g_free(info);
}

static void
cb_complete_iterated_test (CutRunContext *run_context,
                           CutIteratedTest *iterated_test,
                           CutTestContext *test_context,
                           gboolean success, gpointer data)
{
    RowInfo *row_info;
    IteratedTestRowInfo *info = data;
    CutGtkUI *ui;

    row_info = &(info->row_info);
    ui = row_info->ui;
    increment_n_completed_tests(row_info);
    pop_message(ui, "iterated-test");
    free_iterated_test_row_info(info);

#define DISCONNECT(name, user_data)                                     \
    g_signal_handlers_disconnect_by_func(run_context,                   \
                                         G_CALLBACK(cb_ ## name),       \
                                         user_data)
    DISCONNECT(pass_assertion, row_info);
    DISCONNECT(success_test, row_info);
    DISCONNECT(failure_test, row_info);
    DISCONNECT(error_test, row_info);
    DISCONNECT(pending_test, row_info);
    DISCONNECT(notification_test, row_info);
    DISCONNECT(omission_test, row_info);
    DISCONNECT(crash_test, row_info);
    DISCONNECT(complete_iterated_test, info);
#undef DISCONNECT
}

static void
cb_start_iterated_test (CutRunContext *run_context,
                        CutIteratedTest *iterated_test,
                        CutTestContext *test_context, gpointer data)
{
    RowInfo *row_info;
    IteratedTestRowInfo *info;
    CutGtkUI *ui;

    info = g_new0(IteratedTestRowInfo, 1);
    info->test_iterator_row_info = data;
    info->iterated_test = g_object_ref(iterated_test);
    if (cut_test_context_have_data(test_context)) {
        CutTestData *data;
        data = cut_test_context_get_current_data(test_context);
        info->data_name = g_strdup(cut_test_data_get_name(data));
    }
    if (!info->data_name) {
        info->data_name = g_strdup(cut_test_get_name(CUT_TEST(iterated_test)));
    }
    row_info = &(info->row_info);
    row_info->parent_row_info = &(info->test_iterator_row_info->row_info);
    ui = row_info->parent_row_info->ui;
    row_info->ui = g_object_ref(ui);
    row_info->status = -1;
    row_info->pulse = 0;
    row_info->update_pulse_id = 0;
    row_info->path = append_row(ui,
                                row_info->parent_row_info->path,
                                info->data_name, NULL);

    push_message(ui,
                 "iterated-test",
                 _("Running iterated test: %s (%s)"),
                 cut_test_get_name(CUT_TEST(info->iterated_test)),
                 info->data_name);
    /* Always expand running test case row. Is it OK? */
    expand_row(ui, row_info->path);

#define CONNECT(name, user_data)                                        \
    g_signal_connect(run_context, #name, G_CALLBACK(cb_ ## name), user_data)

    CONNECT(pass_assertion, row_info);
    CONNECT(success_test, row_info);
    CONNECT(failure_test, row_info);
    CONNECT(error_test, row_info);
    CONNECT(pending_test, row_info);
    CONNECT(notification_test, row_info);
    CONNECT(omission_test, row_info);
    CONNECT(crash_test, row_info);
    CONNECT(complete_iterated_test, info);
#undef CONNECT
}

static void
collapse_test_iterator_row (TestIteratorRowInfo *info)
{
    RowInfo *row_info;
    CutGtkUI *ui;
    GtkTreeIter iter;

    row_info = &(info->row_info);
    ui = row_info->ui;

    if (row_info->status == CUT_TEST_RESULT_SUCCESS &&
        gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &iter, row_info->path)) {

        GtkTreePath *path;
        path = gtk_tree_model_get_path(GTK_TREE_MODEL(ui->logs), &iter);
        gtk_tree_view_collapse_row(ui->tree_view, path);
        gtk_tree_path_free(path);
    }
}

static void
free_test_iterator_row_info (TestIteratorRowInfo *info)
{
    RowInfo *row_info;
    CutGtkUI *ui;

    row_info = &(info->row_info);
    ui = row_info->ui;

    g_object_unref(info->test_iterator);
    g_object_unref(ui);
    g_free(row_info->path);

    g_free(info);
}

static void
cb_complete_test_iterator (CutRunContext *run_context,
                           CutTestCase *test_case, gboolean success,
                           gpointer data)
{
    RowInfo *row_info;
    TestIteratorRowInfo *info = data;

    row_info = &(info->row_info);
    update_summary(row_info->ui);
    update_row_status(row_info);
    collapse_test_iterator_row(info);
    free_test_iterator_row_info(info);

    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_start_iterated_test),
                                         data);
    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_complete_test_iterator),
                                         data);
}

static void
cb_ready_test_iterator (CutRunContext *run_context,
                        CutTestIterator *test_iterator, guint n_tests,
                        TestCaseRowInfo *test_case_row_info)
{
    RowInfo *row_info;
    TestIteratorRowInfo *info;
    CutGtkUI *ui;

    info = g_new0(TestIteratorRowInfo, 1);
    info->test_case_row_info = test_case_row_info;
    info->test_iterator = g_object_ref(test_iterator);
    row_info = &(info->row_info);
    row_info->parent_row_info = &(test_case_row_info->row_info);
    ui = row_info->parent_row_info->ui;
    row_info->ui = g_object_ref(ui);
    row_info->n_tests = n_tests;
    row_info->n_completed_tests = 0;
    row_info->status = CUT_TEST_RESULT_SUCCESS;

    ui->n_tests += n_tests - 1;
    row_info->parent_row_info->n_tests += n_tests - 1;

    row_info->path = append_row(ui,
                                row_info->parent_row_info->path,
                                cut_test_get_name(CUT_TEST(test_iterator)),
                                cut_test_get_description(CUT_TEST(test_iterator)));

    g_signal_connect(run_context, "start-iterated-test",
                     G_CALLBACK(cb_start_iterated_test), info);
    g_signal_connect(run_context, "complete-test-iterator",
                     G_CALLBACK(cb_complete_test_iterator), info);
}

static void
collapse_test_case_row (TestCaseRowInfo *info)
{
    CutGtkUI *ui;
    GtkTreeIter iter;

    ui = info->row_info.ui;

    if (info->row_info.status == CUT_TEST_RESULT_SUCCESS &&
        gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &iter, info->row_info.path)) {

        GtkTreePath *path;
        path = gtk_tree_model_get_path(GTK_TREE_MODEL(ui->logs), &iter);
        gtk_tree_view_collapse_row(ui->tree_view, path);
        gtk_tree_path_free(path);
    }
}

static void
cb_complete_test_case (CutRunContext *run_context,
                       CutTestCase *test_case, gboolean success,
                       gpointer data)
{
    RowInfo *row_info;
    TestCaseRowInfo *info = data;

    row_info = &(info->row_info);
    update_summary(row_info->ui);
    update_row_status(row_info);
    collapse_test_case_row(info);
    free_test_case_row_info(info);
    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_start_test),
                                         data);
    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_ready_test_iterator),
                                         data);
    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_complete_test_case),
                                         data);
}

static void
cb_ready_test_case (CutRunContext *run_context, CutTestCase *test_case,
                    guint n_tests, CutGtkUI *ui)
{
    RowInfo *row_info;
    TestCaseRowInfo *info;

    info = g_new0(TestCaseRowInfo, 1);
    info->test_case = g_object_ref(test_case);
    row_info = &(info->row_info);
    row_info->parent_row_info = NULL;
    row_info->ui = g_object_ref(ui);
    row_info->n_tests = n_tests;
    row_info->n_completed_tests = 0;
    row_info->status = CUT_TEST_RESULT_SUCCESS;

    row_info->path = append_row(row_info->ui, NULL,
                                cut_test_get_name(CUT_TEST(test_case)),
                                cut_test_get_description(CUT_TEST(test_case)));

    g_signal_connect(run_context, "start-test",
                     G_CALLBACK(cb_start_test), info);
    g_signal_connect(run_context, "ready-test-iterator",
                     G_CALLBACK(cb_ready_test_iterator), info);
    g_signal_connect(run_context, "complete-test-case",
                     G_CALLBACK(cb_complete_test_case), info);
}

static void
cb_complete_run (CutRunContext *run_context, gboolean success, CutGtkUI *ui)
{
    ui->running = FALSE;
    update_button_sensitive(ui);
}

static void
cb_complete_test_suite (CutRunContext *run_context,
                        CutTestSuite *test_suite,
                        gboolean success,
                        CutGtkUI *ui)
{
    gchar *summary;

    pop_message(ui, "test-suite");

    summary = generate_summary_message(ui->run_context);
    push_message(ui, "test-suite",
                 _("Finished in %0.1f seconds: %s"),
                 cut_run_context_get_elapsed(ui->run_context),
                 summary);
    g_free(summary);
}

static void
cb_error (CutRunContext *run_context, GError *error, CutGtkUI *ui)
{
    g_print("SystemError: %s:%d: %s\n",
            g_quark_to_string(error->domain),
            error->code,
            error->message);
}

static void
connect_to_run_context (CutGtkUI *ui, CutRunContext *run_context)
{
#define CONNECT(name) \
    g_signal_connect(run_context, #name, G_CALLBACK(cb_ ## name), ui)

    CONNECT(ready_test_suite);
    CONNECT(ready_test_case);

    CONNECT(complete_test_suite);
    CONNECT(complete_run);

    CONNECT(error);
#undef CONNECT
}

static void
disconnect_from_run_context (CutGtkUI *ui, CutRunContext *run_context)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(run_context,                   \
                                         G_CALLBACK(cb_ ## name),       \
                                         ui)

    DISCONNECT(ready_test_suite);
    DISCONNECT(ready_test_case);

    DISCONNECT(complete_test_suite);
    DISCONNECT(complete_run);

    DISCONNECT(error);
#undef DISCONNECT
}

static void
attach_to_run_context (CutListener *listener,
                       CutRunContext *run_context)
{
    connect_to_run_context(CUT_GTK_UI(listener), run_context);
}

static void
detach_from_run_context (CutListener *listener,
                         CutRunContext *run_context)
{
    disconnect_from_run_context(CUT_GTK_UI(listener), run_context);
}

static gboolean
run (CutUI *ui, CutRunContext *run_context)
{
    CutGtkUI *gtk_ui = CUT_GTK_UI(ui);
    CutRunContext *pipeline;

    if (CUT_IS_STREAM_READER(run_context)) {
        pipeline = run_context;
        g_object_ref(pipeline);
    } else {
        pipeline = cut_pipeline_new_from_run_context(run_context);
    }

    gtk_ui->run_context = pipeline;
    gtk_widget_show_all(gtk_ui->window);
    gtk_tree_store_clear(gtk_ui->logs);

    run_test(gtk_ui);

    gtk_main();

    return TRUE;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
