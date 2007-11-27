/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#  include <cutter/config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-ui.h>
#include <cutter/cut-context.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-verbose-level.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_UI_GTK            cut_type_ui_gtk
#define CUT_UI_GTK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_UI_GTK, CutUIGtk))
#define CUT_UI_GTK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_UI_GTK, CutUIGtkClass))
#define CUT_IS_UI_GTK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_UI_GTK))
#define CUT_IS_UI_GTK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_UI_GTK))
#define CUT_UI_GTK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_UI_GTK, CutUIGtkClass))

typedef struct _CutUIGtk CutUIGtk;
typedef struct _CutUIGtkClass CutUIGtkClass;

struct _CutUIGtk
{
    CutUI     object;

    GtkWidget     *window;
    GtkTextBuffer *text_buffer;
    GtkWidget     *text_view;
    GtkProgressBar *progress_bar;
    GtkTreeView   *tree_view;
    GtkTreeStore  *logs;
    GtkStatusbar  *statusbar;

    CutTestSuite  *test_suite;
    CutContext    *context;

    GMutex        *mutex;

    gboolean       running;
    guint          n_tests;
    guint          n_completed_tests;

    CutTestResultStatus status;

    guint          update_pulse_id;
};

struct _CutUIGtkClass
{
    CutUIClass parent_class;
};

enum
{
    PROP_0
};

enum
{
    COLUMN_COLOR,
    COLUMN_TEST_STATUS,
    COLUMN_PROGRESS_VALUE,
    COLUMN_PROGRESS_TEXT,
    COLUMN_PROGRESS_PULSE,
    COLUMN_PROGRESS_VISIBLE,
    COLUMN_NAME,
    COLUMN_DESCRIPTION,
    N_COLUMN
};

static GType cut_type_ui_gtk = 0;
static CutUIClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);
static gboolean run        (CutUI    *ui,
                            CutTestSuite *test_suite,
                            CutContext   *context);

static void
class_init (CutUIClass *klass)
{
    GObjectClass *gobject_class;
    CutUIClass *ui_class;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    ui_class  = CUT_UI_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    ui_class->run           = run;
}

static void
setup_progress_bar (GtkBox *box, CutUIGtk *ui)
{
    GtkWidget *progress_bar;

    progress_bar = gtk_progress_bar_new();
    gtk_box_pack_start(box, progress_bar, FALSE, TRUE, 0);

    ui->progress_bar = GTK_PROGRESS_BAR(progress_bar);
    gtk_progress_bar_set_pulse_step(ui->progress_bar, 0.01);
}

static void
setup_tree_view_columns (GtkTreeView *tree_view)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_progress_new();
    column =
        gtk_tree_view_column_new_with_attributes("Status", renderer,
                                                 "value", COLUMN_PROGRESS_VALUE,
                                                 "text", COLUMN_PROGRESS_TEXT,
                                                 "pulse", COLUMN_PROGRESS_PULSE,
                                                 "visible", COLUMN_PROGRESS_VISIBLE,
                                                 NULL);
    gtk_tree_view_append_column(tree_view, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name", renderer,
                                                      "text", COLUMN_NAME,
                                                      "background", COLUMN_COLOR,
                                                      NULL);
    gtk_tree_view_column_set_sort_column_id(column, COLUMN_NAME);
    gtk_tree_view_append_column(tree_view, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Description", renderer,
                                                      "text", COLUMN_DESCRIPTION,
                                                      "background", COLUMN_COLOR,
                                                      NULL);
    gtk_tree_view_append_column(tree_view, column);
}

