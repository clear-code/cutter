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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <sys/types.h>
#include <glib.h>

#include "cut-test-context.h"
#include "cut-test-suite.h"
#include "cut-test-result.h"
#include "cut-process.h"

#define CUT_TEST_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_CONTEXT, CutTestContextPrivate))

typedef struct _CutTestContextPrivate	CutTestContextPrivate;
struct _CutTestContextPrivate
{
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutTest *test;
    gboolean failed;
    jmp_buf *jump_buffer;
    GList *taken_strings;
    gpointer user_data;
    GDestroyNotify user_data_destroy_notify;
    GList *processes;
};

enum
{
    PROP_0,
    PROP_TEST_SUITE,
    PROP_TEST_CASE,
    PROP_TEST
};

G_DEFINE_TYPE (CutTestContext, cut_test_context, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void
cut_test_context_class_init (CutTestContextClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_object("test-suite",
                               "Test suite",
                               "The test suite of the test context",
                               CUT_TYPE_TEST_SUITE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST_SUITE, spec);

    spec = g_param_spec_object("test-case",
                               "Test case",
                               "The test case of the test context",
                               CUT_TYPE_TEST_CASE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST_CASE, spec);

    spec = g_param_spec_object("test",
                               "Test",
                               "The test of the test context",
                               CUT_TYPE_TEST,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST, spec);

    g_type_class_add_private(gobject_class, sizeof(CutTestContextPrivate));
}

static void
cut_test_context_init (CutTestContext *context)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    priv->test_suite = NULL;
    priv->test_case = NULL;
    priv->test = NULL;

    priv->failed = FALSE;

    priv->taken_strings = NULL;

    priv->user_data = NULL;
    priv->user_data_destroy_notify = NULL;

    priv->processes = NULL;
}

static void
dispose (GObject *object)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(object);

    if (priv->test_suite) {
        g_object_unref(priv->test_suite);
        priv->test_suite = NULL;
    }

    if (priv->test_case) {
        g_object_unref(priv->test_case);
        priv->test_case = NULL;
    }

    if (priv->test) {
        g_object_unref(priv->test);
        priv->test = NULL;
    }

    if (priv->processes) {
        g_list_foreach(priv->processes, (GFunc)g_object_unref, NULL);
        g_list_free(priv->processes);
        priv->processes = NULL;
    }

    g_list_foreach(priv->taken_strings, (GFunc)g_free, NULL);
    g_list_free(priv->taken_strings);
    priv->taken_strings = NULL;

    if (priv->user_data && priv->user_data_destroy_notify)
        priv->user_data_destroy_notify(priv->user_data);
    priv->user_data = NULL;

    G_OBJECT_CLASS(cut_test_context_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestContext *context = CUT_TEST_CONTEXT(object);

    switch (prop_id) {
      case PROP_TEST_SUITE:
        cut_test_context_set_test_suite(context, g_value_get_object(value));
        break;
      case PROP_TEST_CASE:
        cut_test_context_set_test_case(context, g_value_get_object(value));
        break;
      case PROP_TEST:
        cut_test_context_set_test(context, g_value_get_object(value));
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
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_TEST_SUITE:
        g_value_set_object(value, priv->test_suite);
        break;
      case PROP_TEST_CASE:
        g_value_set_object(value, priv->test_case);
        break;
      case PROP_TEST:
        g_value_set_object(value, priv->test);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestContext *
cut_test_context_new (CutTestSuite *test_suite, CutTestCase *test_case,
                      CutTest *test)
{
    return g_object_new(CUT_TYPE_TEST_CONTEXT,
                        "test-suite", test_suite,
                        "test-case", test_case,
                        "test", test,
                        NULL);
}

CutTestSuite *
cut_test_context_get_test_suite (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->test_suite;
}

void
cut_test_context_set_test_suite (CutTestContext *context, CutTestSuite *test_suite)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    if (priv->test_suite)
        g_object_unref(priv->test_suite);
    if (test_suite)
        g_object_ref(test_suite);
    priv->test_suite = test_suite;
}

CutTestCase *
cut_test_context_get_test_case (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->test_case;
}

void
cut_test_context_set_test_case (CutTestContext *context, CutTestCase *test_case)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    if (priv->test_case)
        g_object_unref(priv->test_case);
    if (test_case)
        g_object_ref(test_case);
    priv->test_case = test_case;
}

CutTest *
cut_test_context_get_test (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->test;
}

void
cut_test_context_set_test (CutTestContext *context, CutTest *test)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    if (priv->test)
        g_object_unref(priv->test);
    if (test)
        g_object_ref(test);
    priv->test = test;
}

