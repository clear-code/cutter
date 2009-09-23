/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2009  Kouhei Sutou <kou@clear-code.com>
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
#include <setjmp.h>
#include <sys/types.h>
#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <glib.h>
#include <glib/gi18n-lib.h>

#ifdef G_OS_WIN32
#  include <process.h>		/* For getpid() */
#  include <io.h>
#  define STRICT		/* Strict typing, please */
#  define _WIN32_WINDOWS 0x0401 /* to get IsDebuggerPresent */
#  include <windows.h>
#  undef STRICT
#endif

#include "cut-run-context.h"
#include "cut-test-context.h"
#include "cut-test-suite.h"
#include "cut-test-iterator.h"
#include "cut-test-result.h"
#include "cut-test-data.h"
#include "cut-process.h"
#include "cut-backtrace-entry.h"
#include "cut-utils.h"

#define cut_omit(context, message) do                           \
{                                                               \
    cut_test_context_register_result(context,                   \
                                     CUT_TEST_RESULT_OMISSION,  \
                                     message);                  \
    cut_test_context_long_jump(context);                        \
} while (0)

#define CUT_TEST_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_CONTEXT, CutTestContextPrivate))

typedef struct _CutTestContextPrivate	CutTestContextPrivate;
struct _CutTestContextPrivate
{
    CutRunContext *run_context;
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutTestIterator *test_iterator;
    CutTest *test;
    gboolean failed;
    gboolean is_multi_thread;
    jmp_buf *jump_buffer;
    GList *taken_objects;
    GList *taken_string_arrays;
    GList *taken_g_objects;
    GList *taken_errors;
    GList *taken_lists;
    GList *taken_hash_tables;
    GList *current_data;
    GList *data_list;
    GList *processes;
    gchar *fixture_data_dir;
    GHashTable *cached_fixture_data;
    GList *backtrace;
    gchar *user_message;
    guint user_message_keep_count;
    GMutex *mutex;
    gchar *expected;
    gchar *actual;
};

enum
{
    PROP_0,
    PROP_RUN_CONTEXT,
    PROP_TEST_SUITE,
    PROP_TEST_CASE,
    PROP_TEST_ITERATOR,
    PROP_TEST
};

G_DEFINE_TYPE (CutTestContext, cut_test_context, G_TYPE_OBJECT)

typedef struct _TakenObject TakenObject;
struct _TakenObject
{
    gpointer object;
    CutDestroyFunction destroy_function;
};

static TakenObject *
taken_object_new (gpointer object, CutDestroyFunction destroy_function)
{
    TakenObject *taken_object;

    taken_object = g_slice_new(TakenObject);
    taken_object->object = object;
    taken_object->destroy_function = destroy_function;
    return taken_object;
}

static void
taken_object_free (TakenObject *taken_object)
{
    if (taken_object->object)
        taken_object->destroy_function(taken_object->object);
    g_slice_free(TakenObject, taken_object);
}

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
    taken_list->list = list;
    taken_list->destroy_function = destroy_function;
    return taken_list;
}

static void
taken_list_free (TakenList *taken_list)
{
    if (taken_list->destroy_function) {
        GList *node;

        for (node = taken_list->list; node; node = g_list_next(node)) {
            if (node->data)
                taken_list->destroy_function(node->data);
        }
    }
    g_list_free(taken_list->list);

    g_slice_free(TakenList, taken_list);
}

static GStaticPrivate current_context_private;

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

    spec = g_param_spec_object("run-context",
                               "Run context",
                               "The run context of the test context",
                               CUT_TYPE_RUN_CONTEXT,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_RUN_CONTEXT, spec);

    spec = g_param_spec_object("test-suite",
                               "Test suite",
                               "The test suite of the test context",
                               CUT_TYPE_TEST_SUITE,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TEST_SUITE, spec);

    spec = g_param_spec_object("test-case",
                               "Test case",
                               "The test case of the test context",
                               CUT_TYPE_TEST_CASE,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TEST_CASE, spec);

    spec = g_param_spec_object("test-iterator",
                               "Test iterator",
                               "The test iterator of the test context",
                               CUT_TYPE_TEST_ITERATOR,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TEST_ITERATOR, spec);

    spec = g_param_spec_object("test",
                               "Test",
                               "The test of the test context",
                               CUT_TYPE_TEST,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TEST, spec);

    g_type_class_add_private(gobject_class, sizeof(CutTestContextPrivate));
}