static void
setup_tree_view (GtkBox *box, CutUIGtk *ui)
{
    GtkWidget *tree_view, *scrolled_window;
    GtkTreeStore *tree_store;

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(box, scrolled_window, TRUE, TRUE, 0);

    tree_store = gtk_tree_store_new(N_COLUMN,
                                    G_TYPE_STRING,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_STRING,
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
setup_text_view (GtkBox *box, CutUIGtk *ui)
{
    GtkWidget *text_view, *scrolled_window;

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(box, scrolled_window, FALSE, TRUE, 0);

    text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    ui->text_view = text_view;
    ui->text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
}

static void
setup_statusbar (GtkBox *box, CutUIGtk *ui)
{
    GtkWidget *statusbar;

    statusbar = gtk_statusbar_new();
    gtk_box_pack_start(box, statusbar, FALSE, FALSE, 0);
    ui->statusbar = GTK_STATUSBAR(statusbar);
}

static gboolean
cb_destroy (GtkWidget *widget, gpointer data)
{
    CutUIGtk *ui = data;

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
setup_window (CutUIGtk *ui)
{
    GtkWidget *window, *vbox;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    g_signal_connect(G_OBJECT(window), "destroy",
                     G_CALLBACK(cb_destroy), ui);
    g_signal_connect(G_OBJECT(window), "key-press-event",
                     G_CALLBACK(cb_key_press_event), NULL);

    ui->window = window;

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    setup_progress_bar(GTK_BOX(vbox), ui);
    setup_tree_view(GTK_BOX(vbox), ui);
    setup_text_view(GTK_BOX(vbox), ui);
    setup_statusbar(GTK_BOX(vbox), ui);
}

static void
init (CutUIGtk *ui)
{
    ui->test_suite = NULL;
    ui->context = NULL;
    ui->n_tests = 0;
    ui->n_completed_tests = 0;
    ui->status = CUT_TEST_RESULT_SUCCESS;
    ui->update_pulse_id = 0;

    ui->mutex = g_mutex_new();

    setup_window(ui);
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutUIGtkClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutUIGtk),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_ui_gtk = g_type_module_register_type(type_module,
                                                  CUT_TYPE_UI,
                                                  "CutUIGtk",
                                                  &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_ui_gtk)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_ui_gtk));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_UI_GTK, first_property, var_args);
}

G_MODULE_EXPORT gchar *
CUT_MODULE_IMPL_GET_LOG_DOMAIN (void)
{
    return g_strdup(G_LOG_DOMAIN);
}

static void
dispose (GObject *object)
{
    CutUIGtk *ui = CUT_UI_GTK(object);

    if (ui->update_pulse_id) {
        g_source_remove(ui->update_pulse_id);
        ui->update_pulse_id = 0;
    }

    if (ui->window) {
        gtk_widget_destroy(ui->window);
        ui->window = NULL;
    }

    if (ui->test_suite) {
        g_object_unref(ui->test_suite);
        ui->test_suite = NULL;
    }

    if (ui->context) {
        g_object_unref(ui->context);
        ui->context = NULL;
    }

    if (ui->mutex) {
        g_mutex_free(ui->mutex);
        ui->mutex = NULL;
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
status_to_color (CutTestResultStatus status)
{
    const gchar *color = "white";

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        color = "light green";
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        color = "light blue";
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
    }

    return color;
}

static const gchar *
status_to_name (CutTestResultStatus status)
{
    const gchar *name = "?";

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        name = ".";
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        name = "N";
        break;
      case CUT_TEST_RESULT_PENDING:
        name = "P";
        break;
      case CUT_TEST_RESULT_FAILURE:
        name = "F";
        break;
      case CUT_TEST_RESULT_ERROR:
        name = "E";
        break;
    }

    return name;
}

static void
update_status (CutUIGtk *ui, CutTestResultStatus status)
{
    g_mutex_lock(ui->mutex);
    if (ui->status < status)
        ui->status = status;
    g_mutex_unlock(ui->mutex);
}

static void
update_progress_color (GtkProgressBar *bar, CutTestResultStatus status)
{
    GtkStyle *style;

    style = gtk_style_new();
    gdk_color_parse(status_to_color(status), &(style->bg[GTK_STATE_PRELIGHT]));
    gtk_widget_set_style(GTK_WIDGET(bar), style);
    g_object_unref(style);
}

