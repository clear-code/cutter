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

#ifndef __CUT_RUN_CONTEXT_H__
#define __CUT_RUN_CONTEXT_H__

#include <glib-object.h>

#include <cutter/cut-test-suite.h>
#include <cutter/cut-private.h>

G_BEGIN_DECLS

#define CUT_TYPE_RUN_CONTEXT            (cut_run_context_get_type ())
#define CUT_RUN_CONTEXT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_RUN_CONTEXT, CutRunContext))
#define CUT_RUN_CONTEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_RUN_CONTEXT, CutRunContextClass))
#define CUT_IS_RUN_CONTEXT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_RUN_CONTEXT))
#define CUT_IS_RUN_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_RUN_CONTEXT))
#define CUT_RUN_CONTEXT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_RUN_CONTEXT, CutRunContextClass))

typedef enum {
    CUT_ORDER_NONE_SPECIFIED,
    CUT_ORDER_NAME_ASCENDING,
    CUT_ORDER_NAME_DESCENDING
} CutOrder;

typedef struct _CutRunContextClass    CutRunContextClass;

struct _CutRunContext
{
    GObject object;
};

struct _CutRunContextClass
{
    GObjectClass parent_class;

    void (*ready_test_suite)    (CutRunContext  *context,
                                 CutTestSuite   *test_suite,
                                 guint           n_test_cases,
                                 guint           n_tests);
    void (*start_test_suite)    (CutRunContext  *context,
                                 CutTestSuite   *test_suite);
    void (*ready_test_case)     (CutRunContext  *context,
                                 CutTestCase    *test_case,
                                 guint           n_tests);
    void (*start_test_case)     (CutRunContext  *context,
                                 CutTestCase    *test_case);
    void (*start_test)          (CutRunContext  *context,
                                 CutTest        *test,
                                 CutTestContext *test_context);

    void (*pass_test)           (CutRunContext  *context,
                                 CutTest        *test,
                                 CutTestContext *test_context);
    void (*success_test)        (CutRunContext  *context,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*failure_test)        (CutRunContext  *context,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*error_test)          (CutRunContext  *context,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*pending_test)        (CutRunContext  *context,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*notification_test)   (CutRunContext  *context,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*omission_test)       (CutRunContext  *context,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*complete_test)       (CutRunContext  *context,
                                 CutTest        *test,
                                 CutTestContext *test_context);

    void (*success_test_case)   (CutRunContext  *context,
                                 CutTestCase    *test_case,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*failure_test_case)   (CutRunContext  *context,
                                 CutTestCase    *test_case,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*error_test_case)     (CutRunContext  *context,
                                 CutTestCase    *test_case,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*pending_test_case)   (CutRunContext  *context,
                                 CutTestCase    *test_case,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*notification_test_case) (CutRunContext  *context,
                                    CutTestCase    *test_case,
                                    CutTestContext *test_context,
                                    CutTestResult  *result);
    void (*omission_test_case)  (CutRunContext  *context,
                                 CutTestCase    *test_case,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*complete_test_case)  (CutRunContext  *context,
                                 CutTestCase    *test_case);

    void (*complete_test_suite) (CutRunContext  *context,
                                 CutTestSuite   *test_suite);

    void (*crashed)             (CutRunContext  *context,
                                 const gchar    *stack_trace);
};

GType          cut_run_context_get_type  (void) G_GNUC_CONST;

void           cut_run_context_set_test_directory   (CutRunContext *context,
                                                     const gchar   *directory);
const gchar   *cut_run_context_get_test_directory   (CutRunContext *context);
void           cut_run_context_set_source_directory (CutRunContext *context,
                                                     const gchar   *directory);
const gchar   *cut_run_context_get_source_directory (CutRunContext *context);

void           cut_run_context_set_multi_thread     (CutRunContext *context,
                                                     gboolean       use_multi_thread);
gboolean       cut_run_context_get_multi_thread     (CutRunContext *context);
gboolean       cut_run_context_is_multi_thread      (CutRunContext *context);

void           cut_run_context_set_exclude_files    (CutRunContext *context,
                                                     gchar        **filenames);
gchar        **cut_run_context_get_exclude_files    (CutRunContext *context);
void           cut_run_context_set_exclude_dirs    (CutRunContext *context,
                                                     gchar        **directory_names);
gchar        **cut_run_context_get_exclude_dirs    (CutRunContext *context);
void           cut_run_context_set_target_test_case_names(CutRunContext *context,
                                                          gchar      **names);
gchar        **cut_run_context_get_target_test_case_names(CutRunContext *context);
void           cut_run_context_set_target_test_names(CutRunContext *context,
                                                     gchar        **names);
gchar        **cut_run_context_get_target_test_names(CutRunContext *context);

void           cut_run_context_prepare_test         (CutRunContext *context,
                                                     CutTest       *test);
void           cut_run_context_prepare_test_case    (CutRunContext *context,
                                                     CutTestCase   *test_case);
void           cut_run_context_prepare_test_suite   (CutRunContext *context,
                                                     CutTestSuite  *test_suite);

guint          cut_run_context_get_n_tests          (CutRunContext *context);
guint          cut_run_context_get_n_successes      (CutRunContext *context);
guint          cut_run_context_get_n_assertions     (CutRunContext *context);
guint          cut_run_context_get_n_failures       (CutRunContext *context);
guint          cut_run_context_get_n_errors         (CutRunContext *context);
guint          cut_run_context_get_n_pendings       (CutRunContext *context);
guint          cut_run_context_get_n_notifications  (CutRunContext *context);
guint          cut_run_context_get_n_omissions      (CutRunContext *context);

const GList   *cut_run_context_get_results          (CutRunContext *context);

gboolean       cut_run_context_is_crashed           (CutRunContext *context);
const gchar   *cut_run_context_get_stack_trace      (CutRunContext *context);

void           cut_run_context_cancel               (CutRunContext *context);
gboolean       cut_run_context_is_canceled          (CutRunContext *context);

CutTestSuite  *cut_run_context_create_test_suite    (CutRunContext *context);
CutTestSuite  *cut_run_context_get_test_suite       (CutRunContext *context);
void           cut_run_context_set_test_suite       (CutRunContext *context,
                                                     CutTestSuite  *suite);

void           cut_run_context_set_test_case_order  (CutRunContext *context,
                                                     CutOrder       order);
CutOrder       cut_run_context_get_test_case_order  (CutRunContext *context);
GList         *cut_run_context_sort_test_cases      (CutRunContext *context,
                                                     GList         *test_cases);

void           cut_run_context_add_listener         (CutRunContext *context,
                                                     CutListener   *listener);
void           cut_run_context_remove_listener      (CutRunContext *context,
                                                     CutListener   *listener);

void           cut_run_context_attach_listeners     (CutRunContext *context);
void           cut_run_context_detach_listeners     (CutRunContext *context);

gchar         *cut_run_context_build_source_filename(CutRunContext *context,
                                                     const gchar   *filename);

gboolean       cut_run_context_start                (CutRunContext *context);


G_END_DECLS

#endif /* __CUT_RUN_CONTEXT_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
