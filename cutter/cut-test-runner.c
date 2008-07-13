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

#include <glib.h>

#include "cut-test-runner.h"
#include "cut-runner.h"
#include "cut-repository.h"
#include "cut-test-case.h"
#include "cut-test-result.h"

#include "cut-marshalers.h"
#include "cut-enum-types.h"

static void runner_init (CutRunnerIface *iface);


static CutRunnerIface *parent_runner_iface;

G_DEFINE_TYPE_WITH_CODE(CutTestRunner, cut_test_runner, CUT_TYPE_RUN_CONTEXT,
                        G_IMPLEMENT_INTERFACE(CUT_TYPE_RUNNER, runner_init))

static void prepare_test_suite  (CutRunContext  *context,
                                 CutTestSuite   *test_suite);
static void prepare_test_case   (CutRunContext  *context,
                                 CutTestCase    *test_case);
static void prepare_test_iterator
                                (CutRunContext  *context,
                                 CutTestIterator *test_iterator);
static void prepare_test        (CutRunContext  *context,
                                 CutTest        *test);
static void prepare_iterated_test
                                (CutRunContext  *context,
                                 CutIteratedTest *iterated_test);
static gboolean runner_run (CutRunner *runner);

static void
cut_test_runner_class_init (CutTestRunnerClass *klass)
{
    CutRunContextClass *run_context_class;

    run_context_class = CUT_RUN_CONTEXT_CLASS(klass);

    run_context_class->prepare_test_suite = prepare_test_suite;
    run_context_class->prepare_test_case = prepare_test_case;
    run_context_class->prepare_test_iterator = prepare_test_iterator;
    run_context_class->prepare_test = prepare_test;
    run_context_class->prepare_iterated_test = prepare_iterated_test;
}

static void
cut_test_runner_init (CutTestRunner *runner)
{
}

static void
runner_init (CutRunnerIface *iface)
{
    parent_runner_iface = g_type_interface_peek_parent(iface);
    iface->run_async = NULL;
    iface->run       = runner_run;
}

CutRunContext *
cut_test_runner_new (void)
{
    return g_object_new(CUT_TYPE_TEST_RUNNER, NULL);
}

static void
cb_start_test (gpointer test_case_or_test_iterator, CutTest *test,
               CutTestContext *test_context, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "start-test", test, test_context);
}

static void
cb_complete_test (gpointer test_case_or_test_iterator, CutTest *test,
                  CutTestContext *test_context, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "complete-test", test, test_context);
}

static void
cb_success_test_case (CutTestCase *test_case, CutTestContext *test_context,
                      CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "success-test-case", test_case, result);
}

static void
cb_failure_test_case (CutTestCase *test_case, CutTestContext *test_context,
                      CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "failure-test-case", test_case, result);
}

static void
cb_error_test_case (CutTestCase *test_case, CutTestContext *test_context,
                    CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "error-test-case", test_case, result);
}

static void
cb_pending_test_case (CutTestCase *test_case, CutTestContext *test_context,
                      CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "pending-test-case", test_case, result);
}

static void
cb_notification_test_case (CutTestCase *test_case, CutTestContext *test_context,
                           CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "notification-test-case", test_case, result);
}

static void
cb_omission_test_case (CutTestCase *test_case, CutTestContext *test_context,
                       CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "omission-test-case", test_case, result);
}

static void
cb_start_test_case(CutTestCase *test_case, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "start-test-case", test_case);
}

static void
cb_ready_test_case(CutTestCase *test_case, guint n_tests, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "ready-test-case", test_case, n_tests);
}

static void
cb_complete_test_case(CutTestCase *test_case, gpointer data)
{
    CutRunContext *context = data;

#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test_case,                     \
                                         G_CALLBACK(cb_ ## name),       \
                                         data)

    DISCONNECT(start_test);
    DISCONNECT(complete_test);
#undef DISCONNECT

#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test_case,                     \
                                         G_CALLBACK(cb_ ## name ## _test_case), \
                                         data)

    DISCONNECT(success);
    DISCONNECT(failure);
    DISCONNECT(error);
    DISCONNECT(pending);
    DISCONNECT(notification);
    DISCONNECT(omission);

    DISCONNECT(ready);
    DISCONNECT(start);
    DISCONNECT(complete);
#undef DISCONNECT

    g_signal_emit_by_name(context, "complete-test-case", test_case);
}

static void
cb_ready_test_suite(CutTestSuite *test_suite, guint n_test_cases,
                    guint n_tests, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "ready-test-suite", test_suite,
                          n_test_cases, n_tests);
}

static void
cb_start_test_suite(CutTestSuite *test_suite, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "start-test-suite", test_suite);
}

