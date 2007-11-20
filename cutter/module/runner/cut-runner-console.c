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

#include <cut-module-impl.h>
#include <cut-runner.h>
#include <cut-context.h>
#include <cut-test.h>
#include <cut-test-suite.h>
#include <cut-test-context.h>

#define CUT_TYPE_RUNNER_CONSOLE            cut_type_runner_console
#define CUT_RUNNER_CONSOLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_RUNNER_CONSOLE, CutRunnerConsole))
#define CUT_RUNNER_CONSOLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_RUNNER_CONSOLE, CutRunnerConsoleClass))
#define CUT_IS_RUNNER_CONSOLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_RUNNER_CONSOLE))
#define CUT_IS_RUNNER_CONSOLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_RUNNER_CONSOLE))
#define CUT_RUNNER_CONSOLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_RUNNER_CONSOLE, CutRunnerConsoleClass))

#define GET_VERBOSE_LEVEL(console) \
    (cut_runner_get_verbose_level(CUT_RUNNER(console)))

#define RED_COLOR "\033[01;31m"
#define RED_BACK_COLOR "\033[41m"
#define GREEN_COLOR "\033[01;32m"
#define GREEN_BACK_COLOR "\033[01;42m"
#define YELLOW_COLOR "\033[01;33m"
#define BLUE_COLOR "\033[01;34m"
#define PURPLE_COLOR "\033[01;35m"
#define CYAN_COLOR "\033[01;36m"
#define WHITE_COLOR "\033[01;37m"
#define NORMAL_COLOR "\033[00m"

#define CRASH_COLOR RED_BACK_COLOR WHITE_COLOR

typedef struct _CutRunnerConsole CutRunnerConsole;
typedef struct _CutRunnerConsoleClass CutRunnerConsoleClass;

struct _CutRunnerConsole
{
    CutRunner     object;
    gchar        *name;
    gboolean      use_color;
};

struct _CutRunnerConsoleClass
{
    CutRunnerClass parent_class;
};

enum
{
    PROP_0,
    PROP_USE_COLOR
};

static GType cut_type_runner_console = 0;
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
    GParamSpec *spec;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    runner_class  = CUT_RUNNER_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    runner_class->run = run;

    spec = g_param_spec_boolean("use-color",
                                "Use color",
                                "Whether use color",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_USE_COLOR, spec);
}

static void
init (CutRunnerConsole *console)
{
    console->use_color = FALSE;
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutRunnerConsoleClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutRunnerConsole),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_runner_console = g_type_module_register_type(type_module,
                                                          CUT_TYPE_RUNNER,
                                                          "CutRunnerConsole",
                                                          &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_runner_console)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_runner_console));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_RUNNER_CONSOLE, first_property, var_args);
}

