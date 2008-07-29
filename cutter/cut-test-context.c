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
#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <glib.h>

#include "cut-test-context.h"
#include "cut-test-suite.h"
#include "cut-test-iterator.h"
#include "cut-test-result.h"
#include "cut-test-data.h"
#include "cut-process.h"
#include "cut-utils.h"

#define cut_omit(context, message, ...) do                      \
{                                                               \
    cut_test_context_register_result(context,                   \
                                     CUT_TEST_RESULT_OMISSION,  \
                                     __PRETTY_FUNCTION__,       \
                                     __FILE__, __LINE__,        \
                                     message, ## __VA_ARGS__,   \
                                     NULL);                     \
    cut_test_context_long_jump(context);                        \
} while (0)

#define CUT_TEST_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_CONTEXT, CutTestContextPrivate))

typedef struct _CutTestContextPrivate	CutTestContextPrivate;
struct _CutTestContextPrivate
{
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutTestIterator *test_iterator;
    CutTest *test;
    gboolean failed;
    gboolean is_multi_thread;
    jmp_buf *jump_buffer;
    GList *taken_strings;
    GList *taken_string_arrays;
    GList *taken_objects;
    GList *taken_errors;
    GList *taken_lists;
    GList *current_data;
    GList *data_list;
    GList *processes;
    gchar *fixture_data_dir;
    GHashTable *cached_fixture_data;
};

enum
{
    PROP_0,
    PROP_TEST_SUITE,
    PROP_TEST_CASE,
    PROP_TEST_ITERATOR,
    PROP_TEST
};

G_DEFINE_TYPE (CutTestContext, cut_test_context, G_TYPE_OBJECT)

typedef struct _TakenList TakenList;
struct _TakenList
{
    GList *list;
    CutDestroyFunction destroy_function;
};

static TakenList *
taken_list_new (GList *list, CutDestroyFunction destroy_function)
{
    TakenList *taken_list;

    taken_list = g_slice_new(TakenList);
    taken_list->list = g_list_copy(list);
    taken_list->destroy_function = destroy_function;
    return taken_list;
}

static void
taken_list_free (TakenList *taken_list)
{
    if (taken_list->destroy_function)
        g_list_foreach(taken_list->list,
                       (GFunc)taken_list->destroy_function,
                       NULL);
    g_list_free(taken_list->list);

    g_slice_free(TakenList, taken_list);
}


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

    spec = g_param_spec_object("test-iterator",
                               "Test iterator",
                               "The test iterator of the test context",
                               CUT_TYPE_TEST_ITERATOR,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEST_ITERATOR, spec);

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
    priv->test_iterator = NULL;
    priv->test = NULL;

    priv->failed = FALSE;
    priv->is_multi_thread = FALSE;

    priv->taken_strings = NULL;
    priv->taken_string_arrays = NULL;

    priv->taken_objects = NULL;
    priv->taken_errors = NULL;
    priv->taken_lists = NULL;

    priv->data_list = NULL;
    priv->current_data = NULL;

    priv->processes = NULL;

    priv->fixture_data_dir = NULL;
    priv->cached_fixture_data = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                      g_free, g_free);
}

