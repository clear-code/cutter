/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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
#  include <cutter/config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-listener.h>
#include <cutter/cut-ui.h>
#include <cutter/cut-test-runner.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-suite.h>
#include <cutter/cut-test-context.h>
#include <cutter/cut-verbose-level.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_CONSOLE_UI            cut_type_console_ui
#define CUT_CONSOLE_UI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_CONSOLE_UI, CutConsoleUI))
#define CUT_CONSOLE_UI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_CONSOLE_UI, CutConsoleUIClass))
#define CUT_IS_CONSOLE_UI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_CONSOLE_UI))
#define CUT_IS_CONSOLE_UI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_CONSOLE_UI))
#define CUT_CONSOLE_UI_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_CONSOLE_UI, CutConsoleUIClass))

#define RED_COLOR "\033[01;31m"
#define RED_BACK_COLOR "\033[41m"
#define GREEN_COLOR "\033[01;32m"
#define GREEN_BACK_COLOR "\033[01;42m"
#define YELLOW_COLOR "\033[01;33m"
#define BLUE_COLOR "\033[01;34m"
#define BLUE_BACK_COLOR "\033[01;44m"
#define MAGENTA_COLOR "\033[01;35m"
#define CYAN_COLOR "\033[01;36m"
#define WHITE_COLOR "\033[01;37m"
#define NORMAL_COLOR "\033[00m"

#define CRASH_COLOR RED_BACK_COLOR WHITE_COLOR

typedef struct _Error Error;
struct _Error
{
    gchar *name;
    gchar *detail;
};

typedef struct _CutConsoleUI CutConsoleUI;
typedef struct _CutConsoleUIClass CutConsoleUIClass;

struct _CutConsoleUI
{
    GObject       object;
    gchar        *name;
    gboolean      use_color;
    CutVerboseLevel verbose_level;
    GList        *errors;
};

struct _CutConsoleUIClass
{
    GObjectClass parent_class;
};

enum
{
    PROP_0,
    PROP_USE_COLOR,
    PROP_VERBOSE_LEVEL
};

static GType cut_type_console_ui = 0;
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

static void     attach_to_run_context   (CutListener *listener,
                                         CutRunContext   *run_context);
static void     detach_from_run_context (CutListener *listener,
                                         CutRunContext   *run_context);
static gboolean run                     (CutUI       *ui,
                                         CutRunContext   *run_context);

static void
class_init (CutConsoleUIClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

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
init (CutConsoleUI *console)
{
    console->use_color = FALSE;
    console->verbose_level = CUT_VERBOSE_LEVEL_NORMAL;
    console->errors = NULL;
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
            sizeof (CutConsoleUIClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutConsoleUI),
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

    cut_type_console_ui = g_type_module_register_type(type_module,
                                                      G_TYPE_OBJECT,
                                                      "CutConsoleUI",
                                                      &info, 0);

    g_type_module_add_interface(type_module,
                                cut_type_console_ui,
                                CUT_TYPE_UI,
                                &ui_info);

    g_type_module_add_interface(type_module,
                                cut_type_console_ui,
                                CUT_TYPE_LISTENER,
                                &listener_info);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_console_ui)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_console_ui));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_CONSOLE_UI, first_property, var_args);
}

static Error *
error_new (const gchar *name, const gchar *detail)
{
    Error *error;

    error = g_slice_new(Error);
    error->name = g_strdup(name);
    error->detail = g_strdup(detail);

    return error;
}

static void
error_free (Error *error)
{
    if (error->name)
        g_free(error->name);
    if (error->detail)
        g_free(error->detail);
    g_slice_free(Error, error);
}

