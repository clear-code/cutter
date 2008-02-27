/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#include <cutter/cut-module-impl.h>
#include <cutter/cut-ui.h>
#include <cutter/cut-runner.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-suite.h>
#include <cutter/cut-test-context.h>
#include <cutter/cut-verbose-level.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_UI_CONSOLE            cut_type_ui_console
#define CUT_UI_CONSOLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_UI_CONSOLE, CutUIConsole))
#define CUT_UI_CONSOLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_UI_CONSOLE, CutUIConsoleClass))
#define CUT_IS_UI_CONSOLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_UI_CONSOLE))
#define CUT_IS_UI_CONSOLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_UI_CONSOLE))
#define CUT_UI_CONSOLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_UI_CONSOLE, CutUIConsoleClass))

#define RED_COLOR "\033[01;31m"
#define RED_BACK_COLOR "\033[41m"
#define GREEN_COLOR "\033[01;32m"
#define GREEN_BACK_COLOR "\033[01;42m"
#define YELLOW_COLOR "\033[01;33m"
#define BLUE_COLOR "\033[01;34m"
#define MAGENTA_COLOR "\033[01;35m"
#define CYAN_COLOR "\033[01;36m"
#define WHITE_COLOR "\033[01;37m"
#define NORMAL_COLOR "\033[00m"

#define CRASH_COLOR RED_BACK_COLOR WHITE_COLOR

typedef struct _CutUIConsole CutUIConsole;
typedef struct _CutUIConsoleClass CutUIConsoleClass;

struct _CutUIConsole
{
    CutUI     object;
    gchar        *name;
    gboolean      use_color;
    CutVerboseLevel verbose_level;
};

struct _CutUIConsoleClass
{
    CutUIClass parent_class;
};

enum
{
    PROP_0,
    PROP_USE_COLOR,
    PROP_VERBOSE_LEVEL
};

static GType cut_type_ui_console = 0;
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
                            CutRunner   *runner);

static void
class_init (CutUIClass *klass)
{
    GObjectClass *gobject_class;
    CutUIClass *ui_class;
    GParamSpec *spec;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    ui_class  = CUT_UI_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    ui_class->run = run;

    spec = g_param_spec_boolean("use-color",
                                "Use color",
                                "Whether use color",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_USE_COLOR, spec);

    spec = g_param_spec_enum("verbose-level",
                             "Verbose Level",
                             "The number of representing verbosity level",
                             CUT_TYPE_VERBOSE_LEVEL,
                             CUT_VERBOSE_LEVEL_NORMAL,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VERBOSE_LEVEL, spec);
}

static void
init (CutUIConsole *console)
{
    console->use_color = FALSE;
    console->verbose_level = CUT_VERBOSE_LEVEL_NORMAL;
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutUIConsoleClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutUIConsole),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_ui_console = g_type_module_register_type(type_module,
                                                          CUT_TYPE_UI,
                                                          "CutUIConsole",
                                                          &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_ui_console)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_ui_console));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_UI_CONSOLE, first_property, var_args);
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
    CutUIConsole *console = CUT_UI_CONSOLE(object);

    switch (prop_id) {
      case PROP_USE_COLOR:
        console->use_color = g_value_get_boolean(value);
        break;
      case PROP_VERBOSE_LEVEL:
        console->verbose_level = g_value_get_enum(value);
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
    CutUIConsole *console = CUT_UI_CONSOLE(object);

    switch (prop_id) {
      case PROP_USE_COLOR:
        g_value_set_boolean(value, console->use_color);
        break;
      case PROP_VERBOSE_LEVEL:
        g_value_set_enum(value, console->verbose_level);
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
        color = YELLOW_COLOR;
        break;
      case CUT_TEST_RESULT_PENDING:
        color = MAGENTA_COLOR;
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
print_with_colorv (CutUIConsole *console, const gchar *color,
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
print_with_color (CutUIConsole *console, const gchar *color,
                  gchar const *format, ...)
{
    va_list args;

    va_start(args, format);
    print_with_colorv(console, color, format, args);
    va_end(args);
}

static void
print_for_status (CutUIConsole *console, CutTestResultStatus status,
                  gchar const *format, ...)
{
    va_list args;

    va_start(args, format);
    print_with_colorv(console, status_to_color(status), format, args);
    va_end(args);
}

static void
cb_start_test_suite (CutRunner *runner, CutTestSuite *test_suite,
                     CutUIConsole *console)
{
}

static void
cb_start_test_case (CutRunner *runner, CutTestCase *test_case,
                    CutUIConsole *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    print_with_color(console, GREEN_BACK_COLOR,
                     "%s:", cut_test_get_name(CUT_TEST(test_case)));
    g_print("\n");
}

static void
cb_start_test (CutRunner *runner, CutTest *test, CutTestContext *test_context,
               CutUIConsole *console)
{
    GString *tab_stop;
    const gchar *name;
    gint name_length;
    const gchar *description;

    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
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
cb_success (CutRunner *runner, CutTest *test, CutUIConsole *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    if (!CUT_IS_TEST_CONTAINER(test)) {
        print_for_status(console, CUT_TEST_RESULT_SUCCESS, ".");
        fflush(stdout);
    }
}

static void
cb_failure (CutRunner      *runner,
            CutTest        *test,
            CutTestContext *test_context,
            CutTestResult  *result,
            CutUIConsole   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_FAILURE, "F");
    fflush(stdout);
}

static void
cb_error (CutRunner      *runner,
          CutTest        *test,
          CutTestContext *test_context,
          CutTestResult  *result,
          CutUIConsole   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_ERROR, "E");
    fflush(stdout);
}

static void
cb_pending (CutRunner      *runner,
            CutTest        *test,
            CutTestContext *test_context,
            CutTestResult  *result,
            CutUIConsole   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_PENDING, "P");
    fflush(stdout);
}

static void
cb_notification (CutRunner      *runner,
                 CutTest        *test,
                 CutTestContext *test_context,
                 CutTestResult  *result,
                 CutUIConsole   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_NOTIFICATION, "N");
    fflush(stdout);
}

