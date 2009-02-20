/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __GCUT_DATA_H__
#define __GCUT_DATA_H__

#include <gcutter/gcut-data-helper.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-data
 * @title: Convenience test data API.
 * @short_description: API to create test data without
 * structure definition.
 *
 * cut_add_data() requires custom structure data type for
 * complex test data. But it's not easy to
 * write. gcut_add_datum() provides test data API to use
 * test data without custom structure data type. It uses
 * %GType for providing dynamic complex data.
 */


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
 * test_translate(gconstpointer data)
 * {
 *     cut_assert_equal_string(gcut_data_get_string(data, "/translated"),
 *                             gcut_data_get_int(data, "/input"));
 * }
 * ]|
 *
 * Since: 1.0.6
 */
#define gcut_add_datum(label, first_field_name, ...)            \
    cut_test_context_add_data(                                  \
        cut_get_current_test_context(),                         \
        label,                                                  \
        gcut_dynamic_data_new(first_field_name, __VA_ARGS__),   \
        g_object_unref,                                         \
        NULL)


/**
 * gcut_data_get_string:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Returns a field value identified by @field_name as string.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.0.6
 */
#define gcut_data_get_string(data, field_name)                          \
    gcut_data_get_string_helper(                                        \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_string(data, field_name)),    \
         cut_pop_backtrace))

#define gcut_data_get_int(data, field_name)                             \
    gcut_data_get_int_helper(                                           \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_int(data, field_name)),       \
         cut_pop_backtrace))

#define gcut_data_get_uint(data, field_name)                            \
    gcut_data_get_uint_helper(                                          \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_uint(data, field_name)),      \
         cut_pop_backtrace))

#define gcut_data_get_type(data, field_name)                            \
    gcut_data_get_type_helper(                                          \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_type(data, field_name)),      \
         cut_pop_backtrace))

#define gcut_data_get_flags(data, field_name)                           \
    gcut_data_get_flags_helper(                                         \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_flags(data, field_name)),     \
         cut_pop_backtrace))

#define gcut_data_get_enum(data, field_name)                            \
    gcut_data_get_enum_helper(                                          \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_enum(data, field_name)),      \
         cut_pop_backtrace))

#define gcut_data_get_pointer(data, field_name)                         \
    gcut_data_get_pointer_helper(                                       \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_pointer(data, field_name)),   \
         cut_pop_backtrace))


G_END_DECLS

#endif /* __GCUT_DATA_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