static void
dispose (GObject *object)
{
    CutConsoleUI *console;

    console = CUT_CONSOLE_UI(object);

    if (console->errors) {
        g_list_foreach(console->errors, (GFunc)error_free, NULL);
        g_list_free(console->errors);
        console->errors = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutConsoleUI *console = CUT_CONSOLE_UI(object);

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
    CutConsoleUI *console = CUT_CONSOLE_UI(object);

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
      case CUT_TEST_RESULT_NOTIFICATION:
        name = "Notification";
        break;
      case CUT_TEST_RESULT_OMISSION:
        name = "Omission";
        break;
      case CUT_TEST_RESULT_PENDING:
        name = "Pending";
        break;
      case CUT_TEST_RESULT_FAILURE:
        name = "Failure";
        break;
      case CUT_TEST_RESULT_ERROR:
        name = "Error";
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
      case CUT_TEST_RESULT_NOTIFICATION:
        color = CYAN_COLOR;
        break;
      case CUT_TEST_RESULT_OMISSION:
        color = BLUE_COLOR;
        break;
      case CUT_TEST_RESULT_PENDING:
        color = MAGENTA_COLOR;
        break;
      case CUT_TEST_RESULT_FAILURE:
        color = RED_COLOR;
        break;
      case CUT_TEST_RESULT_ERROR:
        color = YELLOW_COLOR;
        break;
      default:
        color = "";
        break;
    }

    return color;
}

static void
print_with_colorv (CutConsoleUI *console, const gchar *color,
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
print_with_color (CutConsoleUI *console, const gchar *color,
                  gchar const *format, ...)
{
    va_list args;

    va_start(args, format);
    print_with_colorv(console, color, format, args);
    va_end(args);
}

static void
print_for_status (CutConsoleUI *console, CutTestResultStatus status,
                  gchar const *format, ...)
{
    va_list args;

    va_start(args, format);
    print_with_colorv(console, status_to_color(status), format, args);
    va_end(args);
}

static void
cb_start_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                     CutConsoleUI *console)
{
}

static void
cb_start_test_case (CutRunContext *run_context, CutTestCase *test_case,
                    CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    print_with_color(console, GREEN_BACK_COLOR,
                     "%s:", cut_test_get_name(CUT_TEST(test_case)));
    g_print("\n");
}

static void
cb_start_test_iterator (CutRunContext *run_context,
                        CutTestIterator *test_iterator,
                        CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    g_print("  ");
    print_with_color(console, BLUE_BACK_COLOR,
                     "%s:", cut_test_get_name(CUT_TEST(test_iterator)));
    g_print("\n");
}

static void
print_test_on_start (CutConsoleUI *console, const gchar *name,
                     CutTest *test, const gchar *indent)
{
    GString *tab_stop;
    gint name_length;
    const gchar *description;

    description = cut_test_get_description(test);
    if (description)
        g_print("  %s%s\n", indent, description);

    name_length = strlen(indent) + strlen(name) + 2;
    tab_stop = g_string_new("");
    while (name_length < (8 * 7 - 1)) {
        g_string_append_c(tab_stop, '\t');
        name_length += 8;
    }
    g_print("  %s%s:%s", indent, name, tab_stop->str);
    g_string_free(tab_stop, TRUE);
    fflush(stdout);
}

static void
cb_start_test (CutRunContext *run_context, CutTest *test,
               CutTestContext *test_context, CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    print_test_on_start(console, cut_test_get_name(test), test, "");
}

static void
cb_start_iterated_test (CutRunContext *run_context,
                        CutIteratedTest *iterated_test,
                        CutTestContext *test_context, CutConsoleUI *console)
{
    const gchar *name = NULL;

    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    if (cut_test_context_have_data(test_context)) {
        CutTestData *data;

        data = cut_test_context_get_current_data(test_context);
        name = cut_test_data_get_name(data);
    }

    if (!name)
        name = cut_test_get_name(CUT_TEST(iterated_test));

    print_test_on_start(console, name, CUT_TEST(iterated_test), "  ");
}

static void
cb_success_test (CutRunContext  *run_context,
                 CutTest        *test,
                 CutTestContext *context,
                 CutTestResult  *result,
                 CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_SUCCESS, ".");
    fflush(stdout);
}

static void
cb_notification_test (CutRunContext      *run_context,
                      CutTest        *test,
                      CutTestContext *test_context,
                      CutTestResult  *result,
                      CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_NOTIFICATION, "N");
    fflush(stdout);
}

static void
cb_omission_test (CutRunContext      *run_context,
                  CutTest        *test,
                  CutTestContext *test_context,
                  CutTestResult  *result,
                  CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_OMISSION, "O");
    fflush(stdout);
}

static void
cb_pending_test (CutRunContext      *run_context,
                 CutTest        *test,
                 CutTestContext *test_context,
                 CutTestResult  *result,
                 CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_PENDING, "P");
    fflush(stdout);
}

