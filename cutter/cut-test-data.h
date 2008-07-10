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

#ifndef __CUT_TEST_DATA_H__
#define __CUT_TEST_DATA_H__

#include <glib-object.h>

#include <cutter/cut-private.h>
#include <cutter/cut-public.h>

G_BEGIN_DECLS

#define CUT_TYPE_TEST_DATA            (cut_test_data_get_type ())
#define CUT_TEST_DATA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST_DATA, CutTestData))
#define CUT_TEST_DATA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST_DATA, CutTestDataClass))
#define CUT_IS_TEST_DATA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST_DATA))
#define CUT_IS_TEST_DATA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST_DATA))
#define CUT_TEST_DATA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST_DATA, CutTestDataClass))

typedef struct _CutTestData         CutTestData;
typedef struct _CutTestDataClass    CutTestDataClass;

struct _CutTestData
{
    GObject object;
};

struct _CutTestDataClass
{
    GObjectClass parent_class;
};

GType            cut_test_data_get_type            (void) G_GNUC_CONST;

CutTestData     *cut_test_data_new                 (const gchar    *name,
                                                    gpointer        value,
                                                    CutDestroyFunction destroy_function);
CutTestData     *cut_test_data_new_empty           (void);

const gchar     *cut_test_data_get_name            (CutTestData *test_data);
void             cut_test_data_set_name            (CutTestData *test_data,
                                                    const gchar *name_data);

gpointer         cut_test_data_get_value           (CutTestData *test_data);
void             cut_test_data_set_value           (CutTestData *test_data,
                                                    gpointer     value,
                                                    CutDestroyFunction destroy_function);

gchar           *cut_test_data_to_xml              (CutTestData *test_data);
void             cut_test_data_to_xml_string       (CutTestData *test_data,
                                                    GString     *string,
                                                    guint        indent);

G_END_DECLS

#endif /* __CUT_TEST_DATA_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
