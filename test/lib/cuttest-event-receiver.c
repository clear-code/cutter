/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
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

#include <glib.h>

#include "cuttest-event-receiver.h"

G_DEFINE_TYPE(CuttestEventReceiver, cuttest_event_receiver, CUT_TYPE_RUN_CONTEXT)

static CuttestReadyTestSuiteInfo *
ready_test_suite_info_new (CutTestSuite *test_suite, guint n_test_cases,
                           guint n_tests)
{
    CuttestReadyTestSuiteInfo *info;

    info = g_slice_new(CuttestReadyTestSuiteInfo);
    info->test_suite = test_suite;
    if (info->test_suite)
        g_object_ref(info->test_suite);
    info->n_test_cases = n_test_cases;
    info->n_tests = n_tests;

    return info;
}

static void
ready_test_suite_info_free (CuttestReadyTestSuiteInfo *info)
{
    if (info->test_suite)
        g_object_unref(info->test_suite);
    g_slice_free(CuttestReadyTestSuiteInfo, info);
}

static CuttestReadyTestCaseInfo *
ready_test_case_info_new (CutTestCase *test_case, guint n_tests)
{
    CuttestReadyTestCaseInfo *info;

    info = g_slice_new(CuttestReadyTestCaseInfo);
    info->test_case = test_case;
    if (info->test_case)
        g_object_ref(info->test_case);
    info->n_tests = n_tests;

    return info;
}

static void
ready_test_case_info_free (CuttestReadyTestCaseInfo *info)
{
    if (info->test_case)
        g_object_unref(info->test_case);
    g_slice_free(CuttestReadyTestCaseInfo, info);
}

static CuttestReadyTestIteratorInfo *
ready_test_iterator_info_new (CutTestIterator *test_iterator, guint n_tests)
{
    CuttestReadyTestIteratorInfo *info;

    info = g_slice_new(CuttestReadyTestIteratorInfo);
    info->test_iterator = test_iterator;
    if (info->test_iterator)
        g_object_ref(info->test_iterator);
    info->n_tests = n_tests;

    return info;
}

static void
ready_test_iterator_info_free (CuttestReadyTestIteratorInfo *info)
{
    if (info->test_iterator)
        g_object_unref(info->test_iterator);
    g_slice_free(CuttestReadyTestIteratorInfo, info);
}

static CuttestStartTestInfo *
start_test_info_new (CutTest *test, CutTestContext *test_context)
{
    CuttestStartTestInfo *info;

    info = g_slice_new(CuttestStartTestInfo);
    info->test = test;
    if (info->test)
        g_object_ref(info->test);
    info->test_context = test_context;
    if (info->test_context)
        g_object_ref(info->test_context);

    return info;
}

static void
start_test_info_free (CuttestStartTestInfo *info)
{
    if (info->test)
        g_object_unref(info->test);
    if (info->test_context)
        g_object_unref(info->test_context);
    g_slice_free(CuttestStartTestInfo, info);
}

static CuttestStartIteratedTestInfo *
start_iterated_test_info_new (CutIteratedTest *iterated_test,
                              CutTestContext *test_context)
{
    CuttestStartIteratedTestInfo *info;

    info = g_slice_new(CuttestStartIteratedTestInfo);
    info->iterated_test = iterated_test;
    if (info->iterated_test)
        g_object_ref(info->iterated_test);
    info->test_context = test_context;
    if (info->test_context)
        g_object_ref(info->test_context);

    return info;
}

static void
start_iterated_test_info_free (CuttestStartIteratedTestInfo *info)
{
    if (info->iterated_test)
        g_object_unref(info->iterated_test);
    if (info->test_context)
        g_object_unref(info->test_context);
    g_slice_free(CuttestStartIteratedTestInfo, info);
}

static CuttestPassAssertionInfo *
pass_assertion_info_new (CutTest *test, CutTestContext *test_context)
{
    CuttestPassAssertionInfo *info;

    info = g_slice_new(CuttestPassAssertionInfo);
    info->test = test;
    if (info->test)
        g_object_ref(info->test);
    info->test_context = test_context;
    if (info->test_context)
        g_object_ref(info->test_context);

    return info;
}

