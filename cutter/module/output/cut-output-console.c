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

#include "cut-module-impl.h"
#include "cut-output.h"
#include "cut-context.h"
#include "cut-test.h"

#define CUT_TYPE_OUTPUT_CONSOLE            cut_type_output_console
#define CUT_OUTPUT_CONSOLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_OUTPUT_CONSOLE, CutOutputConsole))
#define CUT_OUTPUT_CONSOLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_OUTPUT_CONSOLE, CutOutputConsoleClass))
#define CUT_IS_OUTPUT_CONSOLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_OUTPUT_CONSOLE))
#define CUT_IS_OUTPUT_CONSOLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_OUTPUT_CONSOLE))
#define CUT_OUTPUT_CONSOLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_OUTPUT_CONSOLE, CutOutputConsoleClass))

#define RED_COLOR "\033[01;31m"
#define RED_BACK_COLOR "\033[41m"
#define GREEN_COLOR "\033[01;32m"
#define YELLOW_COLOR "\033[01;33m"
#define BLUE_COLOR "\033[01;34m"
#define PURPLE_COLOR "\033[01;35m"
#define CYAN_COLOR "\033[01;36m"
#define WHITE_COLOR "\033[01;37m"
#define NORMAL_COLOR "\033[00m"

typedef struct _CutOutputConsole CutOutputConsole;
typedef struct _CutOutputConsoleClass CutOutputConsoleClass;

struct _CutOutputConsole
{
    CutOutput     object;
    gchar        *name;
    gboolean      use_color;
};

struct _CutOutputConsoleClass
{
    CutOutputClass parent_class;
};

enum
{
    PROP_0,
    PROP_USE_COLOR
};

static GType cut_type_output_console = 0;
static CutOutputClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void on_start_test_suite    (CutOutput *output,
                                    CutTestSuite *test_suite);
static void on_start_test_case     (CutOutput *output,
                                    CutTestCase *test_case);
static void on_start_test          (CutOutput *output,
                                    CutTestCase *test_case,
                                    CutTest *test);
static void on_complete_test       (CutOutput *output,
                                    CutTestCase *test_case,
                                    CutTest *test,
                                    CutTestResult *result);
static void on_success             (CutOutput *output,
                                    CutTest *test);
static void on_failure             (CutOutput *output,
                                    CutTest *test,
                                    CutTestResult *result);
static void on_error               (CutOutput *output,
                                    CutTest *test,
                                    CutTestResult *result);
static void on_pending             (CutOutput *output,
                                    CutTest *test,
                                    CutTestResult *result);
static void on_notification        (CutOutput *output,
                                    CutTest *test,
                                    CutTestResult *result);
static void on_complete_test_case  (CutOutput *output,
                                    CutTestCase *test_case);
static void on_complete_test_suite (CutOutput *output,
                                    CutContext *context,
                                    CutTestSuite *test_suite);
static void on_crashed_test_suite  (CutOutput *output,
                                    CutContext *context,
                                    CutTestSuite *test_suite);

static void
class_init (CutOutputClass *klass)
{
    GObjectClass *gobject_class;
    CutOutputClass *output_class;
    GParamSpec *spec;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    output_class  = CUT_OUTPUT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    output_class->on_start_test_suite    = on_start_test_suite;
    output_class->on_start_test_case     = on_start_test_case;
    output_class->on_start_test          = on_start_test;
    output_class->on_success             = on_success;
    output_class->on_failure             = on_failure;
    output_class->on_error               = on_error;
    output_class->on_pending             = on_pending;
    output_class->on_notification        = on_notification;
    output_class->on_complete_test       = on_complete_test;
    output_class->on_complete_test_case  = on_complete_test_case;
    output_class->on_complete_test_suite = on_complete_test_suite;
    output_class->on_crashed_test_suite  = on_crashed_test_suite;

    spec = g_param_spec_boolean("use-color",
                                "Use color",
                                "Whether use color",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_USE_COLOR, spec);
}

static void
init (CutOutputConsole *output)
{
    output->use_color = FALSE;
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutOutputConsoleClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutOutputConsole),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_output_console = g_type_module_register_type(type_module,
                                                          CUT_TYPE_OUTPUT,
                                                          "CutOutputConsole",
                                                          &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_output_console)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_output_console));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_OUTPUT_CONSOLE, first_property, var_args);
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
    CutOutputConsole *console = CUT_OUTPUT_CONSOLE(object);

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
    CutOutputConsole *console = CUT_OUTPUT_CONSOLE(object);

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
      case CUT_TEST_RESULT_CRASH:
        name = "CRASH!!!";
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
      case CUT_TEST_RESULT_CRASH:
        color = RED_BACK_COLOR WHITE_COLOR;
        break;
      default:
        color = "";
        break;
    }

    return color;
}

static void
print_for_status(CutOutputConsole *console, CutTestResultStatus status,
                 gchar const *format, ...)
{
    va_list args;

    va_start(args, format);
    if (console->use_color) {
        gchar *message;
        message = g_strdup_vprintf(format, args);
        g_print("%s%s%s", status_to_color(status), message, NORMAL_COLOR);
        g_free(message);
    } else {
        g_vprintf(format, args);
    }
    va_end(args);
}

static void
on_start_test_suite (CutOutput *output, CutTestSuite *test_suite)
{
}

