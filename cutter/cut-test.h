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

#ifndef __CUT_TEST_H__
#define __CUT_TEST_H__

#include <glib-object.h>

#include "cut-private.h"
#include "cut-assertions.h"
#include "cut-test-context.h"
#include "cut-test-result.h"

G_BEGIN_DECLS

#define CUT_TYPE_TEST            (cut_test_get_type ())
#define CUT_TEST(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST, CutTest))
#define CUT_TEST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST, CutTestClass))
#define CUT_IS_TEST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST))
#define CUT_IS_TEST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST))
#define CUT_TEST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST, CutTestClass))

typedef struct _CutTestClass    CutTestClass;

typedef void    (*CutTestFunction)     (void);
typedef void    (*CutSetupFunction)    (void);
typedef void    (*CutTearDownFunction) (void);
typedef CutTestContext *(*CutGetCurrentTestContextFunction) (void);
typedef void            (*CutSetCurrentTestContextFunction) (CutTestContext *context);

struct _CutTest
{
    GObject object;
};

struct _CutTestClass
{
    GObjectClass parent_class;

    void (*start)          (CutTest        *test);
    void (*pass_assertion) (CutTest        *test,
                            CutTestContext *context);
    void (*success)        (CutTest        *test);
    void (*failure)        (CutTest        *test,
                            CutTestContext *context,
                            CutTestResult  *result);
    void (*error)          (CutTest        *test,
                            CutTestContext *context,
                            CutTestResult  *result);
    void (*pending)        (CutTest        *test,
                            CutTestContext *context,
                            CutTestResult  *result);
    void (*notification)   (CutTest        *test,
                            CutTestContext *context,
                            CutTestResult  *result);
    void (*complete)       (CutTest        *test);
    void (*crashed)        (CutTest        *test,
                            const gchar    *stack_trace);

    gdouble      (*get_elapsed)  (CutTest *test);
    const gchar *(*get_name)     (CutTest *test);
};

GType        cut_test_get_type  (void) G_GNUC_CONST;

CutTest     *cut_test_new                 (const gchar *function_name,
                                           const gchar *description,
                                           CutTestFunction function);
gboolean     cut_test_run                 (CutTest *test,
                                           CutTestContext *test_context,
                                           CutContext *context);

const gchar *cut_test_get_name            (CutTest *test);
const gchar *cut_test_get_description     (CutTest *test);

gdouble      cut_test_get_elapsed         (CutTest *test);

G_END_DECLS

#endif /* __CUT_TEST_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