static void
pass_assertion_info_free (CuttestPassAssertionInfo *info)
{
    if (info->test)
        g_object_unref(info->test);
    if (info->test_context)
        g_object_unref(info->test_context);
    g_slice_free(CuttestPassAssertionInfo, info);
}

static CuttestCompleteIteratedTestInfo *
complete_iterated_test_info_new (CutIteratedTest *iterated_test,
                                 CutTestContext *test_context,
                                 gboolean success)
{
    CuttestCompleteIteratedTestInfo *info;

    info = g_slice_new(CuttestCompleteIteratedTestInfo);
    info->iterated_test = iterated_test;
    if (info->iterated_test)
        g_object_ref(info->iterated_test);
    info->test_context = test_context;
    if (info->test_context)
        g_object_ref(info->test_context);
    info->success = success;

    return info;
}

static void
complete_iterated_test_info_free (CuttestCompleteIteratedTestInfo *info)
{
    if (info->iterated_test)
        g_object_unref(info->iterated_test);
    if (info->test_context)
        g_object_unref(info->test_context);
    g_slice_free(CuttestCompleteIteratedTestInfo, info);
}

static CuttestCompleteTestInfo *
complete_test_info_new (CutTest *test, CutTestContext *test_context,
                        gboolean success)
{
    CuttestCompleteTestInfo *info;

    info = g_slice_new(CuttestCompleteTestInfo);
    info->test = test;
    if (info->test)
        g_object_ref(info->test);
    info->test_context = test_context;
    if (info->test_context)
        g_object_ref(info->test_context);
    info->success = success;

    return info;
}

static void
complete_test_info_free (CuttestCompleteTestInfo *info)
{
    if (info->test)
        g_object_unref(info->test);
    if (info->test_context)
        g_object_unref(info->test_context);
    g_slice_free(CuttestCompleteTestInfo, info);
}

static void
dispose (GObject *object)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(object);

    if (receiver->ready_test_suites) {
        g_list_foreach(receiver->ready_test_suites,
                       (GFunc)ready_test_suite_info_free, NULL);
        g_list_free(receiver->ready_test_suites);
        receiver->ready_test_suites = NULL;
    }

    if (receiver->start_test_suites) {
        g_list_foreach(receiver->start_test_suites, (GFunc)g_object_unref, NULL);
        g_list_free(receiver->start_test_suites);
        receiver->start_test_suites = NULL;
    }

    if (receiver->ready_test_cases) {
        g_list_foreach(receiver->ready_test_cases,
                       (GFunc)ready_test_case_info_free, NULL);
        g_list_free(receiver->ready_test_cases);
        receiver->ready_test_cases = NULL;
    }

    if (receiver->start_test_cases) {
        g_list_foreach(receiver->start_test_cases, (GFunc)g_object_unref, NULL);
        g_list_free(receiver->start_test_cases);
        receiver->start_test_cases = NULL;
    }

    if (receiver->ready_test_iterators) {
        g_list_foreach(receiver->ready_test_iterators,
                       (GFunc)ready_test_iterator_info_free, NULL);
        g_list_free(receiver->ready_test_iterators);
        receiver->ready_test_iterators = NULL;
    }

    if (receiver->start_test_iterators) {
        g_list_foreach(receiver->start_test_iterators,
                       (GFunc)g_object_unref, NULL);
        g_list_free(receiver->start_test_iterators);
        receiver->start_test_iterators = NULL;
    }

    if (receiver->start_tests) {
        g_list_foreach(receiver->start_tests,
                       (GFunc)start_test_info_free, NULL);
        g_list_free(receiver->start_tests);
        receiver->start_tests = NULL;
    }

    if (receiver->start_iterated_tests) {
        g_list_foreach(receiver->start_iterated_tests,
                       (GFunc)start_iterated_test_info_free, NULL);
        g_list_free(receiver->start_iterated_tests);
        receiver->start_iterated_tests = NULL;
    }

    if (receiver->pass_assertions) {
        g_list_foreach(receiver->pass_assertions,
                       (GFunc)pass_assertion_info_free, NULL);
        g_list_free(receiver->pass_assertions);
        receiver->pass_assertions = NULL;
    }

    if (receiver->complete_iterated_tests) {
        g_list_foreach(receiver->complete_iterated_tests,
                       (GFunc)complete_iterated_test_info_free, NULL);
        g_list_free(receiver->complete_iterated_tests);
        receiver->complete_iterated_tests = NULL;
    }

    if (receiver->complete_tests) {
        g_list_foreach(receiver->complete_tests,
                       (GFunc)complete_test_info_free, NULL);
        g_list_free(receiver->complete_tests);
        receiver->complete_tests = NULL;
    }

    if (receiver->complete_test_iterators) {
        g_list_foreach(receiver->complete_test_iterators,
                       (GFunc)g_object_unref, NULL);
        g_list_free(receiver->complete_test_iterators);
        receiver->complete_test_iterators = NULL;
    }

    if (receiver->complete_test_cases) {
        g_list_foreach(receiver->complete_test_cases,
                       (GFunc)g_object_unref, NULL);
        g_list_free(receiver->complete_test_cases);
        receiver->complete_test_cases = NULL;
    }

    if (receiver->complete_test_suites) {
        g_list_foreach(receiver->complete_test_suites,
                       (GFunc)g_object_unref, NULL);
        g_list_free(receiver->complete_test_suites);
        receiver->complete_test_suites = NULL;
    }

    if (receiver->complete_runs) {
        g_list_free(receiver->complete_runs);
        receiver->complete_runs = NULL;
    }

    if (receiver->crasheds) {
        g_list_foreach(receiver->crasheds, (GFunc)g_free, NULL);
        g_list_free(receiver->crasheds);
        receiver->crasheds = NULL;
    }

    G_OBJECT_CLASS(cuttest_event_receiver_parent_class)->dispose(object);
}