static void
print_log (CutUIGtk *ui, gchar const *format, ...)
{
    gchar *message;
    va_list args;
    GtkTextIter iter;

    return;
    va_start(args, format);
    message = g_strdup_vprintf(format, args);
    gtk_text_buffer_get_end_iter(ui->text_buffer, &iter);
    gtk_text_buffer_insert(ui->text_buffer, &iter, message, -1);
    g_free(message);
    va_end(args);
}

static gboolean
timeout_cb_pulse (gpointer data)
{
    CutUIGtk *ui = data;
    GtkProgressBar *bar;
    gboolean running;
    guint n_tests, n_completed_tests;

    running = ui->running;
    n_tests = ui->n_tests;
    n_completed_tests = ui->n_completed_tests;
    bar = ui->progress_bar;

    update_progress_color(bar, ui->status);

    if (n_tests > 0) {
        gdouble fraction;
        gchar *text;

        fraction = n_completed_tests / (gdouble)n_tests;
        gtk_progress_bar_set_fraction(ui->progress_bar, fraction);

        text = g_strdup_printf("%u/%u (%u%%): %.1fs",
                               n_completed_tests, n_tests,
                               (guint)(fraction * 100),
                               cut_test_get_elapsed(CUT_TEST(ui->test_suite)));
        gtk_progress_bar_set_text(bar, text);
        g_free(text);
    } else {
        gtk_progress_bar_pulse(bar);
    }

    if (!running)
        ui->update_pulse_id = 0;
    return running;
}

static void
cb_ready_test_suite (CutContext *context, CutTestSuite *test_suite,
                     guint n_test_cases, guint n_tests, CutUIGtk *ui)
{
    ui->running = TRUE;
    ui->n_tests = n_tests;
    ui->update_pulse_id = g_timeout_add(10, timeout_cb_pulse, ui);
}

typedef struct _TestCaseRowInfo
{
    CutUIGtk *ui;
    CutTestCase *test_case;
    gchar *path;
    guint n_tests;
    guint n_completed_tests;
} TestCaseRowInfo;

typedef struct TestRowInfo
{
    TestCaseRowInfo *test_case_row_info;
    CutTest *test;
    gchar *path;
    gint pulse;
    guint update_pulse_id;
    CutTestResultStatus status;
} TestRowInfo;

static gboolean
idle_cb_free_test_case_row_info (gpointer data)
{
    TestCaseRowInfo *info = data;

    g_object_unref(info->ui);
    g_object_unref(info->test_case);
    g_free(info->path);

    g_free(info);

    return FALSE;
}

static gboolean
idle_cb_free_test_row_info (gpointer data)
{
    TestRowInfo *info = data;
    CutUIGtk *ui;
    GtkTreeIter iter;

    if (info->update_pulse_id)
        g_source_remove(info->update_pulse_id);

    ui = info->test_case_row_info->ui;

    g_mutex_lock(ui->mutex);
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &iter, info->path)) {
        gtk_tree_store_set(ui->logs, &iter,
                           COLUMN_PROGRESS_VALUE, 100,
                           COLUMN_PROGRESS_PULSE, -1,
                           -1);
    }
    g_mutex_unlock(ui->mutex);

    g_object_unref(info->test);
    g_free(info->path);

    g_free(info);

    return FALSE;
}

static gboolean
idle_cb_append_test_case_row (gpointer data)
{
    TestCaseRowInfo *info = data;
    CutUIGtk *ui;
    CutTestCase *test_case;
    GtkTreeIter iter;

    ui = info->ui;
    test_case = info->test_case;

    g_mutex_lock(ui->mutex);
    gtk_tree_store_append(ui->logs, &iter, NULL);
    gtk_tree_store_set(ui->logs, &iter,
                       COLUMN_NAME,
                       cut_test_get_name(CUT_TEST(test_case)),
                       COLUMN_DESCRIPTION,
                       cut_test_get_description(CUT_TEST(test_case)),
                       COLUMN_PROGRESS_PULSE, -1,
                       COLUMN_PROGRESS_VISIBLE, TRUE,
                       -1);
    info->path =
        gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(ui->logs),
                                            &iter);
    g_mutex_unlock(ui->mutex);

    return FALSE;
}

