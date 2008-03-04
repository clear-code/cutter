/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#include <glib.h>

#include "cut-listener.h"
#include "cut-test-result.h"
#include "cut-marshalers.h"

static void cut_listener_base_init (gpointer g_class);

GType
cut_listener_get_type (void)
{
    static GType type = 0;

    if (!type) {
        const GTypeInfo info = {
            sizeof (CutListenerClass),  /* class_size */
            cut_listener_base_init,	    /* base_init */
            NULL,			            /* base_finalize */
        };

        type = g_type_register_static (G_TYPE_INTERFACE, "CutListener",
                                       &info, 0);
    }

    return type;
}

static void
cut_listener_base_init (gpointer g_class)
{
    static gboolean initialized = FALSE;
    if (! initialized) {
        g_signal_new ("ready-test-suite",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, ready_test_suite),
                      NULL, NULL,
                      _cut_marshal_VOID__OBJECT_UINT_UINT,
                      G_TYPE_NONE, 3,
                      CUT_TYPE_TEST_SUITE, G_TYPE_UINT, G_TYPE_UINT);

        g_signal_new ("start-test-suite",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, start_test_suite),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1, CUT_TYPE_TEST_SUITE);

        g_signal_new ("ready-test-case",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, ready_test_case),
                      NULL, NULL,
                      _cut_marshal_VOID__OBJECT_UINT,
                      G_TYPE_NONE, 2, CUT_TYPE_TEST_CASE, G_TYPE_UINT);

        g_signal_new ("start-test-case",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, start_test_case),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);

        g_signal_new ("start-test",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, start_test),
                      NULL, NULL,
                      _cut_marshal_VOID__OBJECT_OBJECT,
                      G_TYPE_NONE, 2, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

        g_signal_new ("pass",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, pass),
                      NULL, NULL,
                      _cut_marshal_VOID__OBJECT_OBJECT,
                      G_TYPE_NONE, 2, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

        g_signal_new ("success",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, success),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1, CUT_TYPE_TEST);

        g_signal_new ("failure",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, failure),
                      NULL, NULL,
                      _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                      G_TYPE_NONE, 3,
                      CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                      CUT_TYPE_TEST_RESULT);

        g_signal_new ("error",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, error),
                      NULL, NULL,
                      _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                      G_TYPE_NONE, 3,
                      CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                      CUT_TYPE_TEST_RESULT);

        g_signal_new ("pending",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, pending),
                      NULL, NULL,
                      _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                      G_TYPE_NONE, 3,
                      CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                      CUT_TYPE_TEST_RESULT);

        g_signal_new ("notification",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, notification),
                      NULL, NULL,
                      _cut_marshal_VOID__OBJECT_OBJECT_OBJECT,
                      G_TYPE_NONE, 3,
                      CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT,
                      CUT_TYPE_TEST_RESULT);

        g_signal_new ("complete-test",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, complete_test),
                      NULL, NULL,
                      _cut_marshal_VOID__OBJECT_OBJECT,
                      G_TYPE_NONE, 2, CUT_TYPE_TEST, CUT_TYPE_TEST_CONTEXT);

        g_signal_new ("complete-test-case",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, complete_test_case),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);

        g_signal_new ("complete-test-suite",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, complete_test_suite),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1, CUT_TYPE_TEST_SUITE);

        g_signal_new ("crashed",
                      CUT_TYPE_LISTENER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (CutListenerClass, crashed),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__STRING,
                      G_TYPE_NONE, 1, G_TYPE_STRING);

        initialized = TRUE;
    }
}

void
cut_listener_ready_test_suite (CutListener    *listener,
                               CutTestCase    *test_case,
                               guint           n_test_cases,
                               guint           n_tests)
{
    if (CUT_LISTENER_GET_CLASS(listener)->ready_test_suite)
        CUT_LISTENER_GET_CLASS(listener)->ready_test_suite(listener, test_case, n_test_cases, n_tests);
}