static void
cb_crashed_test_suite(CutTestSuite *test_suite, const gchar *backtrace,
                      gpointer data)
{
    CutRunContext *context = data;

    cut_run_context_crash(context, backtrace);
    g_signal_emit_by_name(context, "crashed", backtrace);
}

static void
cb_complete_test_suite(CutTestSuite *test_suite, gpointer data)
{
    CutRunContext *context = data;

#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test_suite,                    \
                                         G_CALLBACK(cb_ ##              \
                                                    name ##             \
                                                    _test_suite),       \
                                         data)

    DISCONNECT(ready);
    DISCONNECT(start);
    DISCONNECT(complete);
    DISCONNECT(crashed);
#undef DISCONNECT

    g_signal_emit_by_name(context, "complete-test-suite", test_suite);
}

static void
prepare_test_suite (CutRunContext *context, CutTestSuite *test_suite)
{
    CutRunContextClass *klass;

    klass = CUT_RUN_CONTEXT_CLASS(cut_test_runner_parent_class);
    if (klass->prepare_test_suite)
        klass->prepare_test_suite(context, test_suite);

#define CONNECT(name)                                                \
    g_signal_connect(test_suite, #name,                              \
                     G_CALLBACK(cb_ ## name ## _test_suite),         \
                     context)

    CONNECT(ready);
    CONNECT(start);
    CONNECT(complete);
    CONNECT(crashed);
#undef CONNECT
}

static void
prepare_test_case (CutRunContext *context, CutTestCase *test_case)
{
    CutRunContextClass *klass;

    klass = CUT_RUN_CONTEXT_CLASS(cut_test_runner_parent_class);
    if (klass->prepare_test_case)
        klass->prepare_test_case(context, test_case);

#define CONNECT(name) \
    g_signal_connect(test_case, #name, G_CALLBACK(cb_ ## name), context)

    CONNECT(start_test);
    CONNECT(complete_test);
#undef CONNECT

#define CONNECT(name)                                                   \
    g_signal_connect(test_case, #name,                                  \
                     G_CALLBACK(cb_ ## name ## _test_case), context)

    CONNECT(success);
    CONNECT(failure);
    CONNECT(error);
    CONNECT(pending);
    CONNECT(notification);
    CONNECT(omission);

    CONNECT(ready);
    CONNECT(start);
    CONNECT(complete);
#undef CONNECT
}

static void
cb_success_test_iterator (CutTestIterator *test_iterator,
                          CutTestContext *test_context,
                          CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "success-test-iterator", test_iterator, result);
}

static void
cb_failure_test_iterator (CutTestIterator *test_iterator,
                          CutTestContext *test_context,
                          CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "failure-test-iterator", test_iterator, result);
}

static void
cb_error_test_iterator (CutTestIterator *test_iterator,
                        CutTestContext *test_context,
                        CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "error-test-iterator", test_iterator, result);
}

static void
cb_pending_test_iterator (CutTestIterator *test_iterator,
                          CutTestContext *test_context,
                          CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "pending-test-iterator", test_iterator, result);
}

static void
cb_notification_test_iterator (CutTestIterator *test_iterator,
                               CutTestContext *test_context,
                               CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "notification-test-iterator", test_iterator, result);
}

static void
cb_omission_test_iterator (CutTestIterator *test_iterator,
                           CutTestContext *test_context,
                           CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "omission-test-iterator", test_iterator, result);
}

static void
cb_start_test_iterator(CutTestIterator *test_iterator, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "start-test-iterator", test_iterator);
}

static void
cb_ready_test_iterator(CutTestIterator *test_iterator, guint n_tests,
                       gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "ready-test-iterator", test_iterator, n_tests);
}

static void
cb_complete_test_iterator(CutTestIterator *test_iterator, gpointer data)
{
    CutRunContext *context = data;

#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test_iterator,                 \
                                         G_CALLBACK(cb_ ## name),       \
                                         data)

    DISCONNECT(start_test);
    DISCONNECT(complete_test);
#undef DISCONNECT

#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test_iterator,                 \
                                         G_CALLBACK(cb_ ## name ## _test_iterator), \
                                         data)

    DISCONNECT(success);
    DISCONNECT(failure);
    DISCONNECT(error);
    DISCONNECT(pending);
    DISCONNECT(notification);
    DISCONNECT(omission);

    DISCONNECT(ready);
    DISCONNECT(start);
    DISCONNECT(complete);
#undef DISCONNECT

    g_signal_emit_by_name(context, "complete-test-iterator", test_iterator);
}