gpointer
cut_test_context_get_user_data (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->user_data;
}

void
cut_test_context_set_user_data (CutTestContext *context, gpointer user_data,
                                GDestroyNotify notify)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    if (priv->user_data && priv->user_data_destroy_notify)
        priv->user_data_destroy_notify(priv->user_data);

    priv->user_data = user_data;
    priv->user_data_destroy_notify = notify;
}

void
cut_test_context_set_jump (CutTestContext *context, jmp_buf *buffer)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    priv->jump_buffer = buffer;
}

void
cut_test_context_long_jump (CutTestContext *context)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    longjmp(*(priv->jump_buffer), 1);
}

void
cut_test_context_pass_assertion (CutTestContext *context)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    g_return_if_fail(priv->test);

    g_signal_emit_by_name(priv->test, "pass-assertion", context);
}

void
cut_test_context_register_result (CutTestContext *context,
                                  CutTestResultStatus status,
                                  const gchar *function_name,
                                  const gchar *filename,
                                  guint line,
                                  const gchar *message,
                                  ...)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    CutTestResult *result;
    const gchar *status_signal_name = NULL;
    const gchar *system_message;
    gchar *user_message = NULL, *user_message_format;
    va_list args;

    priv->failed = TRUE;

    system_message = message;
    va_start(args, message);
    user_message_format = va_arg(args, gchar *);
    if (user_message_format) {
        user_message = g_strdup_vprintf(user_message_format, args);
    }
    va_end(args);

    result = cut_test_result_new(status,
                                 priv->test, priv->test_case, priv->test_suite,
                                 user_message, system_message,
                                 function_name, filename, line);
    if (user_message)
        g_free(user_message);

    status_signal_name = cut_test_result_status_to_signal_name(status);
    if (priv->test) {
        cut_test_stop_timer(priv->test);
        g_object_set(result, "elapsed", cut_test_get_elapsed(priv->test), NULL);
        g_signal_emit_by_name(priv->test, status_signal_name,
                              context, result);
    } else if (priv->test_case) {
        g_signal_emit_by_name(priv->test_case, status_signal_name,
                              context, result);
    }

    g_object_unref(result);
}

gboolean
cut_test_context_is_failed (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->failed;
}

const char *
cut_test_context_take_string (CutTestContext *context,
                              char           *string)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    char *taken_string;

    taken_string = g_strdup(string);
    priv->taken_strings = g_list_prepend(priv->taken_strings, taken_string);
    g_free(string);

    return taken_string;
}

const char *
cut_test_context_take_printf (CutTestContext *context, char *format, ...)
{
    const char *taken_string = NULL;
    va_list args;

    va_start(args, format);
    if (format) {
        char *formatted_string;
        formatted_string = g_strdup_vprintf(format, args);
        taken_string = cut_test_context_take_string(context, formatted_string);
    }
    va_end(args);

    return taken_string;
}

const char *
cut_test_context_inspect_string_array (CutTestContext *context,
                                       const char    **strings)
{
    GString *inspected;
    const char **string, **next_string;

    if (!strings)
        return "(null)";

    inspected = g_string_new("[");
    string = strings;
    while (*string) {
        g_string_append_printf(inspected, "\"%s\"", *string);
        next_string = string + 1;
        if (*next_string)
            g_string_append(inspected, ", ");
        string = next_string;
    }
    g_string_append(inspected, "]");

    return cut_test_context_take_string(context,
                                        g_string_free(inspected, FALSE));
}

int
cut_test_context_trap_fork (CutTestContext *context,
                            unsigned int time_out)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    CutProcess *process;
    int pid;

    process = cut_process_new();
    priv->processes = g_list_prepend(priv->processes, process);

    pid = cut_process_fork(process); 

    return pid;
}

static CutProcess *
get_process_from_pid (CutTestContext *context, int pid)
{
    GList *node;
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    for (node = priv->processes; node; node = g_list_next(node)) {
        CutProcess *process = CUT_PROCESS(node->data);
        if (pid == cut_process_get_pid(process))
            return process;
    }

    return NULL;
}

const char *
cut_test_context_get_forked_stdout_message (CutTestContext *context,
                                            int pid)
{
    CutProcess *process;

    process = get_process_from_pid(context, pid);
    if (process)
        return cut_process_get_stdout_message(process);

    return NULL;
}

const char *
cut_test_context_get_forked_stderr_message (CutTestContext *context,
                                            int pid)
{
    CutProcess *process;

    process = get_process_from_pid(context, pid);
    if (process)
        return cut_process_get_stderr_message(process);

    return NULL;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
