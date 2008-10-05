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

static gboolean runner_run (CutRunner *runner);

static void
cut_test_runner_class_init (CutTestRunnerClass *klass)
{
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
cb_start_test (CutTest *test, CutTestContext *test_context, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "start-test", test, test_context);
}

static void
cb_pass_assertion_test (CutTest *test, CutTestContext *test_context,
                        gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "pass-assertion", test, test_context);
}

static void
cb_success_test (CutTest *test, CutTestContext *test_context,
                 CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "success-test", test, test_context, result);
}

static void
cb_failure_test (CutTest *test, CutTestContext *test_context,
                 CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "failure-test", test, test_context, result);
}

static void
cb_error_test (CutTest *test, CutTestContext *test_context,
               CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "error-test", test, test_context, result);
}

static void
cb_pending_test (CutTest *test, CutTestContext *test_context,
                 CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "pending-test", test, test_context, result);
}

static void
cb_notification_test (CutTest *test, CutTestContext *test_context,
                      CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "notification-test",
                          test, test_context, result);
}

static void
cb_omission_test (CutTest *test, CutTestContext *test_context,
                  CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "omission-test", test, test_context, result);
}

static void
cb_complete_test (CutTest *test, CutTestContext *test_context,
                  gboolean success, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "complete-test", test, test_context, success);
}

static void
connect_to_test (CutRunContext *context, CutTest *test)
{
#define CONNECT(name)                                                   \
    g_signal_connect(test, #name,                                       \
                     G_CALLBACK(cb_ ## name ## _test), context)

    CONNECT(start);
    CONNECT(pass_assertion);
    CONNECT(success);
    CONNECT(failure);
    CONNECT(error);
    CONNECT(pending);
    CONNECT(notification);
    CONNECT(omission);
    CONNECT(complete);
#undef CONNECT
}

static void
cb_start_test_test_case (CutTestCase *test_case, CutTest *test,
                         CutTestContext *test_context, gpointer data)
{
    CutRunContext *context = data;

    connect_to_test(context, test);
}

static void
cb_complete_test_test_case (CutTestCase *test_case, CutTest *test,
                            CutTestContext *test_context, gboolean success,
                            gpointer data)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test,                          \
                                         G_CALLBACK(cb_ ##              \
                                                    name ##             \
                                                    _test),             \
                                         data)

    DISCONNECT(start);
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
cb_start_test_case (CutTestCase *test_case, CutTestContext *test_context,
                    gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "start-test-case", test_case);
}

static void
cb_ready_test_case (CutTestCase *test_case, guint n_tests, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "ready-test-case", test_case, n_tests);
}

static void
cb_complete_test_case (CutTestCase *test_case, CutTestContext *test_context,
                       gboolean success, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "complete-test-case", test_case, success);
}

static void
cb_ready_test_suite (CutTestSuite *test_suite, guint n_test_cases,
                     guint n_tests, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "ready-test-suite", test_suite,
                          n_test_cases, n_tests);
}

static void
cb_start_test_suite (CutTestSuite *test_suite, CutTestContext *test_context,
                     gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "start-test-suite", test_suite);
}

static void
cb_crashed_test_suite (CutTestSuite *test_suite, const gchar *backtrace,
                       gpointer data)
{
    CutRunContext *context = data;

    cut_run_context_crash(context, backtrace);
    g_signal_emit_by_name(context, "crashed", backtrace);
}

static void
cb_start_iterated_test (CutIteratedTest *iterated_test,
                        CutTestContext *test_context, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "start-iterated-test",
                          iterated_test, test_context);
}

static void
cb_pass_assertion_iterated_test (CutIteratedTest *iterated_test,
                                 CutTestContext *test_context, gpointer data)
{
    cb_pass_assertion_test(CUT_TEST(iterated_test), test_context, data);
}

static void
cb_success_iterated_test (CutIteratedTest *iterated_test,
                          CutTestContext *test_context,
                          CutTestResult *result, gpointer data)
{
    cb_success_test(CUT_TEST(iterated_test), test_context, result, data);
}

static void
cb_failure_iterated_test (CutIteratedTest *iterated_test,
                          CutTestContext *test_context,
                          CutTestResult *result, gpointer data)
{
    cb_failure_test(CUT_TEST(iterated_test), test_context, result, data);
}

