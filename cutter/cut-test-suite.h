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

#ifndef __CUT_TEST_SUITE_H__
#define __CUT_TEST_SUITE_H__

#include <glib-object.h>

#include "cut-test-container.h"

G_BEGIN_DECLS

#define CUT_TYPE_TEST_SUITE            (cut_test_suite_get_type ())
#define CUT_TEST_SUITE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST_SUITE, CutTestSuite))
#define CUT_TEST_SUITE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST_SUITE, CutTestSuiteClass))
#define CUT_IS_TEST_SUITE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST_SUITE))
#define CUT_IS_TEST_SUITE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST_SUITE))
#define CUT_TEST_SUITE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST_SUITE, CutTestSuiteClass))

typedef struct _CutTestSuite      CutTestSuite;
typedef struct _CutTestSuiteClass CutTestSuiteClass;

struct _CutTestSuite
{
    CutTestContainer object;
};

struct _CutTestSuiteClass
{
    CutTestContainerClass parent_class;
};

GType           cut_test_suite_get_type      (void) G_GNUC_CONST;

CutTestSuite   *cut_test_suite_new           (void);
gboolean        cut_test_suite_run_test_case (CutTestSuite *suite,
                                              const gchar *name);


G_END_DECLS

#endif /* __CUT_TEST_SUITE_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