static void
start_run (CutRunContext *context)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    receiver->n_start_runs++;
}

static void
ready_test_suite (CutRunContext *context, CutTestSuite *test_suite,
                  guint n_test_cases, guint n_tests)
{
    CuttestEventReceiver *receiver;
    CuttestReadyTestSuiteInfo *info;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    info = ready_test_suite_info_new(test_suite, n_test_cases, n_tests);
    receiver->ready_test_suites =
        g_list_append(receiver->ready_test_suites, info);
}

static void
start_test_suite (CutRunContext *context, CutTestSuite *test_suite)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    receiver->start_test_suites =
        g_list_append(receiver->start_test_suites, g_object_ref(test_suite));
}

static void
ready_test_case (CutRunContext *context, CutTestCase *test_case, guint n_tests)
{
    CuttestEventReceiver *receiver;
    CuttestReadyTestCaseInfo *info;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    info = ready_test_case_info_new(test_case, n_tests);
    receiver->ready_test_cases = g_list_append(receiver->ready_test_cases, info);
}

static void
start_test_case (CutRunContext *context, CutTestCase *test_case)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    receiver->start_test_cases =
        g_list_append(receiver->start_test_cases, g_object_ref(test_case));
}

static void
ready_test_iterator (CutRunContext *context, CutTestIterator *test_iterator,
                     guint n_tests)
{
    CuttestEventReceiver *receiver;
    CuttestReadyTestIteratorInfo *info;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    info = ready_test_iterator_info_new(test_iterator, n_tests);
    receiver->ready_test_iterators =
        g_list_append(receiver->ready_test_iterators, info);
}

static void
start_test_iterator (CutRunContext *context, CutTestIterator *test_iterator)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    receiver->start_test_iterators =
        g_list_append(receiver->start_test_iterators,
                      g_object_ref(test_iterator));
}

static void
start_test (CutRunContext *context, CutTest *test, CutTestContext *test_context)
{
    CuttestEventReceiver *receiver;
    CuttestStartTestInfo *info;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    info = start_test_info_new(test, test_context);
    receiver->start_tests = g_list_append(receiver->start_tests, info);
}

static void
start_iterated_test (CutRunContext *context, CutIteratedTest *iterated_test,
                     CutTestContext *test_context)
{
    CuttestEventReceiver *receiver;
    CuttestStartIteratedTestInfo *info;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    info = start_iterated_test_info_new(iterated_test, test_context);
    receiver->start_iterated_tests =
        g_list_append(receiver->start_iterated_tests, info);
}

static void
pass_assertion (CutRunContext *context, CutTest *test,
                CutTestContext *test_context)
{
    CuttestEventReceiver *receiver;
    CuttestPassAssertionInfo *info;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    info = pass_assertion_info_new(test, test_context);
    receiver->pass_assertions = g_list_append(receiver->pass_assertions, info);
}