static gboolean
idle_cb_update_test_case_row (gpointer data)
{
    TestCaseRowInfo *info = data;
    CutUIGtk *ui;
    GtkTreeIter iter;

    ui = info->ui;

    g_mutex_lock(ui->mutex);
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &iter, info->path)) {
        gdouble fraction;
        gint percent;
        gchar *text;

        fraction = info->n_completed_tests / (gdouble)info->n_tests;
        percent = (gint)(fraction * 100);
        text = g_strdup_printf("%d/%d (%d%%)",
                               info->n_completed_tests, info->n_tests, percent);
        gtk_tree_store_set(ui->logs, &iter,
                           COLUMN_PROGRESS_TEXT, text,
                           COLUMN_PROGRESS_VALUE, percent,
                           -1);
        g_free(text);
    }
    g_mutex_unlock(ui->mutex);

    return FALSE;
}

static gboolean
timeout_cb_pulse_test (gpointer data)
{
    TestRowInfo *info = data;
    CutUIGtk *ui;
    GtkTreeIter iter;

    ui = info->test_case_row_info->ui;

    g_mutex_lock(ui->mutex);
    info->pulse++;
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &iter, info->path)) {
        gtk_tree_store_set(ui->logs, &iter,
                           COLUMN_PROGRESS_PULSE, info->pulse,
                           -1);
    }
    g_mutex_unlock(ui->mutex);

    return TRUE;
}

static gboolean
idle_cb_append_test_row (gpointer data)
{
    TestRowInfo *info = data;
    CutUIGtk *ui;
    CutTest *test;
    GtkTreeIter test_case_iter, iter;

    ui = info->test_case_row_info->ui;
    test = info->test;

    g_mutex_lock(ui->mutex);
    gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                        &test_case_iter,
                                        info->test_case_row_info->path);
    gtk_tree_store_append(ui->logs, &iter, &test_case_iter);
    gtk_tree_store_set(ui->logs, &iter,
                       COLUMN_PROGRESS_TEXT, "",
                       COLUMN_PROGRESS_PULSE, 0,
                       COLUMN_PROGRESS_VISIBLE, TRUE,
                       COLUMN_NAME, cut_test_get_name(test),
                       COLUMN_DESCRIPTION, cut_test_get_description(test),
                       -1);
    info->path =
        gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(ui->logs),
                                            &iter);
    info->update_pulse_id = g_timeout_add(10, timeout_cb_pulse_test, info);
    g_mutex_unlock(ui->mutex);

    return FALSE;
}

static void
update_test_row_progress_color (TestRowInfo *info)
{
    GdkColor color;
    GtkWidget *tree_view;

    tree_view = GTK_WIDGET(info->test_case_row_info->ui->tree_view);

    gdk_color_parse(status_to_color(info->status), &color);
    gtk_widget_modify_bg(tree_view, GTK_STATE_SELECTED, &color);

    gdk_color_parse("black", &color);
    gtk_widget_modify_fg(tree_view, GTK_STATE_SELECTED, &color);
}

static gboolean
idle_cb_update_test_row_status (gpointer data)
{
    TestRowInfo *info = data;
    CutUIGtk *ui;
    GtkTreeIter iter;

    ui = info->test_case_row_info->ui;

    g_mutex_lock(ui->mutex);
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &iter, info->path)) {
        update_test_row_progress_color(info);
        gtk_tree_store_set(ui->logs, &iter,
                           COLUMN_COLOR, status_to_color(info->status),
                           COLUMN_PROGRESS_TEXT, status_to_name(info->status),
                           -1);

        if (info->status != CUT_TEST_RESULT_SUCCESS) {
            GtkTreePath *path;
            path = gtk_tree_model_get_path(GTK_TREE_MODEL(ui->logs), &iter);
            gtk_tree_view_expand_to_path(ui->tree_view, path);
            gtk_tree_view_scroll_to_cell(ui->tree_view, path, NULL,
                                         TRUE, 0, 0.5);
            gtk_tree_path_free(path);
        }
    }
    g_mutex_unlock(ui->mutex);

    return FALSE;
}


