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

#ifndef __CUT_ITERATED_TEST_H__
#define __CUT_ITERATED_TEST_H__

#include <glib-object.h>

#include <cutter/cut-private.h>
#include <cutter/cut-test.h>

G_BEGIN_DECLS

#define CUT_TYPE_ITERATED_TEST            (cut_iterated_test_get_type ())
#define CUT_ITERATED_TEST(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_ITERATED_TEST, CutIteratedTest))
#define CUT_ITERATED_TEST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_ITERATED_TEST, CutIteratedTestClass))
#define CUT_IS_ITERATED_TEST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_ITERATED_TEST))
#define CUT_IS_ITERATED_TEST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_ITERATED_TEST))
#define CUT_ITERATED_TEST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_ITERATED_TEST, CutIteratedTestClass))

typedef void    (*CutIteratedTestFunction) (gconstpointer data);

typedef struct _CutIteratedTest         CutIteratedTest;
typedef struct _CutIteratedTestClass    CutIteratedTestClass;

struct _CutIteratedTest
{
    CutTest object;
};

struct _CutIteratedTestClass
{
    CutTestClass parent_class;
};

GType                cut_iterated_test_get_type    (void) G_GNUC_CONST;

CutIteratedTest     *cut_iterated_test_new         (const gchar     *name,
                                                    CutIteratedTestFunction function);
CutIteratedTest     *cut_iterated_test_new_empty   (void);

G_END_DECLS

#endif /* __CUT_ITERATED_TEST_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