static void
cut_test_context_init (CutTestContext *context)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    priv->run_context = NULL;
    priv->test_suite = NULL;
    priv->test_case = NULL;
    priv->test_iterator = NULL;
    priv->test = NULL;

    priv->failed = FALSE;
    priv->is_multi_thread = FALSE;

    priv->taken_objects = NULL;

    priv->data_list = NULL;
    priv->current_data = NULL;

    priv->processes = NULL;

    priv->fixture_data_dir = NULL;
    priv->cached_fixture_data = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                      g_free, g_free);

    priv->backtrace = NULL;

    priv->user_message = NULL;
    priv->user_message_keep_count = 0;

    priv->mutex = g_mutex_new();

    priv->expected = NULL;
    priv->actual = NULL;
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
clear_user_message (CutTestContextPrivate *priv)
{
    if (priv->user_message_keep_count > 0)
        priv->user_message_keep_count--;

    if (priv->user_message_keep_count == 0) {
        if (priv->user_message) {
            g_free(priv->user_message);
            priv->user_message = NULL;
        }
    }
}

static void
clear_additional_test_result_data (CutTestContextPrivate *priv)
{
    if (priv->expected) {
        g_free(priv->expected);
        priv->expected = NULL;
    }

    if (priv->actual) {
        g_free(priv->actual);
        priv->actual = NULL;
    }
}

