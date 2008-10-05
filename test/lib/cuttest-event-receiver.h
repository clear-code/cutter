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

#ifndef __CUTTEST_EVENT_RECEIVER_H__
#define __CUTTEST_EVENT_RECEIVER_H__

#include <glib-object.h>
#include <cutter/cut-run-context.h>

G_BEGIN_DECLS

#define CUTTEST_TYPE_EVENT_RECEIVER            (cuttest_event_receiver_get_type ())
#define CUTTEST_EVENT_RECEIVER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUTTEST_TYPE_EVENT_RECEIVER, CuttestEventReceiver))
#define CUTTEST_EVENT_RECEIVER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUTTEST_TYPE_EVENT_RECEIVER, CuttestEventReceiverClass))
#define CUTTEST_IS_EVENT_RECEIVER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUTTEST_TYPE_EVENT_RECEIVER))
#define CUTTEST_IS_EVENT_RECEIVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUTTEST_TYPE_EVENT_RECEIVER))
#define CUTTEST_EVENT_RECEIVER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUTTEST_TYPE_EVENT_RECEIVER, CuttestEventReceiverClass))

typedef struct _CuttestEventReceiver      CuttestEventReceiver;
typedef struct _CuttestEventReceiverClass CuttestEventReceiverClass;

struct _CuttestEventReceiver
{
    CutRunContext object;

    guint n_start_runs;
    GList *ready_test_suites;
    GList *start_test_suites;
    GList *ready_test_cases;
    GList *start_test_cases;
    GList *ready_test_iterators;
    GList *start_test_iterators;
    GList *start_tests;
    GList *start_iterated_tests;
    GList *pass_assertions;
    GList *complete_iterated_tests;
    GList *complete_tests;
    GList *complete_test_iterators;
    GList *complete_test_cases;
    GList *complete_test_suites;
    GList *complete_runs;
    GList *crasheds;
};

struct _CuttestEventReceiverClass
{
    CutRunContextClass parent_class;
};

typedef struct _CuttestReadyTestSuiteInfo
{
    CutTestSuite *test_suite;
    guint n_test_cases;
    guint n_tests;
} CuttestReadyTestSuiteInfo;

typedef struct _CuttestReadyTestCaseInfo
{
    CutTestCase *test_case;
    guint n_tests;
} CuttestReadyTestCaseInfo;

typedef struct _CuttestReadyTestIteratorInfo
{
    CutTestIterator *test_iterator;
    guint n_tests;
} CuttestReadyTestIteratorInfo;

typedef struct _CuttestStartTestInfo
{
    CutTest *test;
    CutTestContext *test_context;
} CuttestStartTestInfo;

typedef struct _CuttestStartIteratedTestInfo
{
    CutIteratedTest *iterated_test;
    CutTestContext *test_context;
} CuttestStartIteratedTestInfo;

typedef struct _CuttestPassAssertionInfo
{
    CutTest *test;
    CutTestContext *test_context;
} CuttestPassAssertionInfo;

typedef struct _CuttestCompleteIteratedTestInfo
{
    CutIteratedTest *iterated_test;
    CutTestContext *test_context;
    gboolean success;
} CuttestCompleteIteratedTestInfo;

typedef struct _CuttestCompleteTestInfo
{
    CutTest *test;
    CutTestContext *test_context;
    gboolean success;
} CuttestCompleteTestInfo;


GType                 cuttest_event_receiver_get_type  (void) G_GNUC_CONST;

CutRunContext        *cuttest_event_receiver_new       (void);

G_END_DECLS

#endif /* __CUTTEST_EVENT_RECEIVER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
