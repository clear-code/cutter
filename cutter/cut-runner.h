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

#ifndef __CUT_RUNNER_H__
#define __CUT_RUNNER_H__

#include <glib-object.h>

#include <cutter/cut-test-suite.h>
#include <cutter/cut-listener.h>

G_BEGIN_DECLS

#define CUT_TYPE_RUNNER            (cut_runner_get_type ())
#define CUT_RUNNER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_RUNNER, CutRunner))
#define CUT_RUNNER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_RUNNER, CutRunnerClass))
#define CUT_IS_RUNNER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_RUNNER))
#define CUT_IS_RUNNER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_RUNNER))
#define CUT_RUNNER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_RUNNER, CutRunnerClass))

typedef enum {
    CUT_ORDER_NONE_SPECIFIED,
    CUT_ORDER_NAME_ASCENDING,
    CUT_ORDER_NAME_DESCENDING
} CutOrder;

typedef struct _CutRunnerClass    CutRunnerClass;

struct _CutRunner
{
    GObject object;
};

struct _CutRunnerClass
{
    GObjectClass parent_class;

    void (*ready_test_suite)    (CutRunner      *runner,
                                 CutTestSuite   *test_suite,
                                 guint           n_test_cases,
                                 guint           n_tests);
    void (*start_test_suite)    (CutRunner      *runner,
                                 CutTestSuite   *test_suite);
    void (*ready_test_case)     (CutRunner      *runner,
                                 CutTestCase    *test_case,
                                 guint           n_tests);
    void (*start_test_case)     (CutRunner      *runner,
                                 CutTestCase    *test_case);
    void (*start_test)          (CutRunner      *runner,
                                 CutTest        *test,
                                 CutTestContext *test_context);

    void (*pass_test)           (CutRunner      *runner,
                                 CutTest        *test,
                                 CutTestContext *test_context);
    void (*success_test)        (CutRunner      *runner,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*failure_test)        (CutRunner      *runner,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*error_test)          (CutRunner      *runner,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*pending_test)        (CutRunner      *runner,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*notification_test)   (CutRunner      *runner,
                                 CutTest        *test,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*complete_test)       (CutRunner      *runner,
                                 CutTest        *test,
                                 CutTestContext *test_context);

    void (*success_test_case)   (CutRunner      *runner,
                                 CutTestCase    *test_case,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*failure_test_case)   (CutRunner      *runner,
                                 CutTestCase    *test_case,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*error_test_case)     (CutRunner      *runner,
                                 CutTestCase    *test_case,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*pending_test_case)   (CutRunner      *runner,
                                 CutTestCase    *test_case,
                                 CutTestContext *test_context,
                                 CutTestResult  *result);
    void (*notification_test_case) (CutRunner      *runner,
                                    CutTestCase    *test_case,
                                    CutTestContext *test_context,
                                    CutTestResult  *result);
    void (*complete_test_case)  (CutRunner      *runner,
                                 CutTestCase    *test_case);

    void (*complete_test_suite) (CutRunner      *runner,
                                 CutTestSuite   *test_suite);

    void (*crashed)             (CutRunner      *runner,
                                 const gchar    *stack_trace);
};

GType        cut_runner_get_type  (void) G_GNUC_CONST;

CutRunner    *cut_runner_new (void);

CutRunner    *cut_runner_copy                      (CutRunner   *runner);

void          cut_runner_set_test_directory        (CutRunner   *runner,
                                                    const gchar *directory);
const gchar  *cut_runner_get_test_directory        (CutRunner   *runner);
void          cut_runner_set_source_directory      (CutRunner   *runner,
                                                    const gchar *directory);
const gchar  *cut_runner_get_source_directory      (CutRunner   *runner);

void          cut_runner_set_multi_thread          (CutRunner   *runner,
                                                    gboolean     use_multi_thread);
gboolean      cut_runner_get_multi_thread          (CutRunner   *runner);

void          cut_runner_set_target_test_case_names(CutRunner   *runner,
                                                    const gchar **names);
const gchar **cut_runner_get_target_test_case_names(CutRunner   *runner);
void          cut_runner_set_target_test_names     (CutRunner   *runner,
                                                    const gchar **names);
const gchar **cut_runner_get_target_test_names     (CutRunner   *runner);

void          cut_runner_start_test                (CutRunner   *runner,
                                                    CutTest     *test);
void          cut_runner_start_test_case           (CutRunner   *runner,
                                                    CutTestCase *test_case);
void          cut_runner_start_test_suite          (CutRunner   *runner,
                                                    CutTestSuite *test_suite);

guint         cut_runner_get_n_tests               (CutRunner *runner);
guint         cut_runner_get_n_assertions          (CutRunner *runner);
guint         cut_runner_get_n_failures            (CutRunner *runner);
guint         cut_runner_get_n_errors              (CutRunner *runner);
guint         cut_runner_get_n_pendings            (CutRunner *runner);
guint         cut_runner_get_n_notifications       (CutRunner *runner);

const GList  *cut_runner_get_results               (CutRunner *runner);

gboolean      cut_runner_is_crashed                (CutRunner *runner);
const gchar  *cut_runner_get_stack_trace           (CutRunner *runner);

void          cut_runner_cancel                    (CutRunner *runner);
gboolean      cut_runner_is_canceled               (CutRunner *runner);

CutTestSuite *cut_runner_create_test_suite         (CutRunner *runner);
CutTestSuite *cut_runner_get_test_suite            (CutRunner *runner);
void          cut_runner_set_test_suite            (CutRunner *runner,
                                                    CutTestSuite *suite);

void          cut_runner_set_test_case_order       (CutRunner *runner,
                                                    CutOrder   order);
CutOrder      cut_runner_get_test_case_order       (CutRunner *runner);
GList        *cut_runner_sort_test_cases           (CutRunner *runner,
                                                    GList     *test_cases);

gboolean      cut_runner_run                       (CutRunner *runner);

void          cut_runner_add_listener              (CutRunner   *runner,
                                                    CutListener *listener);
void          cut_runner_remove_listener           (CutRunner   *runner,
                                                    CutListener *listener);

gchar        *cut_runner_build_source_filename     (CutRunner *runner,
                                                    const gchar *filename);


G_END_DECLS

#endif /* __CUT_RUNNER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
