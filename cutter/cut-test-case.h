/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifndef __CUT_TEST_CASE_H__
#define __CUT_TEST_CASE_H__

#include <glib-object.h>

#include "cut-test-container.h"

G_BEGIN_DECLS

#define CUT_TYPE_TEST_CASE            (cut_test_case_get_type ())
#define CUT_TEST_CASE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST_CASE, CutTestCase))
#define CUT_TEST_CASE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST_CASE, CutTestCaseClass))
#define CUT_IS_TEST_CASE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST_CASE))
#define CUT_IS_TEST_CASE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST_CASE))
#define CUT_TEST_CASE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST_CASE, CutTestCaseClass))

typedef struct _CutTestCaseClass CutTestCaseClass;

struct _CutTestCase
{
    CutTestContainer object;
};

struct _CutTestCaseClass
{
    CutTestContainerClass parent_class;

    void (*start_test)    (CutTestCase *test_case, CutTest *test);
    void (*complete_test) (CutTestCase *test_case, CutTest *test);
};

GType        cut_test_case_get_type       (void) G_GNUC_CONST;

CutTestCase *cut_test_case_new            (const gchar *name,
                                           CutSetupFunction setup_funcion,
                                           CutTearDownFunction teardown_function,
                                           CutGetCurrentTestContextFunction
                                           get_current_test_context_function,
                                           CutSetCurrentTestContextFunction
                                           set_current_test_context_function);
void         cut_test_case_add_test       (CutTestCase *test_case,
                                           CutTest *test);
guint        cut_test_case_get_test_count (CutTestCase *test_case);
gboolean     cut_test_case_run            (CutTestCase *test_case,
                                           CutContext  *context);
gboolean     cut_test_case_run_test       (CutTestCase *test_case,
                                           CutContext  *context,
                                           const gchar *name);
gboolean     cut_test_case_run_with_filter(CutTestCase *test_case,
                                           CutContext  *context,
                                           gchar **test_names);
gboolean     cut_test_case_has_test       (CutTestCase *test_case,
                                           const gchar *name);

G_END_DECLS

#endif /* __CUT_TEST_CASE_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