typedef struct _TestResultRowInfo
{
    TestRowInfo *test_row_info;
    CutTestResult *result;
} TestResultRowInfo;

static void
append_test_result_row (CutUIGtk *ui, CutTestResult *result,
                        GtkTreeIter *test_row_iter,
                        GtkTreeIter *result_row_iter)
{
    CutTestResultStatus status;
    gchar *filename, *name;
    const gchar *message;
    const gchar *source_directory;
    const gchar *test_name;

    source_directory = cut_context_get_source_directory(ui->context);
    if (source_directory)
        filename = g_build_filename(source_directory,
                                    cut_test_result_get_filename(result),
                                    NULL);
    else
        filename = g_strdup(cut_test_result_get_filename(result));
    status = cut_test_result_get_status(result);
    message = cut_test_result_get_message(result);
    test_name = cut_test_result_get_test_name(result);
    if (!test_name)
        test_name = cut_test_result_get_test_case_name(result);
    if (!test_name)
        test_name = cut_test_result_get_test_suite_name(result);
    name = g_strdup_printf("%s:%d: %s()",
                           filename,
                           cut_test_result_get_line(result),
                           cut_test_result_get_function_name(result));
    g_free(filename);

    gtk_tree_store_append(ui->logs, result_row_iter, test_row_iter);
    gtk_tree_store_set(ui->logs, result_row_iter,
                       COLUMN_PROGRESS_VISIBLE, FALSE,
                       COLUMN_COLOR, status_to_color(status),
                       COLUMN_NAME, name,
                       COLUMN_DESCRIPTION, message,
                       -1);
    g_free(name);
}

static gboolean
idle_cb_append_test_result_row (gpointer data)
{
    TestResultRowInfo *info = data;
    CutTestResult *result;
    CutUIGtk *ui;
    GtkTreeIter test_row_iter;
    gchar *test_row_path;

    ui = info->test_row_info->test_case_row_info->ui;
    result = info->result;
    test_row_path = info->test_row_info->path;

    g_mutex_lock(ui->mutex);
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(ui->logs),
                                            &test_row_iter, test_row_path)) {
        GtkTreePath *path;
        GtkTreeIter iter;

        append_test_result_row(ui, result, &test_row_iter, &iter);

        path = gtk_tree_model_get_path(GTK_TREE_MODEL(ui->logs), &iter);
        gtk_tree_view_expand_to_path(ui->tree_view, path);
        gtk_tree_view_scroll_to_cell(ui->tree_view, path, NULL,
                                     TRUE, 0, 0.5);
        gtk_tree_path_free(path);
    }
    g_mutex_unlock(ui->mutex);

    g_object_unref(result);
    g_free(info);

    return FALSE;
}

static void
cb_success_test (CutTest *test, gpointer data)
{
    TestRowInfo *info = data;

    if (info->status == -1) {
        info->status = CUT_TEST_RESULT_SUCCESS;

        g_idle_add(idle_cb_update_test_row_status, data);
    }
}

static void
cb_failure_test (CutTest *test, CutTestContext *context, CutTestResult *result,
                 gpointer data)
{
    TestRowInfo *info = data;
    TestResultRowInfo *result_row_info;

    info->status = CUT_TEST_RESULT_FAILURE;

    update_status(info->test_case_row_info->ui, CUT_TEST_RESULT_FAILURE);

    g_idle_add(idle_cb_update_test_row_status, data);

    result_row_info = g_new0(TestResultRowInfo, 1);
    result_row_info->test_row_info = info;
    result_row_info->result = g_object_ref(result);
    g_idle_add(idle_cb_append_test_result_row, result_row_info);
}

