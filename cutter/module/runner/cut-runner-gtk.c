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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>
#include <gtk/gtk.h>

#include "cut-module-impl.h"
#include "cut-runner.h"
#include "cut-context.h"
#include "cut-test.h"
#include "cut-test-case.h"
#include "cut-verbose-level.h"
#include "cut-enum-types.h"

#define CUT_TYPE_RUNNER_GTK            cut_type_runner_gtk
#define CUT_RUNNER_GTK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_RUNNER_GTK, CutRunnerGtk))
#define CUT_RUNNER_GTK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_RUNNER_GTK, CutRunnerGtkClass))
#define CUT_IS_RUNNER_GTK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_RUNNER_GTK))
#define CUT_IS_RUNNER_GTK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_RUNNER_GTK))
#define CUT_RUNNER_GTK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_RUNNER_GTK, CutRunnerGtkClass))

typedef struct _CutRunnerGtk CutRunnerGtk;
typedef struct _CutRunnerGtkClass CutRunnerGtkClass;

struct _CutRunnerGtk
{
    CutRunner     object;

    GtkWidget     *window;
    GtkTextBuffer *text_buffer;
};

struct _CutRunnerGtkClass
{
    CutRunnerClass parent_class;
};

enum
{
    PROP_0
};

static GType cut_type_runner_gtk = 0;
static CutRunnerClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);
static gboolean run        (CutRunner    *runner,
                            CutTestSuite *test_suite,
                            CutContext   *context);

static void
class_init (CutRunnerClass *klass)
{
    GObjectClass *gobject_class;
    CutRunnerClass *runner_class;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    runner_class  = CUT_RUNNER_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    runner_class->run           = run;
}

static GtkWidget *
create_window (CutRunnerGtk *runner)
{
    GtkWidget *window, *scrolled_window;
    GtkWidget *text_view;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(G_OBJECT(window), "destroy",
                     G_CALLBACK (gtk_main_quit), NULL);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);
    gtk_widget_show(scrolled_window);

    text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_widget_show(text_view);

    runner->text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    gtk_widget_show(window);

    return window;
}

static void
init (CutRunnerGtk *runner)
{
    runner->window = create_window(runner);
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutRunnerGtkClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutRunnerGtk),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_runner_gtk = g_type_module_register_type(type_module,
                                                      CUT_TYPE_RUNNER,
                                                      "CutRunnerGtk",
                                                      &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_runner_gtk)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_runner_gtk));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_RUNNER_GTK, first_property, var_args);
}

G_MODULE_EXPORT gchar *
CUT_MODULE_IMPL_GET_LOG_DOMAIN (void)
{
    return g_strdup(G_LOG_DOMAIN);
}