static void
cb_error_iterated_test (CutIteratedTest *iterated_test,
                        CutTestContext *test_context,
                        CutTestResult *result, gpointer data)
{
    cb_error_test(CUT_TEST(iterated_test), test_context, result, data);
}

static void
cb_pending_iterated_test (CutIteratedTest *iterated_test,
                          CutTestContext *test_context,
                          CutTestResult *result, gpointer data)
{
    cb_pending_test(CUT_TEST(iterated_test), test_context, result, data);
}

static void
cb_notification_iterated_test (CutIteratedTest *iterated_test,
                               CutTestContext *test_context,
                               CutTestResult *result, gpointer data)
{
    cb_notification_test(CUT_TEST(iterated_test), test_context, result, data);
}

static void
cb_omission_iterated_test (CutIteratedTest *iterated_test,
                           CutTestContext *test_context,
                           CutTestResult *result, gpointer data)
{
    cb_omission_test(CUT_TEST(iterated_test), test_context, result, data);
}

static void
cb_complete_iterated_test (CutIteratedTest *iterated_test,
                           CutTestContext *test_context, gboolean success,
                           gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "complete-iterated-test",
                          iterated_test, test_context, success);
}

static void
cb_start_iterated_test_test_iterator (CutTestIterator *test_iterator,
                                      CutIteratedTest *iterated_test,
                                      CutTestContext *test_context,
                                      gpointer data)
{
    CutRunContext *context = data;

#define CONNECT(name)                                                   \
    g_signal_connect(iterated_test, #name,                              \
                     G_CALLBACK(cb_ ## name ## _iterated_test),         \
                     context)

    CONNECT(start);
    CONNECT(complete);

    CONNECT(pass_assertion);
    CONNECT(success);
    CONNECT(failure);
    CONNECT(error);
    CONNECT(pending);
    CONNECT(notification);
    CONNECT(omission);
#undef CONNECT
}

static void
cb_complete_iterated_test_test_iterator (CutTestIterator *test_iterator,
                                         CutIteratedTest *iterated_test,
                                         CutTestContext *test_context,
                                         gboolean success,
                                         gpointer data)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(iterated_test,                 \
                                         G_CALLBACK(cb_ ##              \
                                                    name ##             \
                                                    _iterated_test),    \
                                         data)

    DISCONNECT(start);
    DISCONNECT(complete);

    DISCONNECT(pass_assertion);
    DISCONNECT(success);
    DISCONNECT(failure);
    DISCONNECT(error);
    DISCONNECT(pending);
    DISCONNECT(notification);
    DISCONNECT(omission);
#undef DISCONNECT
}

static void
cb_success_test_iterator (CutTestIterator *test_iterator,
                          CutTestContext *test_context,
                          CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "success-test-iterator",
                          test_iterator, result);
}

static void
cb_failure_test_iterator (CutTestIterator *test_iterator,
                          CutTestContext *test_context,
                          CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "failure-test-iterator",
                          test_iterator, result);
}

static void
cb_error_test_iterator (CutTestIterator *test_iterator,
                        CutTestContext *test_context,
                        CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "error-test-iterator",
                          test_iterator, result);
}

static void
cb_pending_test_iterator (CutTestIterator *test_iterator,
                          CutTestContext *test_context,
                          CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "pending-test-iterator",
                          test_iterator, result);
}

static void
cb_notification_test_iterator (CutTestIterator *test_iterator,
                               CutTestContext *test_context,
                               CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "notification-test-iterator",
                          test_iterator, result);
}

static void
cb_omission_test_iterator (CutTestIterator *test_iterator,
                           CutTestContext *test_context,
                           CutTestResult *result, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "omission-test-iterator",
                          test_iterator, result);
}

static void
cb_start_test_iterator (CutTestIterator *test_iterator,
                        CutTestContext *test_context, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "start-test-iterator", test_iterator);
}

static void
cb_ready_test_iterator (CutTestIterator *test_iterator, guint n_tests,
                        gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "ready-test-iterator",
                          test_iterator, n_tests);
}

static void
cb_complete_test_iterator (CutTestIterator *test_iterator,
                           CutTestContext *test_context,
                           gboolean success, gpointer data)
{
    CutRunContext *context = data;

    g_signal_emit_by_name(context, "complete-test-iterator",
                          test_iterator, success);
}