static void
cb_error_test (CutTest *test, CutTestContext *context, CutTestResult *result,
               gpointer data)
{
    TestRowInfo *info = data;
    TestResultRowInfo *result_row_info;

    info->status = CUT_TEST_RESULT_ERROR;

    update_status(info->test_case_row_info->ui, CUT_TEST_RESULT_ERROR);

    g_idle_add(idle_cb_update_test_row_status, data);

    result_row_info = g_new0(TestResultRowInfo, 1);
    result_row_info->test_row_info = info;
    result_row_info->result = g_object_ref(result);
    g_idle_add(idle_cb_append_test_result_row, result_row_info);
}

static void
cb_pending_test (CutTest *test, CutTestContext *context, CutTestResult *result,
                 gpointer data)
{
    TestRowInfo *info = data;
    TestResultRowInfo *result_row_info;

    info->status = CUT_TEST_RESULT_PENDING;

    update_status(info->test_case_row_info->ui, CUT_TEST_RESULT_PENDING);

    g_idle_add(idle_cb_update_test_row_status, data);

    result_row_info = g_new0(TestResultRowInfo, 1);
    result_row_info->test_row_info = info;
    result_row_info->result = g_object_ref(result);
    g_idle_add(idle_cb_append_test_result_row, result_row_info);
}

static void
cb_notification_test (CutTest *test, CutTestContext *context,
                      CutTestResult *result, gpointer data)
{
    TestRowInfo *info = data;
    TestResultRowInfo *result_row_info;

    info->status = CUT_TEST_RESULT_NOTIFICATION;

    update_status(info->test_case_row_info->ui, CUT_TEST_RESULT_NOTIFICATION);

    g_idle_add(idle_cb_update_test_row_status, data);

    result_row_info = g_new0(TestResultRowInfo, 1);
    result_row_info->test_row_info = info;
    result_row_info->result = g_object_ref(result);
    g_idle_add(idle_cb_append_test_result_row, result_row_info);
}

static void
cb_complete_test (CutTest *test, gpointer data)
{
    TestRowInfo *info = data;
    TestCaseRowInfo *test_case_row_info;

    test_case_row_info = info->test_case_row_info;
    test_case_row_info->ui->n_completed_tests++;
    test_case_row_info->n_completed_tests++;

    g_idle_add(idle_cb_update_test_case_row, info->test_case_row_info);
    g_idle_add(idle_cb_free_test_row_info, data);

#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test,                          \
                                         G_CALLBACK(cb_ ## name ## _test ), \
                                         data)
    DISCONNECT(success);
    DISCONNECT(failure);
    DISCONNECT(error);
    DISCONNECT(pending);
    DISCONNECT(notification);
    DISCONNECT(complete);
#undef DISCONNECT
}