static void
cb_failure_test (CutRunContext      *run_context,
                 CutTest        *test,
                 CutTestContext *test_context,
                 CutTestResult  *result,
                 CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_FAILURE, "F");
    fflush(stdout);
}

static void
cb_error_test (CutRunContext      *run_context,
               CutTest        *test,
               CutTestContext *test_context,
               CutTestResult  *result,
               CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_ERROR, "E");
    fflush(stdout);
}

static void
cb_complete_test (CutRunContext *run_context, CutTest *test,
                  CutTestContext *test_context, CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    g_print(": (%f)\n", cut_test_get_elapsed(test));
    fflush(stdout);
}

static void
cb_complete_iterated_test (CutRunContext *run_context,
                           CutIteratedTest *iterated_test,
                           CutTestContext *test_context, CutConsoleUI *console)
{
    cb_complete_test(run_context, CUT_TEST(iterated_test),
                     test_context, console);
}

static void
cb_complete_test_iterator (CutRunContext *run_context,
                           CutTestIterator *test_iterator,
                           CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;
}

static void
cb_complete_test_case (CutRunContext *run_context, CutTestCase *test_case,
                       CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;
}

typedef struct _ConsoleAndStatus {
    CutConsoleUI *console;
    CutTestResultStatus status;
} ConsoleAndStatus;

static void
print_each_attribute (gpointer key, gpointer value, gpointer data)
{
    ConsoleAndStatus *info = (ConsoleAndStatus*)data;
    g_print("\n");
    print_with_color(info->console, status_to_color(info->status),
                     "  %s: %s",
                     (const gchar*)key, (const gchar*)value,
                     NULL);
}

static void
print_test_attributes (CutConsoleUI *console, CutTestResultStatus status,
                     CutTest *test)
{
    ConsoleAndStatus info;
    info.console = console;
    info.status = status;
    g_hash_table_foreach(cut_test_get_attributes(test),
                         (GHFunc)print_each_attribute, &info);
}

static void
print_results (CutConsoleUI *console, CutRunContext *run_context)
{
    gint i;
    const GList *node;

    i = 1;
    for (node = console->errors; node; node = g_list_next(node)) {
        Error *error = node->data;

        g_print("\n%d) ", i);
        print_for_status(console, CUT_TEST_RESULT_ERROR,
                         "SystemError: %s: %s", error->name, error->detail);
        g_print("\n");
        i++;
    }

    for (node = cut_run_context_get_results(run_context);
         node;
         node = g_list_next(node)) {
        CutTestResult *result = node->data;
        CutTestResultStatus status;
        CutTest *test;
        gchar *full_filename;
        const gchar *filename;
        const gchar *message;
        const gchar *name;

        status = cut_test_result_get_status(result);
        if (status == CUT_TEST_RESULT_SUCCESS)
            continue;

        filename = cut_test_result_get_filename(result);
        full_filename = cut_run_context_build_source_filename(run_context,
                                                              filename);

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
            print_test_attributes(console, status, test);

        if (message) {
            g_print("\n");
            print_for_status(console, status, "%s", message);
        }
        g_print("\n%s:%d: %s()\n",
                full_filename,
                cut_test_result_get_line(result),
                cut_test_result_get_function_name(result));
        i++;
        g_free(full_filename);
    }
}

static void
print_summary (CutConsoleUI *console, CutRunContext *run_context,
               gboolean crashed)
{
    guint n_tests, n_assertions, n_successes, n_failures, n_errors;
    guint n_pendings, n_notifications, n_omissions;
    const gchar *color;

    n_tests = cut_run_context_get_n_tests(run_context);
    n_assertions = cut_run_context_get_n_assertions(run_context);
    n_successes = cut_run_context_get_n_successes(run_context);
    n_failures = cut_run_context_get_n_failures(run_context);
    n_errors = cut_run_context_get_n_errors(run_context);
    n_pendings = cut_run_context_get_n_pendings(run_context);
    n_notifications = cut_run_context_get_n_notifications(run_context);
    n_omissions = cut_run_context_get_n_omissions(run_context);

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
        } else if (n_omissions > 0) {
            status = CUT_TEST_RESULT_OMISSION;
        } else if (n_notifications > 0) {
            status = CUT_TEST_RESULT_NOTIFICATION;
        } else {
            status = CUT_TEST_RESULT_SUCCESS;
        }
        color = status_to_color(status);
    }
    print_with_color(console, color,
                     "%d test(s), %d assertion(s), %d failure(s), "
                     "%d error(s), %d pending(s), %d omission(s), "
                     "%d notification(s)",
                     n_tests, n_assertions, n_failures, n_errors,
                     n_pendings, n_omissions, n_notifications);
    g_print("\n");
    print_with_color(console, color,
                     "%g%% passed",
                     100.0 * ((gdouble)n_successes / (gdouble)n_tests));
    g_print("\n");
}