G_MODULE_EXPORT gchar *
CUT_MODULE_IMPL_GET_LOG_DOMAIN (void)
{
    return g_strdup(G_LOG_DOMAIN);
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutRunnerConsole *console = CUT_RUNNER_CONSOLE(object);

    switch (prop_id) {
      case PROP_USE_COLOR:
        console->use_color = g_value_get_boolean(value);
        break;
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
    CutRunnerConsole *console = CUT_RUNNER_CONSOLE(object);

    switch (prop_id) {
      case PROP_USE_COLOR:
        g_value_set_boolean(value, console->use_color);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static const gchar *
status_to_name(CutTestResultStatus status)
{
    const gchar *name;

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        name = "Success";
        break;
      case CUT_TEST_RESULT_FAILURE:
        name = "Failure";
        break;
      case CUT_TEST_RESULT_ERROR:
        name = "Error";
        break;
      case CUT_TEST_RESULT_PENDING:
        name = "Pending";
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        name = "Notification";
        break;
      default:
        name = "MUST NOT HAPPEN!!!";
        break;
    }

    return name;
}

static const gchar *
status_to_color(CutTestResultStatus status)
{
    const gchar *color;

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        color = GREEN_COLOR;
        break;
      case CUT_TEST_RESULT_FAILURE:
        color = RED_COLOR;
        break;
      case CUT_TEST_RESULT_ERROR:
        color = PURPLE_COLOR;
        break;
      case CUT_TEST_RESULT_PENDING:
        color = YELLOW_COLOR;
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        color = CYAN_COLOR;
        break;
      default:
        color = "";
        break;
    }

    return color;
}

static void
print_with_colorv (CutRunnerConsole *console, const gchar *color,
                   gchar const *format, va_list args)
{
   if (console->use_color) {
        gchar *message;
        message = g_strdup_vprintf(format, args);
        g_print("%s%s%s", color, message, NORMAL_COLOR);
        g_free(message);
    } else {
        g_vprintf(format, args);
    }
}

static void
print_with_color (CutRunnerConsole *console, const gchar *color,
                  gchar const *format, ...)
{
    va_list args;

    va_start(args, format);
    print_with_colorv(console, color, format, args);
    va_end(args);
}

static void
print_for_status (CutRunnerConsole *console, CutTestResultStatus status,
                  gchar const *format, ...)
{
    va_list args;

    va_start(args, format);
    print_with_colorv(console, status_to_color(status), format, args);
    va_end(args);
}

static void
cb_start_test_suite (CutContext *context, CutTestSuite *test_suite,
                     CutRunnerConsole *console)
{
}

static void
cb_start_test_case (CutContext *context, CutTestCase *test_case,
                    CutRunnerConsole *console)
{
    if (GET_VERBOSE_LEVEL(console) < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    print_with_color(console, GREEN_BACK_COLOR,
                     "%s:", cut_test_get_name(CUT_TEST(test_case)));
    g_print("\n");
}

static void
cb_start_test (CutContext *context, CutTest *test, CutTestContext *test_context,
               CutRunnerConsole *console)
{
    GString *tab_stop;
    const gchar *name;
    gint name_length;
    const gchar *description;

    if (GET_VERBOSE_LEVEL(console) < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    description = cut_test_get_description(test);
    if (description)
        g_print("  %s\n", description);

    name = cut_test_get_name(test);
    name_length = strlen(name) + 2;
    tab_stop = g_string_new("");
    while (name_length < (8 * 7 - 1)) {
        g_string_append_c(tab_stop, '\t');
        name_length += 8;
    }
    g_print("  %s:%s", name, tab_stop->str);
    g_string_free(tab_stop, TRUE);
    fflush(stdout);
}

static void
cb_success (CutContext *context, CutTest *test, CutRunnerConsole *console)
{
    if (GET_VERBOSE_LEVEL(console) < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    if (!CUT_IS_TEST_CONTAINER(test)) {
        print_for_status(console, CUT_TEST_RESULT_SUCCESS, ".");
        fflush(stdout);
    }
}

static void
cb_failure (CutContext       *context,
            CutTest          *test,
            CutTestContext   *test_context,
            CutTestResult    *result,
            CutRunnerConsole *console)
{
    if (GET_VERBOSE_LEVEL(console) < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_FAILURE, "F");
    fflush(stdout);
}

static void
cb_error (CutContext       *context,
          CutTest          *test,
          CutTestContext   *test_context,
          CutTestResult    *result,
          CutRunnerConsole *console)
{
    if (GET_VERBOSE_LEVEL(console) < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_ERROR, "E");
    fflush(stdout);
}

static void
cb_pending (CutContext       *context,
            CutTest          *test,
            CutTestContext   *test_context,
            CutTestResult    *result,
            CutRunnerConsole *console)
{
    if (GET_VERBOSE_LEVEL(console) < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_PENDING, "P");
    fflush(stdout);
}

static void
cb_notification (CutContext       *context,
                 CutTest          *test,
                 CutTestContext   *test_context,
                 CutTestResult    *result,
                 CutRunnerConsole *console)
{
    if (GET_VERBOSE_LEVEL(console) < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_NOTIFICATION, "N");
    fflush(stdout);
}

static void
cb_complete_test (CutContext *context, CutTest *test,
                  CutTestContext *test_context, CutRunnerConsole *console)
{
    if (GET_VERBOSE_LEVEL(console) < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    g_print(": (%f)\n", cut_test_get_elapsed(test));
    fflush(stdout);
}

static void
cb_complete_test_case (CutContext *context, CutTestCase *test_case,
                       CutRunnerConsole *console)
{
    if (GET_VERBOSE_LEVEL(console) < CUT_VERBOSE_LEVEL_VERBOSE)
        return;
}

static void
print_results (CutRunnerConsole *console, CutContext *context)
{
    gint i;
    const GList *node;
    CutRunner *runner;

    runner = CUT_RUNNER(console);

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

        source_directory = cut_runner_get_source_directory(runner);
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

        g_print("\n%d) ", i);
        print_for_status(console, status, "%s: %s",
                         status_to_name(status), name);

        if (message) {
            g_print("\n");
            print_for_status(console, status, "%s", message);
        }
        g_print("\n%s:%d: %s()\n",
                filename,
                cut_test_result_get_line(result),
                cut_test_result_get_function_name(result));
        i++;
        g_free(filename);
    }
}

static void
print_summary (CutRunnerConsole *console, CutContext *context,
               gboolean crashed)
{
    guint n_tests, n_assertions, n_failures, n_errors;
    guint n_pendings, n_notifications;
    const gchar *color;

    n_tests = cut_context_get_n_tests(context);
    n_assertions = cut_context_get_n_assertions(context);
    n_failures = cut_context_get_n_failures(context);
    n_errors = cut_context_get_n_errors(context);
    n_pendings = cut_context_get_n_pendings(context);
    n_notifications = cut_context_get_n_notifications(context);

    if (crashed) {
        color = CRASH_COLOR;
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
        color = status_to_color(status);
    }
    print_with_color(console, color,
                     "%d test(s), %d assertion(s), %d failure(s), "
                     "%d error(s), %d pending(s), %d notification(s)",
                     n_tests, n_assertions, n_failures, n_errors,
                     n_pendings, n_notifications);
    g_print("\n");
}

static void
cb_complete_test_suite (CutContext *context, CutTestSuite *test_suite,
                        CutRunnerConsole *console)
{
    gboolean crashed;
    CutVerboseLevel verbose_level;

    verbose_level = GET_VERBOSE_LEVEL(console);
    if (verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;

    if (verbose_level == CUT_VERBOSE_LEVEL_NORMAL)
        g_print("\n");

    crashed = cut_context_is_crashed(context);
    if (crashed) {
        const gchar *stack_trace;
        print_with_color(console, CRASH_COLOR, "CRASH!!!");
        g_print("\n");

        stack_trace = cut_context_get_stack_trace(context);
        if (stack_trace)
            g_print("%s\n", stack_trace);
    }

    print_results(console, context);

    g_print("\n");
    g_print("Finished in %f seconds",
            cut_test_get_elapsed(CUT_TEST(test_suite)));
    g_print("\n\n");

    print_summary(console, context, crashed);
}

static void
cb_crashed (CutContext *context, CutRunnerConsole *console)
{
    print_with_color(console, CRASH_COLOR, "!");
    fflush(stdout);
}

static void
connect_to_context (CutRunnerConsole *console, CutContext *context)
{
#define CONNECT(name) \
    g_signal_connect(context, #name, G_CALLBACK(cb_ ## name), console)

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
disconnect_from_context (CutRunnerConsole *console, CutContext *context)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(context,                       \
                                         G_CALLBACK(cb_ ## name),       \
                                         console)

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

static gboolean
run (CutRunner *runner, CutTestSuite *test_suite, CutContext *context)
{
    CutRunnerConsole *console = CUT_RUNNER_CONSOLE(runner);
    gboolean success;

    connect_to_context(console, context);
    success = cut_test_suite_run(test_suite, context);
    disconnect_from_context(console, context);

    return success;
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
