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

#ifndef __GCUTTER_H__
#define __GCUTTER_H__

#include <cutter.h>
#include <gcutter/gcut-main.h>
#include <gcutter/gcut-assertions.h>

/**
 * gcut_add_datum:
 * @name: The name of the data.
 * @first_field_name: The first field name.
 * @...: FIXME: The type and value of the field. NULL
 *       terminated.
 *
 * Adds a datum to use data driven test. FIXME: It's
 * convenient rather than cut_add_data() because you doesn't
 * need to define a new data structure for a test.
 *
 * e.g.:
 * |[
 * #include <gcutter.h>
 *
 * void data_translate (void);
 * void test_translate (gconstpointer data);
 *
 * static const gchar*
 * translate (gint input)
 * {
 *    switch(input) {
 *    case 1:
 *        return "first";
 *    case 111:
 *        return "a hundred eleven";
 *    default:
 *        return "unsupported";
 *    }
 * }
 *
 * void
 * data_translate(void)
 * {
 *     gcut_add_datum("simple data",
 *                    "/translated", G_TYPE_STRING, "first",
 *                    "/input", G_TYPE_INT, 1,
 *                    NULL);
 *     gcut_add_datum("complex data",
 *                    "/translated", G_TYPE_STRING, "a hundred eleven",
 *                    "/input", G_TYPE_INT, 111,
 *                    NULL);
 * }
 *
 * void
 * test_translate(gconstpointer *_data)
 * {
 *     const GCutData *data = _data;
 *
 *     cut_assert_equal_string(gcut_data_get_string(data, "/translated"),
 *                             gcut_data_get_int(data, "/input"));
 * }
 * ]|
 *
 * Since: 1.0.6
 */
#define gcut_add_datum(label, first_field_name, ...)    \
    cut_test_context_add_data(                          \
        cut_get_current_test_context(),                 \
        label,                                          \
        gcut_data_new(first_field_name, __VA_ARGS__),   \
        g_object_unref,                                 \
        NULL)

#define gcut_data_get_string(data, field_name)                          \
    gcut_data_get_string_helper(                                        \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_string(data, field_name)),    \
         gcut_pop_backtrace))

#define gcut_data_get_gtype(data, field_name)                           \
    gcut_data_get_gtype_helper(                                         \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_gtype(data, field_name)),     \
         gcut_pop_backtrace))

#endif /* __GCUTTER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