void
cut_listener_start_test_suite (CutListener    *listener,
                               CutTestSuite   *test_suite)
{
    if (CUT_LISTENER_GET_CLASS(listener)->start_test_suite)
        CUT_LISTENER_GET_CLASS(listener)->start_test_suite(listener, test_suite);
}

void
cut_listener_ready_test_case (CutListener    *listener,
                              CutTestCase    *test_case,
                              guint           n_tests)
{
    if (CUT_LISTENER_GET_CLASS(listener)->ready_test_case)
        CUT_LISTENER_GET_CLASS(listener)->ready_test_case(listener, test_case, n_tests);
}

void
cut_listener_start_test_case (CutListener    *listener,
                              CutTestCase    *test_case)
{
    if (CUT_LISTENER_GET_CLASS(listener)->start_test_case)
        CUT_LISTENER_GET_CLASS(listener)->start_test_case(listener, test_case);
}

void
cut_listener_start_test (CutListener    *listener,
                         CutTest        *test,
                         CutTestContext *test_context)
{
    if (CUT_LISTENER_GET_CLASS(listener)->start_test)
        CUT_LISTENER_GET_CLASS(listener)->start_test(listener, test, test_context);
}

void
cut_listener_pass (CutListener    *listener,
                   CutTest        *test,
                   CutTestContext *test_context)
{
    if (CUT_LISTENER_GET_CLASS(listener)->pass)
        CUT_LISTENER_GET_CLASS(listener)->pass(listener, test, test_context);
}

void
cut_listener_success (CutListener    *listener,
                      CutTest        *test)
{
    if (CUT_LISTENER_GET_CLASS(listener)->success)
        CUT_LISTENER_GET_CLASS(listener)->success(listener, test);
}

void
cut_listener_failure (CutListener    *listener,
                      CutTest        *test,
                      CutTestContext *test_context,
                      CutTestResult  *result)
{
    if (CUT_LISTENER_GET_CLASS(listener)->failure)
        CUT_LISTENER_GET_CLASS(listener)->failure(listener, test, test_context, result);
}

void
cut_listener_error (CutListener    *listener,
                    CutTest        *test,
                    CutTestContext *test_context,
                    CutTestResult  *result)
{
    if (CUT_LISTENER_GET_CLASS(listener)->error)
        CUT_LISTENER_GET_CLASS(listener)->error(listener, test, test_context, result);
}

void
cut_listener_pending (CutListener    *listener,
                      CutTest        *test,
                      CutTestContext *test_context,
                      CutTestResult  *result)
{
    if (CUT_LISTENER_GET_CLASS(listener)->pending)
        CUT_LISTENER_GET_CLASS(listener)->pending(listener, test, test_context, result);
}

void
cut_listener_notification (CutListener    *listener,
                           CutTest        *test,
                           CutTestContext *test_context,
                           CutTestResult  *result)
{
    if (CUT_LISTENER_GET_CLASS(listener)->notification)
        CUT_LISTENER_GET_CLASS(listener)->notification(listener, test, test_context, result);
}

void
cut_listener_complete_test (CutListener    *listener,
                            CutTest        *test,
                            CutTestContext *test_context)
{
    if (CUT_LISTENER_GET_CLASS(listener)->complete_test)
        CUT_LISTENER_GET_CLASS(listener)->complete_test(listener, test, test_context);
}

void
cut_listener_complete_test_case (CutListener    *listener,
                                 CutTestCase    *test_case)
{
    if (CUT_LISTENER_GET_CLASS(listener)->complete_test_case)
        CUT_LISTENER_GET_CLASS(listener)->complete_test_case(listener, test_case);
}

void
cut_listener_complete_test_suite(CutListener    *listener,
                                 CutTestSuite   *test_suite)
{
    if (CUT_LISTENER_GET_CLASS(listener)->complete_test_suite)
        CUT_LISTENER_GET_CLASS(listener)->complete_test_suite(listener, test_suite);
}

void
cut_listener_crashed (CutListener    *listener,
                      const gchar    *stack_trace)
{
    if (CUT_LISTENER_GET_CLASS(listener)->crashed)
        CUT_LISTENER_GET_CLASS(listener)->crashed(listener, stack_trace);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