static void
dispose (GObject *object)
{
    CutRunnerGtk *gtk = CUT_RUNNER_GTK(object);

    if (gtk->window) {
        gtk_widget_destroy(gtk->window);
        gtk->window = NULL;
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

static void
print_for_status (CutRunnerGtk *runner, CutTestResultStatus status,
                  gchar const *format, ...)
{
    gchar *message;
    va_list args;
    GtkTextIter iter;

    va_start(args, format);
    message = g_strdup_vprintf(format, args);
    gtk_text_buffer_get_end_iter(runner->text_buffer, &iter);
    gtk_text_buffer_insert(runner->text_buffer, &iter, message, -1);
    g_free(message);
    va_end(args);
}

static void
print_log (CutRunnerGtk *runner, gchar const *format, ...)
{
    gchar *message;
    va_list args;
    GtkTextIter iter;

    va_start(args, format);
    message = g_strdup_vprintf(format, args);
    gtk_text_buffer_get_end_iter(runner->text_buffer, &iter);
    gtk_text_buffer_insert(runner->text_buffer, &iter, message, -1);
    g_free(message);
    va_end(args);
}

static void
cb_start_test_suite (CutContext *context, CutTestSuite *test_suite,
                     CutRunnerGtk *runner)
{
}

static void
cb_start_test_case (CutContext *context, CutTestCase *test_case,
                    CutRunnerGtk *runner)
{
}

static void
cb_start_test (CutContext *context, CutTest *test, CutTestContext *test_context,
               CutRunnerGtk *runner)
{
    GString *tab_stop;
    const gchar *name;
    gint name_length;
    const gchar *description;

    description = cut_test_get_description(test);
    if (description)
        print_log(runner, "  %s\n", description);

    name = cut_test_get_name(test);
    name_length = strlen(name) + 2;
    tab_stop = g_string_new("");
    while (name_length < (8 * 7 - 1)) {
        g_string_append_c(tab_stop, '\t');
        name_length += 8;
    }
    print_log(runner, "  %s:%s", name, tab_stop->str);
    g_string_free(tab_stop, TRUE);
}

static void
cb_success (CutContext *context, CutTest *test, CutRunnerGtk *runner)
{
    if (!CUT_IS_TEST_CONTAINER(test)) {
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(runner->text_buffer, &iter);
        gtk_text_buffer_insert(runner->text_buffer, &iter, ".", -1);
    }
}

static void
cb_failure (CutContext       *context,
            CutTest          *test,
            CutTestContext   *test_context,
            CutTestResult    *result,
            CutRunnerGtk *runner)
{
    print_for_status(runner, CUT_TEST_RESULT_FAILURE, "F");
}

static void
cb_error (CutContext       *context,
          CutTest          *test,
          CutTestContext   *test_context,
          CutTestResult    *result,
          CutRunnerGtk *runner)
{
    print_for_status(runner, CUT_TEST_RESULT_ERROR, "E");
}

static void
cb_pending (CutContext       *context,
            CutTest          *test,
            CutTestContext   *test_context,
            CutTestResult    *result,
            CutRunnerGtk *runner)
{
    print_for_status(runner, CUT_TEST_RESULT_PENDING, "P");
}

static void
cb_notification (CutContext       *context,
                 CutTest          *test,
                 CutTestContext   *test_context,
                 CutTestResult    *result,
                 CutRunnerGtk *runner)
{
    print_for_status(runner, CUT_TEST_RESULT_NOTIFICATION, "N");
}

static void
cb_complete_test (CutContext *context, CutTest *test,
                  CutTestContext *test_context, CutRunnerGtk *runner)
{
    print_log(runner, ": (%f)\n", cut_test_get_elapsed(test));
}

static void
cb_complete_test_case (CutContext *context, CutTestCase *test_case,
                       CutRunnerGtk *runner)
{
}

static void
print_results (CutRunnerGtk *runner, CutContext *context)
{
    gint i;
    const GList *node;

    i = 1;
    for (node = cut_context_get_results(context);
         node;
         node = g_list_next(node)) {
        CutTestResult *result = node->data;
        CutTestResultStatus status;
        gchar *filename;
        const gchar *message;
        const gchar *source_directory;
        const gchar *name;

        source_directory = cut_context_get_source_directory(context);
        if (source_directory)
            filename = g_build_filename(source_directory,
                                        cut_test_result_get_filename(result),
                                        NULL);
        else
            filename = g_strdup(cut_test_result_get_filename(result));

        status = cut_test_result_get_status(result);
        message = cut_test_result_get_message(result);
        name = cut_test_result_get_test_name(result);
        if (!name)
            name = cut_test_result_get_test_case_name(result);
        if (!name)
            name = cut_test_result_get_test_suite_name(result);


        print_log(runner, "\n%s:%d: %s()\n",
                  filename,
                  cut_test_result_get_line(result),
                  cut_test_result_get_function_name(result));
        i++;
        g_free(filename);
    }
}

static void
print_summary (CutRunnerGtk *runner, CutContext *context,
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
                        CutRunnerGtk *runner)
{
    gboolean crashed;

    crashed = cut_context_is_crashed(context);
    if (crashed) {
        const gchar *stack_trace;

        stack_trace = cut_context_get_stack_trace(context);
        if (stack_trace)
            print_log(runner, "%s\n", stack_trace);
    }

    print_results(runner, context);

    print_log(runner, "Finished in %f seconds",
              cut_test_get_elapsed(CUT_TEST(test_suite)));

    print_summary(runner, context, crashed);
}

static void
cb_crashed (CutContext *context, const gchar *stack_trace,
            CutRunnerGtk *runner)
{
}

static void
connect_to_context (CutRunnerGtk *runner, CutContext *context)
{
#define CONNECT(name) \
    g_signal_connect(context, #name, G_CALLBACK(cb_ ## name), runner)

    CONNECT(start_test_suite);
    CONNECT(start_test_case);
    CONNECT(start_test);

    CONNECT(success);
    CONNECT(failure);
    CONNECT(error);
    CONNECT(pending);
    CONNECT(notification);

    CONNECT(complete_test);
    CONNECT(complete_test_case);
    CONNECT(complete_test_suite);

    CONNECT(crashed);

#undef CONNECT
}

static void
disconnect_from_context (CutRunnerGtk *runner, CutContext *context)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(context,                       \
                                         G_CALLBACK(cb_ ## name),       \
                                         runner)

    DISCONNECT(start_test_suite);
    DISCONNECT(start_test_case);
    DISCONNECT(start_test);

    DISCONNECT(success);
    DISCONNECT(failure);
    DISCONNECT(error);
    DISCONNECT(pending);
    DISCONNECT(notification);

    DISCONNECT(complete_test);
    DISCONNECT(complete_test_case);
    DISCONNECT(complete_test_suite);

    DISCONNECT(crashed);

#undef DISCONNECT
}

typedef struct _RunTestInfo
{
    CutRunner    *runner;
    CutTestSuite *test_suite;
    CutContext   *context;
} RunTestInfo;

static gpointer
test_run (gpointer data)
{
    gboolean success;
    RunTestInfo *info = data;
    CutRunner *runner;
    CutTestSuite *test_suite;
    CutContext *context;

    runner = info->runner;
    test_suite = info->test_suite;
    context = info->context;

    connect_to_context(CUT_RUNNER_GTK(runner), context);
    success = cut_test_suite_run(test_suite, context);
    disconnect_from_context(CUT_RUNNER_GTK(runner), context);

    g_object_unref(runner);
    g_object_unref(test_suite);
    g_object_unref(context);
    g_free(info);

    return GINT_TO_POINTER(success);
}

static gboolean
run (CutRunner *runner, CutTestSuite *test_suite, CutContext *context)
{
    CutRunnerGtk *gtk_runner;
    gboolean success;
    RunTestInfo *info;
    GThread *thread;

    info = g_new0(RunTestInfo, 1);
    info->runner = g_object_ref(runner);
    info->test_suite = g_object_ref(test_suite);
    info->context = g_object_ref(context);

    thread = g_thread_create(test_run, info, TRUE, NULL);

    gtk_runner = CUT_RUNNER_GTK(runner);
    gtk_widget_show_all(gtk_runner->window);
    gtk_main();

    success = GPOINTER_TO_INT(g_thread_join(thread));

    return success;
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