static void
cb_complete_test (CutRunner *runner, CutTest *test,
                  CutTestContext *test_context, CutUIConsole *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    g_print(": (%f)\n", cut_test_get_elapsed(test));
    fflush(stdout);
}

static void
cb_complete_test_case (CutRunner *runner, CutTestCase *test_case,
                       CutUIConsole *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;
}

typedef struct _ConsoleAndStatus {
    CutUIConsole *console;
    CutTestResultStatus status;
} ConsoleAndStatus;

static void
print_each_metadata (gpointer key, gpointer value, gpointer data)
{
    ConsoleAndStatus *info = (ConsoleAndStatus*)data;
    g_print("\n");
    print_with_color(info->console, status_to_color(info->status),
                     "  %s: %s",
                     (const gchar*)key, (const gchar*)value,
                     NULL);
}

static void
print_test_metadata (CutUIConsole *console, CutTestResultStatus status,
                     CutTest *test)
{
    ConsoleAndStatus info;
    info.console = console;
    info.status = status;
    g_hash_table_foreach((GHashTable*)cut_test_get_all_metadata(test),
                         (GHFunc)print_each_metadata, &info);
}

static void
print_results (CutUIConsole *console, CutRunner *runner)
{
    gint i;
    const GList *node;
    CutUI *ui;

    ui = CUT_UI(console);

    i = 1;
    for (node = cut_runner_get_results(runner);
         node;
         node = g_list_next(node)) {
        CutTestResult *result = node->data;
        CutTestResultStatus status;
        CutTest *test;
        gchar *filename;
        const gchar *message;
        const gchar *source_directory;
        const gchar *name;

        status = cut_test_result_get_status(result);
        if (status == CUT_TEST_RESULT_SUCCESS)
            continue; 

        source_directory = cut_runner_get_source_directory(runner);
        if (source_directory)
            filename = g_build_filename(source_directory,
                                        cut_test_result_get_filename(result),
                                        NULL);
        else
            filename = g_strdup(cut_test_result_get_filename(result));

        message = cut_test_result_get_message(result);
        name = cut_test_result_get_test_name(result);
        if (!name)
            name = cut_test_result_get_test_case_name(result);
        if (!name)
            name = cut_test_result_get_test_suite_name(result);

        g_print("\n%d) ", i);
        print_for_status(console, status, "%s: %s",
                         status_to_name(status), name);

        test = cut_test_result_get_test(result);
        if (test) 
            print_test_metadata(console, status, test);

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
print_summary (CutUIConsole *console, CutRunner *runner,
               gboolean crashed)
{
    guint n_tests, n_assertions, n_failures, n_errors;
    guint n_pendings, n_notifications;
    const gchar *color;

    n_tests = cut_runner_get_n_tests(runner);
    n_assertions = cut_runner_get_n_assertions(runner);
    n_failures = cut_runner_get_n_failures(runner);
    n_errors = cut_runner_get_n_errors(runner);
    n_pendings = cut_runner_get_n_pendings(runner);
    n_notifications = cut_runner_get_n_notifications(runner);

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
cb_complete_test_suite (CutRunner *runner, CutTestSuite *test_suite,
                        CutUIConsole *console)
{
    gboolean crashed;
    CutVerboseLevel verbose_level;

    verbose_level = console->verbose_level;
    if (verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;

    if (verbose_level == CUT_VERBOSE_LEVEL_NORMAL)
        g_print("\n");

    crashed = cut_runner_is_crashed(runner);
    if (crashed) {
        const gchar *stack_trace;
        print_with_color(console, CRASH_COLOR, "CRASH!!!");
        g_print("\n");

        stack_trace = cut_runner_get_stack_trace(runner);
        if (stack_trace)
            g_print("%s\n", stack_trace);
    }

    print_results(console, runner);

    g_print("\n");
    g_print("Finished in %f seconds",
            cut_test_get_elapsed(CUT_TEST(test_suite)));
    g_print("\n\n");

    print_summary(console, runner, crashed);
}

static void
cb_crashed (CutRunner *runner, const gchar *stack_trace,
            CutUIConsole *console)
{
    print_with_color(console, CRASH_COLOR, "!");
    fflush(stdout);
}

static void
connect_to_runner (CutUIConsole *console, CutRunner *runner)
{
#define CONNECT(name) \
    g_signal_connect(runner, #name, G_CALLBACK(cb_ ## name), console)

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
disconnect_from_runner (CutUIConsole *console, CutRunner *runner)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(runner,                       \
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
run (CutUI *ui, CutRunner *runner)
{
    CutUIConsole *console = CUT_UI_CONSOLE(ui);
    gboolean success;

    connect_to_runner(console, runner);
    success = cut_runner_run(runner);
    disconnect_from_runner(console, runner);

    return success;
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
