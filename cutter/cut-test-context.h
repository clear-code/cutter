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

#ifndef __CUT_TEST_CONTEXT_H__
#define __CUT_TEST_CONTEXT_H__

#include <glib-object.h>

#include "cut-private.h"
#include "cut-assertion.h"
#include "cut-output.h"

G_BEGIN_DECLS

#define CUT_TYPE_TEST_CONTEXT                 (cut_test_context_get_type ())
#define CUT_TEST_CONTEXT(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST_CONTEXT, CutTestContext))
#define CUT_TEST_TEST_CONTEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST_CONTEXT, CutTestContextClass))
#define CUT_IS_TEST_CONTEXT(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST_CONTEXT))
#define CUT_IS_TEST_TEST_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST_CONTEXT))
#define CUT_TEST_TEST_CONTEXT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST_CONTEXT, CutTestContextClass))

typedef struct _CutTestContextClass    CutTestContextClass;

struct _CutTestContext
{
    GObject object;
};

struct _CutTestContextClass
{
    GObjectClass parent_class;
};

extern GPrivate *cut_test_context_private;

GType        cut_test_context_get_type  (void) G_GNUC_CONST;

CutTestContext  *cut_test_context_new (CutTestSuite *test_suite,
                                       CutTestCase  *test_case,
                                       CutTest      *test);

CutTestSuite *cut_test_context_get_test_suite (CutTestContext *context);
void          cut_test_context_set_test_suite (CutTestContext *context,
                                               CutTestSuite   *test_suite);
CutTestCase  *cut_test_context_get_test_case  (CutTestContext *context);
void          cut_test_context_set_test_case  (CutTestContext *context,
                                               CutTestCase    *test_case);
CutTest      *cut_test_context_get_test       (CutTestContext *context);
void          cut_test_context_set_test       (CutTestContext *context,
                                               CutTest        *test);

G_END_DECLS

#endif /* __CUT_TEST_CONTEXT_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