static void
on_start_test_case (CutOutput *output, CutTestCase *test_case)
{
}

static void
on_start_test (CutOutput *output, CutTestCase *test_case,
               CutTest *test)
{
    const gchar *description;

    if (cut_output_get_verbose_level(output) < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    description = cut_test_get_description(test);
    if (description)
        g_print("%s\n", description);

    g_print("%s(%s): ",
            cut_test_get_name(test),
            cut_test_get_name(CUT_TEST(test_case)));
    fflush(stdout);
}

static void
on_complete_test (CutOutput *output, CutTestCase *test_case,
                  CutTest *test, CutTestResult *result)
{
    if (result && cut_test_result_get_status(result) == CUT_TEST_RESULT_ERROR)
        cut_output_on_error(output, test, result);

    if (cut_output_get_verbose_level(output) < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    g_print(": (%f)\n", cut_test_get_elapsed(test));
    fflush(stdout);
}

static void
on_success (CutOutput *output, CutTest *test)
{
    CutOutputConsole *console = CUT_OUTPUT_CONSOLE(output);

    if (cut_output_get_verbose_level(output) < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_SUCCESS, ".");
    fflush(stdout);
}

static void
on_failure (CutOutput *output, CutTest *test, CutTestResult *result)
{
    CutOutputConsole *console = CUT_OUTPUT_CONSOLE(output);

    if (cut_output_get_verbose_level(output) < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_FAILURE, "F");
    fflush(stdout);
}

static void
on_error (CutOutput *output, CutTest *test, CutTestResult *result)
{
    CutOutputConsole *console = CUT_OUTPUT_CONSOLE(output);

    if (cut_output_get_verbose_level(output) < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_ERROR, "E");
    fflush(stdout);
}

static void
on_pending (CutOutput *output, CutTest *test, CutTestResult *result)
{
    CutOutputConsole *console = CUT_OUTPUT_CONSOLE(output);

    if (cut_output_get_verbose_level(output) < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_PENDING, "P");
    fflush(stdout);
}

static void
on_notification (CutOutput *output, CutTest *test,
                 CutTestResult *result)
{
    CutOutputConsole *console = CUT_OUTPUT_CONSOLE(output);

    if (cut_output_get_verbose_level(output) < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_for_status(console, CUT_TEST_RESULT_NOTIFICATION, "N");
    fflush(stdout);
}

static void
on_complete_test_case (CutOutput *output, CutTestCase *test_case)
{
    if (cut_output_get_verbose_level(output) < CUT_VERBOSE_LEVEL_VERBOSE)
        return;
}

static void
on_complete_test_suite (CutOutput *output, CutContext *context,
                        CutTestSuite *test_suite)
{
    gint i;
    guint n_tests, n_assertions, n_failures, n_errors;
    guint n_pendings, n_notifications;
    const GList *node;
    CutTestResultStatus status;
    gboolean crashed;
    CutOutputConsole *console = CUT_OUTPUT_CONSOLE(output);

    if (cut_output_get_verbose_level(output) < CUT_VERBOSE_LEVEL_NORMAL)
        return;

    if (cut_output_get_verbose_level(output) == CUT_VERBOSE_LEVEL_NORMAL)
        g_print("\n");

    crashed = cut_context_is_crashed(context);
    if (crashed) {
        print_for_status(console, CUT_TEST_RESULT_CRASH,
                         "%s", status_to_name(CUT_TEST_RESULT_CRASH));
        g_print("\n");
    }

    i = 1;
    for (node = cut_context_get_results(context);
         node;
         node = g_list_next(node)) {
        CutTestResult *result = node->data;
        gchar *filename;
        const gchar *message;
        const gchar *source_directory;

        source_directory = cut_output_get_source_directory(output);
        if (source_directory)
            filename = g_build_filename(source_directory,
                                        cut_test_result_get_filename(result),
                                        NULL);
        else
            filename = g_strdup(cut_test_result_get_filename(result));

        status = cut_test_result_get_status(result);
        message = cut_test_result_get_message(result);

        g_print("\n%d) ", i);
        print_for_status(console, status,
                         "%s : %s",
                         status_to_name(status),
                         cut_test_result_get_test_name(result));
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

    g_print("\n");
    g_print("Finished in %f seconds",
            cut_test_get_elapsed(CUT_TEST(test_suite)));
    g_print("\n\n");


    n_tests = cut_context_get_n_tests(context);
    n_assertions = cut_context_get_n_assertions(context);
    n_failures = cut_context_get_n_failures(context);
    n_errors = cut_context_get_n_errors(context);
    n_pendings = cut_context_get_n_pendings(context);
    n_notifications = cut_context_get_n_notifications(context);

    if (crashed) {
        status = CUT_TEST_RESULT_CRASH;
    } else if (n_errors > 0) {
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
    print_for_status(console, status,
                     "%d test(s), %d assertion(s), %d failure(s), "
                     "%d error(s), %d pending(s), %d notification(s)",
                     n_tests, n_assertions, n_failures, n_errors,
                     n_pendings, n_notifications);
    g_print("\n");
}

static void
on_crashed_test_suite (CutOutput *output, CutContext *context,
                       CutTestSuite *test_suite)
{
    CutOutputConsole *console = CUT_OUTPUT_CONSOLE(output);

    print_for_status(console, CUT_TEST_RESULT_CRASH, "!");
    fflush(stdout);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
