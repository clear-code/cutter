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

#ifndef __CUT_LISTENER_H__
#define __CUT_LISTENER_H__

#include <glib-object.h>
#include <cutter/cut-test-suite.h>

G_BEGIN_DECLS

#define CUT_TYPE_LISTENER            (cut_listener_get_type ())
#define CUT_LISTENER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_LISTENER, CutListener))
#define CUT_LISTENER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_LISTENER, CutListenerClass))
#define CUT_IS_LISTENER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_LISTENER))
#define CUT_IS_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_LISTENER))
#define CUT_LISTENER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_LISTENER, CutListenerClass))

typedef struct _CutListener CutListener;
typedef struct _CutListenerClass CutListenerClass;

struct _CutListenerClass
{
    GTypeInterface base_iface;

    void (*ready_test_suite)    (CutListener    *listener,
                                 CutTestCase    *test_case,
                                 guint           n_test_cases,
                                 guint           n_tests);
    void (*start_test_suite)    (CutListener    *listener,
                                 CutTestSuite   *test_suite);
    void (*ready_test_case)     (CutListener    *listener,
                                 CutTestCase    *test_case,
                                 guint           n_tests);
    void (*start_test_case)     (CutListener    *listener,
                                 CutTestCase    *test_case);
    void (*start_test)          (CutListener    *listener,
                                 CutTest        *test,
                                 CutTestContext *test_context);

    void (*pass)                (CutListener    *listener,
                                 CutTest        *test,
                                 CutTestContext *test_context);
    void (*success)             (CutListener    *listener,
                                 CutTest        *test);
    void (*failure)             (CutListener    *listener,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*error)               (CutListener    *listener,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*pending)             (CutListener    *listener,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*notification)        (CutListener    *listener,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);

    void (*complete_test)       (CutListener    *listener,
                                 CutTest        *test,
                                 CutTestContext *test_context);
    void (*complete_test_case)  (CutListener    *listener,
                                 CutTestCase    *test_case);
    void (*complete_test_suite) (CutListener    *listener,
                                 CutTestSuite   *test_suite);

    void (*crashed)             (CutListener    *listener,
                                 const gchar    *stack_trace);
};

GType        cut_listener_get_type  (void) G_GNUC_CONST;


void cut_listener_ready_test_suite   (CutListener    *listener,
                                      CutTestCase    *test_case,
                                      guint           n_test_cases,
                                      guint           n_tests);
void cut_listener_start_test_suite   (CutListener    *listener,
                                      CutTestSuite   *test_suite);
void cut_listener_ready_test_case    (CutListener    *listener,
                                      CutTestCase    *test_case,
                                      guint           n_tests);
void cut_listener_start_test_case    (CutListener    *listener,
                                      CutTestCase    *test_case);
void cut_listener_start_test         (CutListener    *listener,
                                      CutTest        *test,
                                      CutTestContext *test_context);
void cut_listener_pass               (CutListener    *listener,
                                      CutTest        *test,
                                      CutTestContext *test_context);
void cut_listener_success            (CutListener    *listener,
                                      CutTest        *test);
void cut_listener_failure            (CutListener    *listener,
                                      CutTest        *test,
                                      CutTestContext *test_context,
                                      CutTestResult  *result);
void cut_listener_error              (CutListener    *listener,
                                      CutTest        *test,
                                      CutTestContext *test_context,
                                      CutTestResult  *result);
void cut_listener_pending            (CutListener    *listener,
                                      CutTest        *test,
                                      CutTestContext *test_context,
                                      CutTestResult  *result);
void cut_listener_notification       (CutListener    *listener,
                                      CutTest        *test,
                                      CutTestContext *test_context,
                                      CutTestResult  *result);
void cut_listener_complete_test       (CutListener    *listener,
                                      CutTest        *test,
                                      CutTestContext *test_context);
void cut_listener_complete_test_case (CutListener    *listener,
                                      CutTestCase    *test_case);
void cut_listener_complete_test_suite(CutListener    *listener,
                                      CutTestSuite   *test_suite);
void cut_listener_crashed            (CutListener    *listener,
                                      const gchar    *stack_trace);

G_END_DECLS

#endif /* __CUT_LISTENER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