static void
prepare_test_iterator (CutRunContext *context, CutTestIterator *test_iterator)
{
    CutRunContextClass *klass;

    klass = CUT_RUN_CONTEXT_CLASS(cut_test_runner_parent_class);
    if (klass->prepare_test_iterator)
        klass->prepare_test_iterator(context, test_iterator);

#define CONNECT(name) \
    g_signal_connect(test_iterator, #name, G_CALLBACK(cb_ ## name), context)

    CONNECT(start_test);
    CONNECT(complete_test);
#undef CONNECT

#define CONNECT(name)                                                   \
    g_signal_connect(test_iterator, #name,                              \
                     G_CALLBACK(cb_ ## name ## _test_iterator), context)

    CONNECT(success);
    CONNECT(failure);
    CONNECT(error);
    CONNECT(pending);
    CONNECT(notification);
    CONNECT(omission);

    CONNECT(ready);
    CONNECT(start);
    CONNECT(complete);
#undef CONNECT
}

static void
cb_pass_assertion (CutTest *test, CutTestContext *test_context, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "pass-assertion", test, test_context);
}

static void
cb_success (CutTest *test, CutTestContext *test_context, CutTestResult *result,
            gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "success-test", test, test_context, result);
}

static void
cb_failure (CutTest *test, CutTestContext *test_context, CutTestResult *result,
            gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "failure-test", test, test_context, result);
}

static void
cb_error (CutTest *test, CutTestContext *test_context, CutTestResult *result,
          gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "error-test", test, test_context, result);
}

static void
cb_pending (CutTest *test, CutTestContext *test_context, CutTestResult *result,
            gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "pending-test", test, test_context, result);
}

static void
cb_notification (CutTest *test, CutTestContext *test_context,
                 CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "notification-test",
                          test, test_context, result);
}

static void
cb_omission (CutTest *test, CutTestContext *test_context,
             CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "omission-test", test, test_context, result);
}

static void
cb_complete (CutTest *test, gpointer data)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test,                          \
                                         G_CALLBACK(cb_ ## name),       \
                                         data)

    DISCONNECT(pass_assertion);
    DISCONNECT(success);
    DISCONNECT(failure);
    DISCONNECT(error);
    DISCONNECT(pending);
    DISCONNECT(notification);
    DISCONNECT(omission);
    DISCONNECT(complete);
#undef DISCONNECT

}

static void
prepare_test (CutRunContext *context, CutTest *test)
{
    CutRunContextClass *klass;

    klass = CUT_RUN_CONTEXT_CLASS(cut_test_runner_parent_class);
    if (klass->prepare_test)
        klass->prepare_test(context, test);

    g_signal_connect(test, "pass_assertion",
                     G_CALLBACK(cb_pass_assertion), context);
    g_signal_connect(test, "success", G_CALLBACK(cb_success), context);
    g_signal_connect(test, "failure", G_CALLBACK(cb_failure), context);
    g_signal_connect(test, "error", G_CALLBACK(cb_error), context);
    g_signal_connect(test, "pending", G_CALLBACK(cb_pending), context);
    g_signal_connect(test, "notification", G_CALLBACK(cb_notification), context);
    g_signal_connect(test, "omission", G_CALLBACK(cb_omission), context);
    g_signal_connect(test, "complete", G_CALLBACK(cb_complete), context);
}

static void
prepare_iterated_test (CutRunContext *context, CutIteratedTest *iterated_test)
{
    CutRunContextClass *klass;

    klass = CUT_RUN_CONTEXT_CLASS(cut_test_runner_parent_class);
    if (klass->prepare_iterated_test)
        klass->prepare_iterated_test(context, iterated_test);

    g_signal_connect(iterated_test, "pass_assertion",
                     G_CALLBACK(cb_pass_assertion), context);
    g_signal_connect(iterated_test, "success", G_CALLBACK(cb_success), context);
    g_signal_connect(iterated_test, "failure", G_CALLBACK(cb_failure), context);
    g_signal_connect(iterated_test, "error", G_CALLBACK(cb_error), context);
    g_signal_connect(iterated_test, "pending", G_CALLBACK(cb_pending), context);
    g_signal_connect(iterated_test, "notification",
                     G_CALLBACK(cb_notification), context);
    g_signal_connect(iterated_test, "omission",
                     G_CALLBACK(cb_omission), context);
    g_signal_connect(iterated_test, "complete",
                     G_CALLBACK(cb_complete), context);
}

static gboolean
runner_run (CutRunner *runner)
{
    CutRunContext *context;
    CutTestSuite *suite;
    gboolean success;

    context = CUT_RUN_CONTEXT(runner);
    suite = cut_run_context_get_test_suite(context);
    g_signal_emit_by_name(context, "start-run");
    success = cut_test_suite_run(suite, context);
    g_signal_emit_by_name(context, "complete-run", success);
    return success;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
