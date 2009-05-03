/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2009  Kouhei Sutou <kou@cozmixng.org>
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

#include "cut-test-case.h"
#include "cut-test.h"
#include "cut-run-context.h"
#include "cut-test-result.h"
#include "cut-crash-backtrace.h"

#include <gcutter/gcut-marshalers.h>

#define CUT_TEST_CASE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_CASE, CutTestCasePrivate))

typedef struct _CutTestCasePrivate	CutTestCasePrivate;
struct _CutTestCasePrivate
{
    CutSetupFunction setup;
    CutTeardownFunction teardown;
    CutStartupFunction startup;
    CutShutdownFunction shutdown;
};

enum
{
    PROP_0,
    PROP_SETUP_FUNCTION,
    PROP_TEARDOWN_FUNCTION,
    PROP_STARTUP_FUNCTION,
    PROP_SHUTDOWN_FUNCTION
};

enum
{
    READY,
    START_TEST,
    COMPLETE_TEST,
    START_TEST_ITERATOR,
    COMPLETE_TEST_ITERATOR,

    SUCCESS_IN,
    FAILURE_IN,
    ERROR_IN,
    PENDING_IN,
    NOTIFICATION_IN,
    OMISSION_IN,
    CRASH_IN,

    LAST_SIGNAL
};

static gint cut_test_case_signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE (CutTestCase, cut_test_case, CUT_TYPE_TEST_CONTAINER)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);
static void emit_result_signal
                           (CutTest        *test,
                            CutTestContext *test_context,
                            CutTestResult  *result);

static void
cut_test_case_class_init (CutTestCaseClass *klass)
{
    GObjectClass *gobject_class;
    CutTestClass *test_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);
    test_class = CUT_TEST_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    test_class->emit_result_signal = emit_result_signal;

    spec = g_param_spec_pointer("setup-function",
                                "Setup Function",
                                "The function for setup",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_SETUP_FUNCTION, spec);

    spec = g_param_spec_pointer("teardown-function",
                                "Teardown Function",
                                "The function for teardown",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TEARDOWN_FUNCTION, spec);

    spec = g_param_spec_pointer("startup-function",
                                "Startup Function",
                                "The function for initialization of TestCase",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class,
                                    PROP_STARTUP_FUNCTION,
                                    spec);

    spec = g_param_spec_pointer("shutdown-function",
                                "Shutdown Function",
                                "The function for finalization of TestCase",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class,
                                    PROP_SHUTDOWN_FUNCTION,
                                    spec);


    cut_test_case_signals[READY]
        = g_signal_new("ready",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestCaseClass, ready),
                       NULL, NULL,
                       g_cclosure_marshal_VOID__UINT,
                       G_TYPE_NONE, 1, G_TYPE_UINT);

    cut_test_case_signals[START_TEST]
        = g_signal_new("start-test",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestCaseClass, start_test),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

    cut_test_case_signals[COMPLETE_TEST]
        = g_signal_new("complete-test",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestCaseClass, complete_test),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT_BOOLEAN,
                       G_TYPE_NONE, 3,
                       CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT, G_TYPE_BOOLEAN);

    cut_test_case_signals[START_TEST_ITERATOR]
        = g_signal_new("start-test-iterator",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestCaseClass, start_test_iterator),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE,
                       2, CUT_TYPE_TEST_ITERATOR, CUT_TYPE_TEST_CONTEXT);

    cut_test_case_signals[COMPLETE_TEST_ITERATOR]
        = g_signal_new("complete-test-iterator",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestCaseClass, complete_test_iterator),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT_BOOLEAN,
                       G_TYPE_NONE,
                       3, CUT_TYPE_TEST_ITERATOR, CUT_TYPE_TEST_CONTEXT,
                       G_TYPE_BOOLEAN);

    cut_test_case_signals[FAILURE_IN]
        = g_signal_new("failure-in",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestCaseClass, failure_in),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2,
                       CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_case_signals[ERROR_IN]
        = g_signal_new("error-in",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestCaseClass, error_in),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2,
                       CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_case_signals[PENDING_IN]
        = g_signal_new("pending-in",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestCaseClass, pending_in),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2,
                       CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_case_signals[NOTIFICATION_IN]
        = g_signal_new("notification-in",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestCaseClass, notification_in),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2,
                       CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_case_signals[OMISSION_IN]
        = g_signal_new("omission-in",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestCaseClass, omission_in),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2,
                       CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    cut_test_case_signals[CRASH_IN]
        = g_signal_new("crash-in",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(CutTestCaseClass, crash_in),
                       NULL, NULL,
                       _gcut_marshal_VOID__OBJECT_OBJECT,
                       G_TYPE_NONE, 2,
                       CUT_TYPE_TEST_CONTEXT, CUT_TYPE_TEST_RESULT);

    g_type_class_add_private(gobject_class, sizeof(CutTestCasePrivate));
}