static void
complete_iterated_test (CutRunContext *context, CutIteratedTest *iterated_test,
                        CutTestContext *test_context, gboolean success)
{
    CuttestEventReceiver *receiver;
    CuttestCompleteIteratedTestInfo *info;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    info = complete_iterated_test_info_new(iterated_test, test_context, success);
    receiver->complete_iterated_tests =
        g_list_append(receiver->complete_iterated_tests, info);
}

static void
complete_test (CutRunContext *context, CutTest *test,
               CutTestContext *test_context, gboolean success)
{
    CuttestEventReceiver *receiver;
    CuttestCompleteTestInfo *info;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    info = complete_test_info_new(test, test_context, success);
    receiver->complete_tests = g_list_append(receiver->complete_tests, info);
}

static void
complete_test_iterator (CutRunContext *context, CutTestIterator *test_iterator,
                        gboolean success)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    receiver->complete_test_iterators =
        g_list_append(receiver->complete_test_iterators,
                      g_object_ref(test_iterator)); /* FIXME: use success */
}

static void
complete_test_case (CutRunContext *context, CutTestCase *test_case,
                    gboolean success)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    receiver->complete_test_cases =
        g_list_append(receiver->complete_test_cases, g_object_ref(test_case));
    /* FIXME: use success */
}

static void
complete_test_suite (CutRunContext *context, CutTestSuite *test_suite,
                     gboolean success)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    receiver->complete_test_suites =
        g_list_append(receiver->complete_test_suites, g_object_ref(test_suite));
    /* FIXME: use success */
}

static void
complete_run (CutRunContext *context, gboolean success)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    receiver->complete_runs = g_list_append(receiver->complete_runs,
                                            GINT_TO_POINTER(success));
}

static void
crashed (CutRunContext *context, const gchar *backtrace)
{
    CuttestEventReceiver *receiver;

    receiver = CUTTEST_EVENT_RECEIVER(context);
    receiver->crasheds = g_list_append(receiver->crasheds, g_strdup(backtrace));
}

static void
cuttest_event_receiver_class_init (CuttestEventReceiverClass *klass)
{
    GObjectClass *gobject_class;
    CutRunContextClass *run_context_class;

    gobject_class = G_OBJECT_CLASS(klass);
    run_context_class = CUT_RUN_CONTEXT_CLASS(klass);

    gobject_class->dispose = dispose;

    run_context_class->start_run = start_run;
    run_context_class->ready_test_suite = ready_test_suite;
    run_context_class->start_test_suite = start_test_suite;
    run_context_class->ready_test_case = ready_test_case;
    run_context_class->start_test_case = start_test_case;
    run_context_class->ready_test_iterator = ready_test_iterator;
    run_context_class->start_test_iterator = start_test_iterator;
    run_context_class->start_test = start_test;
    run_context_class->start_iterated_test = start_iterated_test;
    run_context_class->pass_assertion = pass_assertion;
    run_context_class->complete_iterated_test = complete_iterated_test;
    run_context_class->complete_test = complete_test;
    run_context_class->complete_test_iterator = complete_test_iterator;
    run_context_class->complete_test_case = complete_test_case;
    run_context_class->complete_test_suite = complete_test_suite;
    run_context_class->complete_run = complete_run;
    run_context_class->crashed = crashed;
}

static void
cuttest_event_receiver_init (CuttestEventReceiver *receiver)
{
    receiver->n_start_runs = 0;
    receiver->ready_test_suites = NULL;
    receiver->start_test_suites = NULL;
    receiver->ready_test_cases = NULL;
    receiver->start_test_cases = NULL;
    receiver->ready_test_iterators = NULL;
    receiver->start_test_iterators = NULL;
    receiver->start_tests = NULL;
    receiver->start_iterated_tests = NULL;
    receiver->complete_iterated_tests = NULL;
    receiver->complete_tests = NULL;
    receiver->complete_test_iterators = NULL;
    receiver->complete_test_cases = NULL;
    receiver->complete_test_suites = NULL;
    receiver->complete_runs = NULL;
    receiver->crasheds = NULL;
}

CutRunContext *
cuttest_event_receiver_new (void)
{
    return g_object_new(CUTTEST_TYPE_EVENT_RECEIVER, NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