static void
cb_complete_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                        CutConsoleUI *console)
{
    gboolean crashed;
    CutVerboseLevel verbose_level;

    verbose_level = console->verbose_level;
    if (verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;

    if (verbose_level == CUT_VERBOSE_LEVEL_NORMAL)
        g_print("\n");

    crashed = cut_run_context_is_crashed(run_context);
    if (crashed) {
        const gchar *backtrace;
        print_with_color(console, CRASH_COLOR, "CRASH!!!");
        g_print("\n");

        backtrace = cut_run_context_get_backtrace(run_context);
        if (backtrace)
            g_print("%s\n", backtrace);
    }

    print_results(console, run_context);

    g_print("\n");
    g_print("Finished in %f seconds",
            cut_test_get_elapsed(CUT_TEST(test_suite)));
    g_print("\n\n");

    print_summary(console, run_context, crashed);
}

static void
cb_error (CutRunContext *run_context, const gchar *name, const gchar *detail,
          CutConsoleUI *console)
{
    print_with_color(console, status_to_color(CUT_TEST_RESULT_ERROR), "E");
    fflush(stdout);

    console->errors = g_list_append(console->errors, error_new(name, detail));
}

static void
cb_crashed (CutRunContext *run_context, const gchar *backtrace,
            CutConsoleUI *console)
{
    print_with_color(console, CRASH_COLOR, "!");
    fflush(stdout);
}

static void
connect_to_run_context (CutConsoleUI *console, CutRunContext *run_context)
{
#define CONNECT(name) \
    g_signal_connect(run_context, #name, G_CALLBACK(cb_ ## name), console)

    CONNECT(start_test_suite);
    CONNECT(start_test_case);
    CONNECT(start_test_iterator);
    CONNECT(start_test);
    CONNECT(start_iterated_test);

    CONNECT(success_test);
    CONNECT(notification_test);
    CONNECT(omission_test);
    CONNECT(pending_test);
    CONNECT(failure_test);
    CONNECT(error_test);

    CONNECT(complete_test);
    CONNECT(complete_iterated_test);
    CONNECT(complete_test_iterator);
    CONNECT(complete_test_case);
    CONNECT(complete_test_suite);

    CONNECT(error);
    CONNECT(crashed);

#undef CONNECT
}

static void
disconnect_from_run_context (CutConsoleUI *console, CutRunContext *run_context)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(run_context,                   \
                                         G_CALLBACK(cb_ ## name),       \
                                         console)

    DISCONNECT(start_test_suite);
    DISCONNECT(start_test_case);
    DISCONNECT(start_test_iterator);
    DISCONNECT(start_test);
    DISCONNECT(start_iterated_test);

    DISCONNECT(success_test);
    DISCONNECT(notification_test);
    DISCONNECT(omission_test);
    DISCONNECT(pending_test);
    DISCONNECT(failure_test);
    DISCONNECT(error_test);

    DISCONNECT(complete_iterated_test);
    DISCONNECT(complete_test);
    DISCONNECT(complete_test_iterator);
    DISCONNECT(complete_test_case);
    DISCONNECT(complete_test_suite);

    DISCONNECT(error);
    DISCONNECT(crashed);

#undef DISCONNECT
}

static void
attach_to_run_context (CutListener *listener,
                       CutRunContext   *run_context)
{
    connect_to_run_context(CUT_CONSOLE_UI(listener), run_context);
}

static void
detach_from_run_context (CutListener *listener,
                         CutRunContext   *run_context)
{
    disconnect_from_run_context(CUT_CONSOLE_UI(listener), run_context);
}

static gboolean
run (CutUI *ui, CutRunContext *run_context)
{
    return cut_run_context_start(run_context);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