static void
connect_to_test_iterator (CutRunContext *context, CutTestIterator *test_iterator)
{

#define CONNECT(name)                                                   \
    g_signal_connect(test_iterator, #name,                              \
                     G_CALLBACK(cb_ ## name ## _test_iterator), context)

    CONNECT(start_iterated_test);
    CONNECT(complete_iterated_test);

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
cb_start_test_iterator_test_case (CutTestCase *test_case,
                                  CutTestIterator *test_iterator,
                                  CutTestContext *test_context,
                                  gpointer data)
{
    CutRunContext *context = data;

    connect_to_test_iterator(context, test_iterator);
}

static void
cb_complete_test_iterator_test_case (CutTestCase *test_case,
                                     CutTestIterator *test_iterator,
                                     CutTestContext *test_context,
                                     gboolean success,
                                     gpointer data)
{
    CutRunContext *context = data;

#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test_iterator,                 \
                                         G_CALLBACK(cb_ ##              \
                                                    name ##             \
                                                    _test_iterator),    \
                                         context)

    DISCONNECT(start_iterated_test);
    DISCONNECT(complete_iterated_test);

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
}

static void
connect_to_test_case (CutRunContext *context, CutTestCase *test_case)
{
#define CONNECT(name)                                                   \
    g_signal_connect(test_case, #name,                                  \
                     G_CALLBACK(cb_ ## name ## _test_case), context)

    CONNECT(start_test);
    CONNECT(complete_test);
    CONNECT(start_test_iterator);
    CONNECT(complete_test_iterator);

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
cb_start_test_case_test_suite (CutTestSuite *test_suite, CutTestCase *test_case,
                               gpointer data)
{
    CutRunContext *context = data;

    connect_to_test_case(context, test_case);
}

static void
cb_complete_test_case_test_suite (CutTestSuite *test_suite,
                                  CutTestCase *test_case,
                                  gboolean success,
                                  gpointer data)
{
    CutRunContext *context = data;

#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(test_case,                     \
                                         G_CALLBACK(cb_ ##              \
                                                    name ##             \
                                                    _test_case),        \
                                         context)

    DISCONNECT(start_test);
    DISCONNECT(complete_test);
    DISCONNECT(start_test_iterator);
    DISCONNECT(complete_test_iterator);

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
}

static void
cb_complete_test_suite (CutTestSuite *test_suite, CutTestContext *test_context,
                        gboolean success, gpointer data)
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

    DISCONNECT(start_test_case);
    DISCONNECT(complete_test_case);
#undef DISCONNECT

    g_signal_emit_by_name(context, "complete-test-suite", test_suite, success);
}

static void
connect_to_test_suite (CutRunContext *context, CutTestSuite *test_suite)
{
#define CONNECT(name)                                                \
    g_signal_connect(test_suite, #name,                              \
                     G_CALLBACK(cb_ ## name ## _test_suite),         \
                     context)

    CONNECT(ready);
    CONNECT(start);
    CONNECT(complete);
    CONNECT(crashed);

    CONNECT(start_test_case);
    CONNECT(complete_test_case);
#undef CONNECT
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
    success = cut_test_runner_run_test_suite(CUT_TEST_RUNNER(runner), suite);
    success = cut_run_context_emit_complete_run(context, success);
    return success;
}

gboolean
cut_test_runner_run_test (CutTestRunner *runner, CutTest *test,
                          CutTestContext *test_context)
{
    CutRunContext *context;

    context = CUT_RUN_CONTEXT(runner);
    connect_to_test(context, test);
    return cut_test_run(test, test_context, context);
}

gboolean
cut_test_runner_run_test_case (CutTestRunner *runner, CutTestCase *test_case)
{
    CutRunContext *context;

    context = CUT_RUN_CONTEXT(runner);
    connect_to_test_case(context, test_case);
    return cut_test_case_run(test_case, context);
}

gboolean
cut_test_runner_run_test_iterator (CutTestRunner *runner,
                                   CutTestIterator *test_iterator,
                                   CutTestContext *test_context)
{
    CutRunContext *context;

    context = CUT_RUN_CONTEXT(runner);
    connect_to_test_iterator(context, test_iterator);
    return cut_test_run(CUT_TEST(test_iterator), test_context, context);
}

gboolean
cut_test_runner_run_test_suite (CutTestRunner *runner, CutTestSuite *test_suite)
{
    CutRunContext *context;

    context = CUT_RUN_CONTEXT(runner);
    connect_to_test_suite(context, test_suite);
    return cut_test_suite_run(test_suite, context);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