static void
cb_start_test (CutTestCase *test_case, CutTest *test,
               CutTestContext *test_context, gpointer data)
{
    TestRowInfo *info;

    info = g_new0(TestRowInfo, 1);
    info->test_case_row_info = data;
    info->test = g_object_ref(test);
    info->path = NULL;
    info->status = -1;
    info->pulse = 0;
    info->update_pulse_id = 0;

    g_idle_add(idle_cb_append_test_row, info);

#define CONNECT(name) \
    g_signal_connect(test, #name, G_CALLBACK(cb_ ## name ## _test), info)

    CONNECT(success);
    CONNECT(failure);
    CONNECT(error);
    CONNECT(pending);
    CONNECT(notification);
    CONNECT(complete);
#undef CONNECT
}

static void
cb_complete_test_case (CutTestCase *test_case, gpointer data)
{
    g_idle_add(idle_cb_free_test_case_row_info, data);
    g_signal_handlers_disconnect_by_func(test_case,
                                         G_CALLBACK(cb_start_test),
                                         data);
    g_signal_handlers_disconnect_by_func(test_case,
                                         G_CALLBACK(cb_complete_test_case),
                                         data);
}

static void
cb_ready_test_case (CutContext *context, CutTestCase *test_case, guint n_tests,
                    CutUIGtk *ui)
{
    TestCaseRowInfo *info;

    info = g_new0(TestCaseRowInfo, 1);
    info->ui = g_object_ref(ui);
    info->test_case = g_object_ref(test_case);
    info->path = NULL;
    info->n_tests = n_tests;
    info->n_completed_tests = 0;

    g_idle_add(idle_cb_append_test_case_row, info);

    g_signal_connect(test_case, "start-test",
                     G_CALLBACK(cb_start_test), info);
    g_signal_connect(test_case, "complete",
                     G_CALLBACK(cb_complete_test_case), info);
}

static void
print_summary (CutUIGtk *ui, CutContext *context,
               gboolean crashed)
{
    guint n_tests, n_assertions, n_failures, n_errors;
    guint n_pendings, n_notifications;

    n_tests = cut_context_get_n_tests(context);
    n_assertions = cut_context_get_n_assertions(context);
    n_failures = cut_context_get_n_failures(context);
    n_errors = cut_context_get_n_errors(context);
    n_pendings = cut_context_get_n_pendings(context);
    n_notifications = cut_context_get_n_notifications(context);

    if (crashed) {
    } else {
        CutTestResultStatus status;
        if (n_errors > 0) {
            status = CUT_TEST_RESULT_ERROR;
        } else if (n_failures > 0) {
            status = CUT_TEST_RESULT_FAILURE;
        } else if (n_pendings > 0) {
            status = CUT_TEST_RESULT_PENDING;
        } else if (n_notifications > 0) {
            status = CUT_TEST_RESULT_NOTIFICATION;
        } else {
            status = CUT_TEST_RESULT_SUCCESS;
        }
    }
}

static void
cb_complete_test_suite (CutContext *context, CutTestSuite *test_suite,
                        CutUIGtk *ui)
{
    gboolean crashed;

    crashed = cut_context_is_crashed(context);
    if (crashed) {
        const gchar *stack_trace;

        stack_trace = cut_context_get_stack_trace(context);
        if (stack_trace)
            print_log(ui, "%s\n", stack_trace);
    }

    print_log(ui, "Finished in %f seconds",
              cut_test_get_elapsed(CUT_TEST(test_suite)));

    print_summary(ui, context, crashed);

    ui->running = FALSE;
}

static void
cb_crashed (CutContext *context, const gchar *stack_trace,
            CutUIGtk *ui)
{
}

static void
connect_to_context (CutUIGtk *ui, CutContext *context)
{
#define CONNECT(name) \
    g_signal_connect(context, #name, G_CALLBACK(cb_ ## name), ui)

    CONNECT(ready_test_suite);
    CONNECT(ready_test_case);

    CONNECT(complete_test_suite);

    CONNECT(crashed);
#undef CONNECT
}

static void
disconnect_from_context (CutUIGtk *ui, CutContext *context)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(context,                       \
                                         G_CALLBACK(cb_ ## name),       \
                                         ui)

    DISCONNECT(ready_test_suite);
    DISCONNECT(ready_test_case);

    DISCONNECT(complete_test_suite);

    DISCONNECT(crashed);
#undef DISCONNECT
}

static gpointer
run_test_thread_func (gpointer data)
{
    CutUIGtk *ui = data;

    g_print("run\n");

    ui->n_tests = 0;
    ui->n_completed_tests = 0;
    ui->status = CUT_TEST_RESULT_SUCCESS;
    cut_test_suite_run(ui->test_suite, ui->context);
    disconnect_from_context(ui, ui->context);

    g_print("finished\n");

    return NULL;
}

static gboolean
run_test_source_func (gpointer data)
{
    CutUIGtk *ui = data;

    connect_to_context(ui, ui->context);
    g_thread_create(run_test_thread_func, ui, TRUE, NULL);

    return FALSE;
}

static gboolean
run (CutUI *ui, CutTestSuite *test_suite, CutContext *context)
{
    CutUIGtk *gtk_ui;

    gtk_ui = CUT_UI_GTK(ui);
    gtk_ui->test_suite = g_object_ref(test_suite);
    gtk_ui->context = g_object_ref(context);
    gtk_widget_show_all(gtk_ui->window);
    g_idle_add(run_test_source_func, gtk_ui);
    gtk_main();

    return gtk_ui->status <= CUT_TEST_RESULT_NOTIFICATION;
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