static void
dispose (GObject *object)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(object);

    if (priv->run_context) {
        g_object_unref(priv->run_context);
        priv->run_context = NULL;
    }

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

    if (priv->taken_objects) {
        g_list_foreach(priv->taken_objects, (GFunc)taken_object_free, NULL);
        g_list_free(priv->taken_objects);
        priv->taken_objects = NULL;
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

    if (priv->backtrace) {
        g_list_foreach(priv->backtrace, (GFunc)g_object_unref, NULL);
        g_list_free(priv->backtrace);
        priv->backtrace = NULL;
    }

    clear_user_message(priv);

    if (priv->mutex) {
        g_mutex_free(priv->mutex);
        priv->mutex = NULL;
    }

    clear_additional_test_result_data(priv);

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
      case PROP_RUN_CONTEXT:
        cut_test_context_set_run_context(context, g_value_get_object(value));
        break;
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
      case PROP_RUN_CONTEXT:
        g_value_set_object(value, priv->run_context);
        break;
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

GQuark
cut_test_context_error_quark (void)
{
    return g_quark_from_static_string("cut-test-context-error-quark");
}

void
cut_test_context_current_init (void)
{
    g_static_private_init(&current_context_private);
}

void
cut_test_context_current_quit (void)
{
    g_static_private_free(&current_context_private);
}

static void
contexts_free (gpointer data)
{
    GPtrArray *contexts = data;
    g_ptr_array_free(contexts, TRUE);
}

void
cut_test_context_current_push (CutTestContext *context)
{
    GPtrArray *contexts;

    contexts = g_static_private_get(&current_context_private);
    if (!contexts) {
        contexts = g_ptr_array_new();
        g_static_private_set(&current_context_private, contexts, contexts_free);
    }
    g_ptr_array_add(contexts, context);
}

CutTestContext *
cut_test_context_current_pop (void)
{
    GPtrArray *contexts;
    CutTestContext *context = NULL;

    contexts = g_static_private_get(&current_context_private);
    if (contexts) {
        context = g_ptr_array_index(contexts, contexts->len - 1);
        g_ptr_array_remove_index(contexts, contexts->len - 1);
    }

    return context;
}

CutTestContext *
cut_test_context_current_peek (void)
{
    GPtrArray *contexts;

    contexts = g_static_private_get(&current_context_private);
    if (contexts)
        return g_ptr_array_index(contexts, contexts->len - 1);
    else
        return NULL;
}

CutTestContext *
cut_test_context_new (CutRunContext *run_context,
                      CutTestSuite *test_suite, CutTestCase *test_case,
                      CutTestIterator *test_iterator, CutTest *test)
{
    return g_object_new(CUT_TYPE_TEST_CONTEXT,
                        "run-context", run_context,
                        "test-suite", test_suite,
                        "test-case", test_case,
                        "test-iterator", test_iterator,
                        "test", test,
                        NULL);
}

CutTestContext *
cut_test_context_new_empty (void)
{
    return cut_test_context_new(NULL, NULL, NULL, NULL, NULL);
}

CutRunContext *
cut_test_context_get_run_context (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->run_context;
}

void
cut_test_context_set_run_context (CutTestContext *context, CutRunContext *run_context)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    if (priv->run_context)
        g_object_unref(priv->run_context);
    if (run_context)
        g_object_ref(run_context);

    priv->run_context = run_context;
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
cut_test_context_set_attributes (CutTestContext *context,
                                 const gchar *first_attribute_name,
                                 ...)
{
    CutTestContextPrivate *priv;
    CutTest *target = NULL;
    const gchar *name;
    va_list args;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    if (priv->test) {
        target = priv->test;
    } else if (priv->test_iterator) {
        target = CUT_TEST(priv->test_iterator);
    } else if (priv->test_case) {
        target = CUT_TEST(priv->test_case);
    } else if (priv->test_suite) {
        target = CUT_TEST(priv->test_suite);
    }

    if (!target) {
        g_warning("can't find a target to set attributes.");
        return;
    }

    name = first_attribute_name;
    va_start(args, first_attribute_name);
    while (name) {
        const gchar *value;

        value = va_arg(args, gchar *);
        cut_test_set_attribute(target, name, value);

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

CutTestData *
cut_test_context_get_data (CutTestContext *context, guint index)
{
    CutTestContextPrivate *priv;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    return g_list_nth_data(priv->data_list, index);
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
    CutTestContextPrivate *priv;

    g_return_if_fail(CUT_IS_TEST_CONTEXT(context));

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    g_return_if_fail(priv->test);

    clear_user_message(priv);
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
                                  const char *system_message)
{
    CutTestContextPrivate *priv;
    CutTestResult *result;
    CutTestData *test_data = NULL;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    if (cut_test_result_status_is_critical(status))
        priv->failed = TRUE;

    if (priv->current_data)
        test_data = priv->current_data->data;
    result = cut_test_result_new(status,
                                 priv->test, priv->test_iterator,
                                 priv->test_case, priv->test_suite,
                                 test_data,
                                 priv->user_message, system_message,
                                 priv->backtrace);
    clear_user_message(priv);

    cut_test_result_set_expected(result, priv->expected);
    cut_test_result_set_actual(result, priv->actual);
    clear_additional_test_result_data(priv);

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

    if (status == CUT_TEST_RESULT_FAILURE &&
        priv->run_context &&
        cut_run_context_get_fatal_failures(priv->run_context)) {
        const gchar *message;

        message = _("Treat a failure as a fatal problem, aborting.");
        cut_run_context_emit_error(priv->run_context,
                                   CUT_TEST_CONTEXT_ERROR,
                                   CUT_TEST_CONTEXT_ERROR_FATAL,
                                   NULL, "%s", message);
        cut_run_context_emit_complete_run(priv->run_context, FALSE);
#ifdef G_OS_WIN32
        if (IsDebuggerPresent())
            G_BREAKPOINT();
        else
            abort();
#else
#  ifdef SIGTRAP
        G_BREAKPOINT();
#  else
        abort();
#  endif
#endif
    }

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

const void *
cut_test_context_take (CutTestContext *context,
                       void           *object,
                       CutDestroyFunction destroy_function)
{
    CutTestContextPrivate *priv;
    TakenObject *taken_object;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    taken_object = taken_object_new(object, destroy_function);
    priv->taken_objects = g_list_prepend(priv->taken_objects, taken_object);

    return object;
}

const void *
cut_test_context_take_memory (CutTestContext *context,
                              void           *memory)
{
    return cut_test_context_take(context, memory, g_free);
}

const char *
cut_test_context_take_string (CutTestContext *context,
                              char           *string)
{
    return cut_test_context_take(context, string, g_free);
}

const char *
cut_test_context_take_strdup (CutTestContext *context,
                              const char     *string)
{
    return cut_test_context_take_string(context, g_strdup(string));
}

const char *
cut_test_context_take_strndup (CutTestContext *context,
                               const char     *string,
                               size_t          size)
{
    return cut_test_context_take_string(context, g_strndup(string, size));
}

const void *
cut_test_context_take_memdup (CutTestContext *context,
                              const void     *memory,
                              size_t          size)
{
    return cut_test_context_take_memory(context, g_memdup(memory, size));
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
    return (const char **)cut_test_context_take(context,
                                                (gpointer)strings,
                                                (CutDestroyFunction)g_strfreev);
}

GObject *
cut_test_context_take_g_object (CutTestContext *context,
                                GObject        *object)
{
    return (GObject *)cut_test_context_take(context, object, g_object_unref);
}

const GError *
cut_test_context_take_g_error (CutTestContext *context, GError *error)
{
    return cut_test_context_take(context, error,
                                 (CutDestroyFunction)g_error_free);
}

const GList *
cut_test_context_take_g_list (CutTestContext *context, GList *list,
                              CutDestroyFunction destroy_function)
{
    TakenList *taken_list;

    taken_list = taken_list_new(list, destroy_function);
    cut_test_context_take(context, taken_list,
                          (CutDestroyFunction)taken_list_free);

    return list;
}

GHashTable *
cut_test_context_take_g_hash_table (CutTestContext *context,
                                    GHashTable     *hash_table)
{
    return (GHashTable *)cut_test_context_take(context,
                                               hash_table,
                                               (CutDestroyFunction)g_hash_table_unref);
}

int
cut_test_context_trap_fork (CutTestContext *context)
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
                                         "can't use cut_fork() "
                                         "in multi thread mode");
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
    priv->fixture_data_dir = cut_utils_expand_path_va_list(path, args);
    va_end(args);
}

gchar *
cut_test_context_build_fixture_data_path_va_list (CutTestContext *context,
                                                  const gchar *path,
                                                  va_list args)
{
    CutTestContextPrivate *priv;
    gchar *concatenated_path, *full_path;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    concatenated_path = cut_utils_build_path_va_list(path, args);
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
    full_path = cut_test_context_build_fixture_data_path_va_list(context,
                                                                 path, args);
    va_end(args);

    return full_path;
}

const gchar *
cut_test_context_get_fixture_data_string_va_list (CutTestContext *context,
                                                  GError **error,
                                                  gchar **full_path,
                                                  const gchar *path,
                                                  va_list args)
{
    CutTestContextPrivate *priv;
    gpointer value;
    gchar *_full_path;

    if (!path)
        return NULL;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    _full_path = cut_test_context_build_fixture_data_path_va_list(context,
                                                                  path, args);
    value = g_hash_table_lookup(priv->cached_fixture_data, _full_path);
    if (!value) {
        gchar *contents;
        gsize length;

        if (g_file_get_contents(_full_path, &contents, &length, error)) {
            g_hash_table_insert(priv->cached_fixture_data,
                                g_strdup(_full_path),
                                contents);
            value = contents;
        }
    }

    if (full_path)
        *full_path = _full_path;
    else
        g_free(_full_path);

    return value;
}

const gchar *
cut_test_context_get_fixture_data_string (CutTestContext *context,
                                          GError **error,
                                          gchar **full_path,
                                          const gchar *path, ...)
{
    const gchar *value;
    va_list args;

    if (!path)
        return NULL;

    va_start(args, path);
    value = cut_test_context_get_fixture_data_string_va_list(context, error,
                                                             full_path,
                                                             path, args);
    va_end(args);

    return value;
}

gchar *
cut_test_context_build_source_filename (CutTestContext *context,
                                        const gchar    *filename)
{
    CutTestContextPrivate *priv;
    const gchar *base_directory = NULL;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    if (priv->test)
        base_directory = cut_test_get_base_directory(priv->test);
    if (!base_directory && priv->test_case)
        base_directory = cut_test_get_base_directory(CUT_TEST(priv->test_case));
    if (!base_directory && priv->test_suite)
        base_directory = cut_test_get_base_directory(CUT_TEST(priv->test_suite));

    if (base_directory) {
        gchar *relative_filename, *full_filename;

        relative_filename = g_build_filename(base_directory, filename, NULL);
        if (priv->run_context) {
            full_filename =
                cut_run_context_build_source_filename(priv->run_context,
                                                      relative_filename);
            g_free(relative_filename);
            return full_filename;
        } else {
            return relative_filename;
        }
    }


    if (priv->run_context)
        return cut_run_context_build_source_filename(priv->run_context,
                                                     filename);
    else
        return g_strdup(filename);
}

static gchar *
convert_directory_separator_in_relative_path (const gchar *relative_path)
{
    gchar *convert, *dirname, *basename;

    dirname = g_path_get_dirname(relative_path);
    basename = g_path_get_basename(relative_path);

    convert = g_build_filename(dirname, basename, NULL);
    g_free(dirname);
    g_free(basename);

    return convert;
}

void
cut_test_context_push_backtrace (CutTestContext *context,
                                 const char     *relative_path,
                                 const char     *filename,
                                 unsigned int    line,
                                 const char     *function_name,
                                 const char     *info)
{
    CutTestContextPrivate *priv;
    CutBacktraceEntry *entry;
    gchar *full_filename = NULL;

    g_return_if_fail(context != NULL);
    g_return_if_fail(CUT_IS_TEST_CONTEXT(context));

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    g_mutex_lock(priv->mutex);
    if (relative_path) {
        const gchar *source_directory = NULL;
        gchar *convert_relative_path;

        convert_relative_path =
            convert_directory_separator_in_relative_path(relative_path);

        if (priv->run_context)
            source_directory =
                cut_run_context_get_source_directory(priv->run_context);
        if (source_directory)
            full_filename = g_build_filename(source_directory,
                                             convert_relative_path,
                                             filename,
                                             NULL);
        else
            full_filename = g_build_filename(convert_relative_path,
                                             filename, NULL);
        g_free(convert_relative_path);
    } else {
        full_filename = cut_test_context_build_source_filename(context,
                                                               filename);
    }
    entry = cut_backtrace_entry_new(full_filename, line, function_name, info);
    g_free(full_filename);
    priv->backtrace = g_list_prepend(priv->backtrace, entry);
    g_mutex_unlock(priv->mutex);
}

void
cut_test_context_pop_backtrace (CutTestContext *context)
{
    CutTestContextPrivate *priv;
    CutBacktraceEntry *entry;

    g_return_if_fail(context != NULL);
    g_return_if_fail(CUT_IS_TEST_CONTEXT(context));

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    g_return_if_fail(priv->backtrace != NULL);

    g_mutex_lock(priv->mutex);
    entry = priv->backtrace->data;
    g_object_unref(entry);
    priv->backtrace = g_list_delete_link(priv->backtrace, priv->backtrace);
    g_mutex_unlock(priv->mutex);
}

void
cut_test_context_get_last_backtrace (CutTestContext *context,
                                     const char    **filename,
                                     unsigned int   *line,
                                     const char    **function_name,
                                     const char    **info)
{
    CutTestContextPrivate *priv;
    CutBacktraceEntry *entry;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    g_return_if_fail(priv->backtrace != NULL);

    entry = priv->backtrace->data;
    if (filename)
        *filename = cut_backtrace_entry_get_file(entry);
    if (line)
        *line = cut_backtrace_entry_get_line(entry);
    if (function_name)
        *function_name = cut_backtrace_entry_get_function(entry);
    if (info)
        *info = cut_backtrace_entry_get_info(entry);
}

void
cut_test_context_set_user_message (CutTestContext *context,
                                   const char     *format,
                                   ...)
{
    va_list args;

    va_start(args, format);
    cut_test_context_set_user_message_va_list(context, format, args);
    va_end(args);
}

void
cut_test_context_set_user_message_backward_compatibility (CutTestContext *context,
                                                          const char     *format,
                                                          ...)
{
    va_list args;

    if (!format)
        return;

    va_start(args, format);
    cut_test_context_set_user_message_va_list(context, format, args);
    va_end(args);
}

void
cut_test_context_set_user_message_va_list (CutTestContext *context,
                                           const char     *format,
                                           va_list         args)
{
    CutTestContextPrivate *priv;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    clear_user_message(priv);
    if (!format)
        return;

    priv->user_message = g_strdup_vprintf(format, args);
}

void
cut_test_context_check_optional_assertion_message (CutTestContext *context,
                                                   const char *message)
{
    CutTestContextPrivate *priv;

    if (!message)
        return;
    if (message[0] == '\0')
        return;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    clear_user_message(priv);
    priv->user_message = g_strdup_printf("optional assertion message "
                                         "on the current environment "
                                         "is unavailable: <%s>", message);
}

void
cut_test_context_keep_user_message (CutTestContext *context)
{
    CUT_TEST_CONTEXT_GET_PRIVATE(context)->user_message_keep_count++;
}

void
cut_test_context_set_expected (CutTestContext *context, const gchar *expected)
{
    CutTestContextPrivate *priv;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    if (priv->expected)
        g_free(priv->expected);
    priv->expected = g_strdup(expected);
}

void
cut_test_context_set_actual (CutTestContext *context, const gchar *actual)
{
    CutTestContextPrivate *priv;

    priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);
    if (priv->actual)
        g_free(priv->actual);
    priv->actual = g_strdup(actual);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
