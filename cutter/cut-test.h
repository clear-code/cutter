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

#include "cut-assertion.h"

G_BEGIN_DECLS

#define CUT_TYPE_TEST            (cut_test_get_type ())
#define CUT_TEST(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST, CutTest))
#define CUT_TEST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST, CutTestClass))
#define CUT_IS_TEST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST))
#define CUT_IS_TEST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST))
#define CUT_TEST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST, CutTestClass))

typedef struct _CutTest         CutTest;
typedef struct _CutTestClass    CutTestClass;
typedef struct _CutTestResult    CutTestResult;

typedef void    (*CutTestFunction)     (void);
typedef void    (*CutSetupFunction)    (void);
typedef void    (*CutTearDownFunction) (void);

struct _CutTestResult
{
    CutTestResultStatus status;
    gchar *message;
    gchar *function_name;
    gchar *filename;
    guint line;
};

struct _CutTest
{
    GObject object;
};

struct _CutTestClass
{
    GObjectClass parent_class;

    void (*start)    (CutTest *test);
    void (*success)  (CutTest *test);
    void (*failure)  (CutTest *test);
    void (*error)    (CutTest *test);
    void (*pending)  (CutTest *test);
    void (*complete) (CutTest *test);

    gboolean (*run)  (CutTest *test);
    gdouble  (*get_elapsed) (CutTest *test);
    guint    (*get_n_assertions) (CutTest *test);
    guint    (*get_n_failures)   (CutTest *test);
    guint    (*get_n_errors)     (CutTest *test);
    guint    (*get_n_pendings)   (CutTest *test);
};

GType        cut_test_get_type  (void) G_GNUC_CONST;

CutTest     *cut_test_new                 (const gchar *function_name,
                                           CutTestFunction function);
gboolean     cut_test_run                 (CutTest *test);
const gchar *cut_test_get_function_name   (CutTest *test);
void         cut_test_increment_assertion_count
                                          (CutTest *test);
void         cut_test_set_result          (CutTest *test,
                                           CutTestResultStatus status,
                                           const gchar *result_message,
                                           const gchar *function_name,
                                           const gchar *filename,
                                           guint line);
const CutTestResult *cut_test_get_result  (CutTest *test);

gdouble      cut_test_get_elapsed         (CutTest *test);

guint        cut_test_get_n_assertions    (CutTest *test);
guint        cut_test_get_n_failures      (CutTest *test);
guint        cut_test_get_n_errors        (CutTest *test);
guint        cut_test_get_n_pendings      (CutTest *test);

G_END_DECLS

#endif /* __CUT_TEST_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
