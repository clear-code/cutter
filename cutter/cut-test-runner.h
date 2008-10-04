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

#ifndef __CUT_TEST_RUNNER_H__
#define __CUT_TEST_RUNNER_H__

#include <glib-object.h>

#include <cutter/cut-run-context.h>

G_BEGIN_DECLS

#define CUT_TYPE_TEST_RUNNER            (cut_test_runner_get_type ())
#define CUT_TEST_RUNNER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST_RUNNER, CutTestRunner))
#define CUT_TEST_RUNNER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST_RUNNER, CutTestRunnerClass))
#define CUT_IS_TEST_RUNNER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST_RUNNER))
#define CUT_IS_TEST_RUNNER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST_RUNNER))
#define CUT_TEST_RUNNER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST_RUNNER, CutTestRunnerClass))

typedef struct _CutTestRunnerClass    CutTestRunnerClass;

struct _CutTestRunner
{
    CutRunContext object;
};

struct _CutTestRunnerClass
{
    CutRunContextClass parent_class;
};

GType          cut_test_runner_get_type         (void) G_GNUC_CONST;

CutRunContext *cut_test_runner_new              (void);

gboolean       cut_test_runner_run_test         (CutTestRunner  *runner,
                                                 CutTest        *test,
                                                 CutTestContext *test_context);
gboolean       cut_test_runner_run_test_case    (CutTestRunner  *runner,
                                                 CutTestCase    *test_case);
gboolean       cut_test_runner_run_test_iterator(CutTestRunner  *runner,
                                                 CutTestIterator *test_iterator,
                                                 CutTestContext *test_context);
gboolean       cut_test_runner_run_test_suite   (CutTestRunner  *runner,
                                                 CutTestSuite   *test_suite);

G_END_DECLS

#endif /* __CUT_TEST_RUNNER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