static void
free_data_list (CutTestContextPrivate *priv)
{
    if (priv->data_list) {
        g_list_foreach(priv->data_list, (GFunc)g_object_unref, NULL);
        g_list_free(priv->data_list);
        priv->data_list = NULL;
    }
    priv->current_data = NULL;
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

    if (priv->test_iterator) {
        g_object_unref(priv->test_iterator);
        priv->test_iterator = NULL;
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

    if (priv->taken_strings) {
        g_list_foreach(priv->taken_strings, (GFunc)g_free, NULL);
        g_list_free(priv->taken_strings);
        priv->taken_strings = NULL;
    }

    if (priv->taken_string_arrays) {
        g_list_foreach(priv->taken_string_arrays, (GFunc)g_strfreev, NULL);
        g_list_free(priv->taken_string_arrays);
        priv->taken_string_arrays = NULL;
    }

    if (priv->taken_objects) {
        g_list_foreach(priv->taken_objects, (GFunc)g_object_unref, NULL);
        g_list_free(priv->taken_objects);
        priv->taken_objects = NULL;
    }

    if (priv->taken_errors) {
        g_list_foreach(priv->taken_errors, (GFunc)g_error_free, NULL);
        g_list_free(priv->taken_errors);
        priv->taken_errors = NULL;
    }

    if (priv->taken_lists) {
        g_list_foreach(priv->taken_lists, (GFunc)taken_list_free, NULL);
        g_list_free(priv->taken_lists);
        priv->taken_lists = NULL;
    }

    free_data_list(priv);

    if (priv->fixture_data_dir) {
        g_free(priv->fixture_data_dir);
        priv->fixture_data_dir = NULL;
    }

    if (priv->cached_fixture_data) {
        g_hash_table_unref(priv->cached_fixture_data);
        priv->cached_fixture_data = NULL;
    }

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
      case PROP_TEST_ITERATOR:
        cut_test_context_set_test_iterator(context, g_value_get_object(value));
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
      case PROP_TEST_ITERATOR:
        g_value_set_object(value, priv->test_iterator);
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
                      CutTestIterator *test_iterator, CutTest *test)
{
    return g_object_new(CUT_TYPE_TEST_CONTEXT,
                        "test-suite", test_suite,
                        "test-case", test_case,
                        "test-iterator", test_iterator,
                        "test", test,
                        NULL);
}

CutTestContext *
cut_test_context_new_empty (void)
{
    return cut_test_context_new(NULL, NULL, NULL, NULL);
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

CutTestIterator *
cut_test_context_get_test_iterator (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->test_iterator;
}

void
cut_test_context_set_test_iterator (CutTestContext *context,
                                    CutTestIterator *test_iterator)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    if (priv->test_iterator)
        g_object_unref(priv->test_iterator);
    if (test_iterator)
        g_object_ref(test_iterator);
    priv->test_iterator = test_iterator;
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

void
cut_test_context_add_data (CutTestContext *context, const gchar *first_data_name,
                           ...)
{
    CutTestContextPrivate *priv;
    const gchar *name;
    va_list args;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    name = first_data_name;
    va_start(args, first_data_name);
    while (name) {
        CutTestData *test_data;
        gpointer data;
        CutDestroyFunction destroy_function;

        data = va_arg(args, gpointer);
        destroy_function = va_arg(args, CutDestroyFunction);
        test_data = cut_test_data_new(name, data, destroy_function);
        priv->data_list = g_list_prepend(priv->data_list, test_data);
        if (!priv->current_data)
            priv->current_data = priv->data_list;

        name = va_arg(args, gchar *);
    }
    va_end(args);
}

void
cut_test_context_set_data (CutTestContext *context, CutTestData *test_data)
{
    CutTestContextPrivate *priv;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    free_data_list(priv);

    if (test_data) {
        g_object_ref(test_data);
        priv->data_list = g_list_prepend(priv->data_list, test_data);
        priv->current_data = priv->data_list;
    }
}

void
cut_test_context_shift_data (CutTestContext *context)
{
    CutTestContextPrivate *priv;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    if (priv->current_data)
        priv->current_data = g_list_previous(priv->current_data);
}

gboolean
cut_test_context_have_data (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->current_data != NULL;
}

CutTestData *
cut_test_context_get_current_data (CutTestContext *context)
{
    CutTestContextPrivate *priv;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    g_return_val_if_fail(priv->current_data != NULL, NULL);

    return priv->current_data->data;
}

guint
cut_test_context_get_n_data (CutTestContext *context)
{
    return g_list_length(CUT_TEST_CONTEXT_GET_PRIVATE(context)->data_list);
}

void
cut_test_context_set_jump (CutTestContext *context, jmp_buf *buffer)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    priv->jump_buffer = buffer;
}

jmp_buf *
cut_test_context_get_jump (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->jump_buffer;
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

void
cut_test_context_emit_signal (CutTestContext *context,
                              CutTestResult *result)
{
    CutTestContextPrivate *priv;
    CutTest *target = NULL;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    if (priv->test) {
        target = priv->test;
    } else if (priv->test_iterator) {
        target = CUT_TEST(priv->test_iterator);
    } else if (priv->test_case) {
        target = CUT_TEST(priv->test_case);
    }

    if (target)
        cut_test_emit_result_signal(target, context, result);
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
    CutTestContextPrivate *priv;
    CutTestResult *result;
    CutTestData *test_data = NULL;
    const gchar *system_message;
    gchar *user_message = NULL, *user_message_format;
    va_list args;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    if (cut_test_result_status_is_critical(status))
        priv->failed = TRUE;

    system_message = message;
    va_start(args, message);
    user_message_format = va_arg(args, gchar *);
    if (user_message_format) {
        user_message = g_strdup_vprintf(user_message_format, args);
    }
    va_end(args);

    if (priv->current_data)
        test_data = priv->current_data->data;
    result = cut_test_result_new(status,
                                 priv->test, priv->test_iterator,
                                 priv->test_case, priv->test_suite,
                                 test_data,
                                 user_message, system_message,
                                 function_name, filename, line);
    if (user_message)
        g_free(user_message);

    if (priv->test) {
        CutProcess *process;
        /* If the current procss is a child process, the pid is 0. */
        process = get_process_from_pid(context, 0);
        if (process) {
            cut_process_send_test_result_to_parent(process, result);
            g_object_unref(result);
            if (status == CUT_TEST_RESULT_NOTIFICATION)
                return;
            cut_process_exit(process);
        }
    }

    cut_test_context_emit_signal(context, result);

    g_object_unref(result);
}

void
cut_test_context_set_failed (CutTestContext *context, gboolean failed)
{
    CUT_TEST_CONTEXT_GET_PRIVATE(context)->failed = failed;
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
cut_test_context_take_printf (CutTestContext *context, const char *format, ...)
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

const char **
cut_test_context_take_string_array (CutTestContext *context,
                                    char          **strings)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    char **taken_strings;

    taken_strings = g_strdupv(strings);
    priv->taken_string_arrays = g_list_prepend(priv->taken_string_arrays, taken_strings);
    g_strfreev(strings);

    return (const char **)taken_strings;
}

GObject *
cut_test_context_take_g_object (CutTestContext *context,
                                GObject        *object)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    priv->taken_objects = g_list_prepend(priv->taken_objects, object);

    return object;
}

const GError *
cut_test_context_take_g_error (CutTestContext *context, GError *error)
{
    CutTestContextPrivate *priv;
    GError *taken_error;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    taken_error = g_error_copy(error);
    g_error_free(error);
    priv->taken_errors = g_list_prepend(priv->taken_errors, taken_error);

    return taken_error;
}

const GList *
cut_test_context_take_g_list (CutTestContext *context, GList *list,
                              CutDestroyFunction destroy_function)
{
    CutTestContextPrivate *priv;
    TakenList *taken_list;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    taken_list = taken_list_new(list, destroy_function);
    priv->taken_lists = g_list_prepend(priv->taken_lists, taken_list);
    g_list_free(list);

    return taken_list->list;
}

int
cut_test_context_trap_fork (CutTestContext *context,
                            const gchar *function_name,
                            const gchar *filename,
                            guint line)
{
#ifdef G_OS_WIN32
    cut_omit(context,
             "cut_test_context_wait_process() doesn't "
             "work on the environment.");
    return 0;
#else
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    CutProcess *process;

    if (cut_test_context_is_multi_thread(context)) {
        cut_test_context_register_result(context,
                                         CUT_TEST_RESULT_OMISSION,
                                         function_name, filename, line,
                                         "can't use cut_fork() "
                                         "in multi thread mode",
                                         NULL);
        cut_test_context_long_jump(context);
    }

    process = cut_process_new();
    priv->processes = g_list_prepend(priv->processes, process);

    return cut_process_fork(process);
#endif
}

int
cut_test_context_wait_process (CutTestContext *context,
                               int pid, unsigned int timeout)
{
    int exit_status = EXIT_SUCCESS;
#ifdef G_OS_WIN32
    cut_omit(context,
             "cut_test_context_wait_process() doesn't "
             "work on the environment.");
#else
    CutProcess *process;

    process = get_process_from_pid(context, pid);
    if (process) {
        int process_status;
        const gchar *xml;

        process_status = cut_process_wait(process, timeout);
        exit_status = WEXITSTATUS(process_status);
        xml = cut_process_get_result_from_child(process);
        if (xml)
            xml = strstr(xml, "<result>");
        while (xml) {
            CutTestResult *result;
            gchar *next_result_xml;
            gint result_xml_length = -1;

            next_result_xml = strstr(xml + 1, "<result>");
            if (next_result_xml)
                result_xml_length = next_result_xml - xml;

            result = cut_test_result_new_from_xml(xml, result_xml_length, NULL);
            if (result) {
                cut_test_context_emit_signal(context, result);
                g_object_unref(result);
            }

            if (result_xml_length == -1)
                xml = NULL;
            else
                xml += result_xml_length;
        }
    }
#endif
    return exit_status;
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

void
cut_test_context_set_multi_thread (CutTestContext *context,
                                   gboolean is_multi_thread)
{
    CUT_TEST_CONTEXT_GET_PRIVATE(context)->is_multi_thread = is_multi_thread;
}

gboolean
cut_test_context_is_multi_thread (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->is_multi_thread;
}

gchar *
cut_test_context_to_xml (CutTestContext *context)
{
    GString *string;

    string = g_string_new(NULL);
    cut_test_context_to_xml_string(context, string, 0);
    return g_string_free(string, FALSE);
}

void
cut_test_context_to_xml_string (CutTestContext *context, GString *string,
                                guint indent)
{
    CutTestContextPrivate *priv;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    cut_utils_append_indent(string, indent);
    g_string_append(string, "<test-context>\n");

    if (priv->test_suite)
        cut_test_to_xml_string(CUT_TEST(priv->test_suite), string, indent + 2);
    if (priv->test_case)
        cut_test_to_xml_string(CUT_TEST(priv->test_case), string, indent + 2);
    if (priv->test_iterator)
        cut_test_to_xml_string(CUT_TEST(priv->test_iterator),
                               string, indent + 2);
    if (priv->test)
        cut_test_to_xml_string(priv->test, string, indent + 2);
    if (priv->current_data)
        cut_test_data_to_xml_string(priv->current_data->data,
                                    string, indent + 2);

    cut_utils_append_xml_element_with_boolean_value(string, indent + 2,
                                                    "failed", priv->failed);


    cut_utils_append_indent(string, indent);
    g_string_append(string, "</test-context>\n");
}

void
cut_test_context_set_fixture_data_dir (CutTestContext *context,
                                       const gchar *path, ...)
{
    CutTestContextPrivate *priv;
    va_list args;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    if (priv->fixture_data_dir)
        g_free(priv->fixture_data_dir);

    va_start(args, path);
    priv->fixture_data_dir = cut_utils_expand_pathv(path, &args);
    va_end(args);
}

gchar *
cut_test_context_build_fixture_data_pathv (CutTestContext *context,
                                           const gchar *path, va_list *args)
{
    CutTestContextPrivate *priv;
    gchar *concatenated_path, *full_path;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    concatenated_path = cut_utils_build_pathv(path, args);
    if (g_path_is_absolute(concatenated_path)) {
        full_path = concatenated_path;
    } else {
        if (priv->fixture_data_dir) {
            full_path = g_build_filename(priv->fixture_data_dir,
                                         concatenated_path,
                                         NULL);
        } else {
            full_path = cut_utils_expand_path(concatenated_path);
        }
        g_free(concatenated_path);
    }

    return full_path;
}

gchar *
cut_test_context_build_fixture_data_path (CutTestContext *context,
                                          const gchar *path, ...)
{
    va_list args;
    gchar *full_path;

    va_start(args, path);
    full_path = cut_test_context_build_fixture_data_pathv(context, path, &args);
    va_end(args);

    return full_path;
}

const gchar *
cut_test_context_get_fixture_data_stringv (CutTestContext *context,
                                           GError **error,
                                           const gchar *path,
                                           va_list *args)
{
    CutTestContextPrivate *priv;
    gchar *full_path;
    gpointer value;

    if (!path)
        return NULL;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    full_path = cut_test_context_build_fixture_data_pathv(context, path, args);
    value = g_hash_table_lookup(priv->cached_fixture_data, full_path);
    if (value) {
        g_free(full_path);
    } else {
        gchar *contents;
        gsize length;

        if (g_file_get_contents(full_path, &contents, &length, error)) {
            g_hash_table_insert(priv->cached_fixture_data, full_path, contents);
            value = contents;
        } else {
            g_free(full_path);
        }
    }

    return value;
}

const gchar *
cut_test_context_get_fixture_data_string (CutTestContext *context,
                                          GError **error,
                                          const gchar *path, ...)
{
    const gchar *value;
    va_list args;

    if (!path)
        return NULL;

    va_start(args, path);
    value = cut_test_context_get_fixture_data_stringv(context, error,
                                                      path, &args);
    va_end(args);

    return value;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