static void
cut_test_case_init (CutTestCase *test_case)
{
    CutTestCasePrivate *priv = CUT_TEST_CASE_GET_PRIVATE(test_case);

    priv->setup = NULL;
    priv->teardown = NULL;
    priv->startup = NULL;
    priv->shutdown = NULL;
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(cut_test_case_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestCasePrivate *priv = CUT_TEST_CASE_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_SETUP_FUNCTION:
        priv->setup = g_value_get_pointer(value);
        break;
      case PROP_TEARDOWN_FUNCTION:
        priv->teardown = g_value_get_pointer(value);
        break;
      case PROP_STARTUP_FUNCTION:
        priv->startup = g_value_get_pointer(value);
        break;
      case PROP_SHUTDOWN_FUNCTION:
        priv->shutdown = g_value_get_pointer(value);
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
    CutTestCasePrivate *priv = CUT_TEST_CASE_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_SETUP_FUNCTION:
        g_value_set_pointer(value, priv->setup);
        break;
      case PROP_TEARDOWN_FUNCTION:
        g_value_set_pointer(value, priv->teardown);
        break;
      case PROP_STARTUP_FUNCTION:
        g_value_set_pointer(value, priv->startup);
        break;
      case PROP_SHUTDOWN_FUNCTION:
        g_value_set_pointer(value, priv->shutdown);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestCase *
cut_test_case_new (const gchar *name,
                   CutSetupFunction setup,
                   CutTeardownFunction teardown,
                   CutStartupFunction startup,
                   CutShutdownFunction shutdown)
{
    return g_object_new(CUT_TYPE_TEST_CASE,
                        "name", name,
                        "element-name", "test-case",
                        "setup-function", setup,
                        "teardown-function", teardown,
                        "startup-function", startup,
                        "shutdown-function", shutdown,
                        NULL);
}

CutTestCase *
cut_test_case_new_empty (void)
{
    return cut_test_case_new(NULL,
                             NULL, NULL,
                             NULL, NULL);
}

static GList *
get_filtered_tests (CutTestCase *test_case, const gchar **test_names)
{
    CutTestContainer *container;

    container = CUT_TEST_CONTAINER(test_case);
    if (test_names && *test_names) {
        return cut_test_container_filter_children(container, test_names);
    } else {
        return g_list_copy(cut_test_container_get_children(container));
    }
}

void
cut_test_case_add_test (CutTestCase *test_case, CutTest *test)
{
    cut_test_container_add_test(CUT_TEST_CONTAINER(test_case), test);
}

void
cut_test_case_run_setup (CutTestCase *test_case, CutTestContext *test_context)
{
    CutTestCasePrivate *priv;

    priv = CUT_TEST_CASE_GET_PRIVATE(test_case);
    if (priv->setup) {
        jmp_buf jump_buffer;

        cut_test_context_set_jump(test_context, &jump_buffer);
        if (setjmp(jump_buffer) == 0) {
            priv->setup();
        }
    }
}

void
cut_test_case_run_teardown (CutTestCase *test_case, CutTestContext *test_context)
{
    CutTestCasePrivate *priv;

    priv = CUT_TEST_CASE_GET_PRIVATE(test_case);
    if (priv->teardown) {
        jmp_buf jump_buffer;

        cut_test_context_set_jump(test_context, &jump_buffer);
        if (setjmp(jump_buffer) == 0) {
            priv->teardown();
        }
    }
}

static gboolean
run_test (CutTestCase *test_case, CutTest *test,
          CutTestContext *test_context, CutRunContext *run_context)
{
    gboolean success = TRUE;

    if (CUT_IS_TEST_ITERATOR(test)) {
        g_signal_emit_by_name(test_case, "start-test-iterator",
                              test, test_context);
    } else {
        g_signal_emit_by_name(test_case, "start-test", test, test_context);
        cut_test_case_run_setup(test_case, test_context);
    }

    if (cut_test_context_is_failed(test_context)) {
        success = FALSE;
    } else {
        success = cut_test_run(test, test_context, run_context);
    }

    if (CUT_IS_TEST_ITERATOR(test)) {
        g_signal_emit_by_name(test_case, "complete-test-iterator",
                              test, test_context, success);
    } else {
        cut_test_case_run_teardown(test_case, test_context);
        g_signal_emit_by_name(test_case, "complete-test",
                              test, test_context, success);
    }

    return success;
}

static gboolean
run (CutTestCase *test_case, CutTest *test, CutRunContext *run_context)
{
    CutTestContext *test_context;
    gboolean success = TRUE;
    gboolean is_multi_thread;
    CutTestSuite *test_suite;

    if (cut_run_context_is_canceled(run_context))
        return TRUE;

    test_suite = cut_run_context_get_test_suite(run_context);
    is_multi_thread = cut_run_context_is_multi_thread(run_context);

    test_context = cut_test_context_new(run_context,
                                        test_suite, test_case, NULL, NULL);
    cut_test_context_set_multi_thread(test_context, is_multi_thread);

    cut_test_context_current_push(test_context);
    cut_test_context_set_test(test_context, test);
    success = run_test(test_case, test, test_context, run_context);
    cut_test_context_set_test(test_context, NULL);

    g_object_unref(test_context);
    cut_test_context_current_pop();
    /* FIXME: We want to use the code:
       g_object_unref(cut_test_context_current_pop());
       We need to hide cut_set_current_test_context() from user.
    */

    return success;
}

static void
cb_test_status (CutTest *test, CutTestContext *context, CutTestResult *result,
                gpointer data)
{
    CutTestResultStatus *status = data;

    *status = MAX(*status, cut_test_result_get_status(result));
}

static void
cut_test_case_run_startup (CutTestCase *test_case, CutTestContext *test_context)
{
    CutTestCasePrivate *priv;

    priv = CUT_TEST_CASE_GET_PRIVATE(test_case);
    if (priv->startup) {
        jmp_buf jump_buffer;

        cut_test_context_set_jump(test_context, &jump_buffer);
        if (setjmp(jump_buffer) == 0) {
            priv->startup();
        }
    }
}

static void
cut_test_case_run_shutdown (CutTestCase *test_case, CutTestContext *test_context)
{
    CutTestCasePrivate *priv;

    priv = CUT_TEST_CASE_GET_PRIVATE(test_case);
    if (priv->shutdown) {
        jmp_buf jump_buffer;

        cut_test_context_set_jump(test_context, &jump_buffer);
        if (setjmp(jump_buffer) == 0) {
            priv->shutdown();
        }
    }
}

static void
emit_result_signal (CutTest *test, CutTestContext *test_context,
                    CutTestResult *result)
{
    const gchar *status_signal_name = NULL;
    gchar *signal_name;
    CutTestResultStatus status;

    status = cut_test_result_get_status(result);
    status_signal_name = cut_test_result_status_to_signal_name(status);
    signal_name = g_strdup_printf("%s-in", status_signal_name);
    g_signal_emit_by_name(test, signal_name, test_context, result);
    g_free(signal_name);
}

static void
cut_test_case_emit_result_signal (CutTestCase *test_case, CutTestResult *result)
{
    const gchar *status_signal_name = NULL;
    CutTestResultStatus status;

    cut_test_set_result_elapsed(CUT_TEST(test_case), result);

    status = cut_test_result_get_status(result);
    status_signal_name = cut_test_result_status_to_signal_name(status);
    g_signal_emit_by_name(test_case, status_signal_name, NULL, result);
}

static gboolean
run_tests (CutTestCase *test_case, CutRunContext *run_context,
           const GList *tests, CutTestResultStatus *status)
{
    const GList *list;
    gboolean all_success = TRUE;

    for (list = tests; list; list = g_list_next(list)) {
        CutTest *test = list->data;

        if (!test)
            continue;

        if (CUT_IS_TEST(test)) {
            g_signal_connect(test, "success", G_CALLBACK(cb_test_status),
                             status);
            g_signal_connect(test, "failure", G_CALLBACK(cb_test_status),
                             status);
            g_signal_connect(test, "error", G_CALLBACK(cb_test_status),
                             status);
            g_signal_connect(test, "pending", G_CALLBACK(cb_test_status),
                             status);
            g_signal_connect(test, "notification", G_CALLBACK(cb_test_status),
                             status);
            g_signal_connect(test, "omission", G_CALLBACK(cb_test_status),
                             status);
            g_signal_connect(test, "crash", G_CALLBACK(cb_test_status),
                             status);
            if (!run(test_case, test, run_context))
                all_success = FALSE;
            g_signal_handlers_disconnect_by_func(test,
                                                 G_CALLBACK(cb_test_status),
                                                 status);
        } else {
            g_warning("This object is not CutTest object");
        }
    }

    return all_success;
}

static void
cb_test_case_status_in (CutTestCase *test_case, CutTestContext *test_context,
                        CutTestResult *result, gpointer data)
{
    CutTestResultStatus *status = data;

    *status = MAX(*status, cut_test_result_get_status(result));
}

static gboolean
cut_test_case_run_tests (CutTestCase *test_case, CutRunContext *run_context,
                         const GList *tests)
{
    CutTestCasePrivate *priv;
    CutTestContext *test_context;
    gboolean all_success = TRUE;
    CutTestResult *result;
    CutTestResultStatus status = CUT_TEST_RESULT_SUCCESS;
    gint signum;
    jmp_buf jump_buffer;
    CutTestSuite *test_suite;
    CutCrashBacktrace *crash_backtrace = NULL;
    gboolean is_multi_thread;

    g_signal_emit_by_name(test_case, "ready", g_list_length((GList *)tests));
    g_signal_emit_by_name(CUT_TEST(test_case), "start", NULL);

    priv = CUT_TEST_CASE_GET_PRIVATE(test_case);

    test_suite = cut_run_context_get_test_suite(run_context);
    test_context = cut_test_context_new(run_context,
                                        test_suite, test_case, NULL, NULL);
    cut_test_context_current_push(test_context);

#define CONNECT(event)                                          \
    g_signal_connect(test_case, #event "-in",                   \
                     G_CALLBACK(cb_test_case_status_in),        \
                     &status)

    CONNECT(failure);
    CONNECT(error);
    CONNECT(pending);
    CONNECT(notification);
    CONNECT(omission);
    CONNECT(crash);

#undef CONNECT

    is_multi_thread = cut_run_context_is_multi_thread(run_context);
    if (is_multi_thread) {
        signum = 0;
    } else {
        crash_backtrace = cut_crash_backtrace_new(&jump_buffer);
        signum = setjmp(jump_buffer);
    }
    switch (signum) {
    case 0:
        cut_test_case_run_startup(test_case, test_context);
        if (cut_test_context_is_failed(test_context)) {
            all_success = FALSE;
        } else {
            if (status != CUT_TEST_RESULT_OMISSION)
                all_success = run_tests(test_case, run_context, tests, &status);
        }

        if (crash_backtrace)
            cut_crash_backtrace_free(crash_backtrace);

        break;
#ifndef G_OS_WIN32
    case SIGSEGV:
    case SIGABRT:
        all_success = FALSE;
        cut_crash_backtrace_emit(test_suite, test_case, NULL, NULL, NULL,
                                 test_context);
        break;
    case SIGINT:
        cut_run_context_cancel(run_context);
        break;
#endif
    default:
        break;
    }

    cut_test_case_run_shutdown(test_case, test_context);

    g_signal_handlers_disconnect_by_func(test_case,
                                         G_CALLBACK(cb_test_case_status_in),
                                         &status);
    result = cut_test_result_new(status,
                                 NULL, NULL, test_case, NULL, NULL,
                                 NULL, NULL, NULL);
    cut_test_case_emit_result_signal(test_case, result);
    g_object_unref(result);

    g_signal_emit_by_name(CUT_TEST(test_case), "complete", NULL, all_success);

    g_object_unref(test_context);
    cut_test_context_current_pop();
    /* FIXME: We want to use the code:
         g_object_unref(cut_test_context_current_pop());
       We need to hide cut_set_current_test_context() from user.
    */

    return all_success;
}

gboolean
cut_test_case_run_test (CutTestCase *test_case, CutRunContext *run_context, const gchar *name)
{
    const gchar *test_names[] = {NULL, NULL};

    g_return_val_if_fail(CUT_IS_TEST_CASE(test_case), FALSE);

    test_names[0] = name;
    return cut_test_case_run_with_filter(test_case, run_context, test_names);
}

gboolean
cut_test_case_run_with_filter (CutTestCase *test_case,
                               CutRunContext *run_context,
                               const gchar **test_names)
{
    GList *filtered_tests;
    gboolean success = TRUE;

    filtered_tests = get_filtered_tests(test_case, test_names);
    if (!filtered_tests)
        return TRUE;

    success = cut_test_case_run_tests(test_case, run_context, filtered_tests);

    g_list_free(filtered_tests);

    return success;
}

gboolean
cut_test_case_run (CutTestCase *test_case, CutRunContext *run_context)
{
    const gchar **test_names;

    test_names = cut_run_context_get_target_test_names(run_context);
    return cut_test_case_run_with_filter(test_case, run_context, test_names);
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
