/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2010  Kouhei Sutou <kou@clear-code.com>
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
 * @title: Convenience test data API
 * @short_description: API to create test data without
 * structure definition.
 *
 * cut_add_data() requires custom data type for complex test
 * data. But it's not easy to write. gcut_add_datum()
 * provides API to use complex test data without custom data
 * type. It uses #GType for providing complex data.
 */


/**
 * gcut_add_datum:
 * @name: the name of the data.
 * @first_field_name: the first field name.
 * @...: the type and value pair of the first field,
 *       followed optionally by the next field name, type
 *       and value triples. %NULL-terminated. See
 *       description for more details.
 *
 * Adds a datum to be used in data driven test. It's
 * convenient rather than cut_add_data() because you doesn't
 * need to define a new structure for a complex test data.
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
 *                    "translated", G_TYPE_STRING, "first",
 *                    "input", G_TYPE_INT, 1,
 *                    NULL);
 *     gcut_add_datum("complex data",
 *                    "translated", G_TYPE_STRING, "a hundred eleven",
 *                    "input", G_TYPE_INT, 111,
 *                    NULL);
 * }
 *
 * void
 * test_translate(gconstpointer data)
 * {
 *     cut_assert_equal_string(gcut_data_get_string(data, "translated"),
 *                             gcut_data_get_int(data, "input"));
 * }
 * ]|
 *
 * Available types and their values are the followings:
 *
 * <variablelist>
 *   <varlistentry>
 *     <term>#G_TYPE_CHAR</term>
 *     <listitem>
 *       <para>#gchar value</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", G_TYPE_CHAR, 'X',
 *                NULL);
 * ]|
 *       </para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>#G_TYPE_STRING</term>
 *     <listitem>
 *       <para>const #gchar *value</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", G_TYPE_STRING, "string value",
 *                NULL);
 * ]|
 *       </para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>#G_TYPE_INT</term>
 *     <listitem>
 *       <para>#gint value</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", G_TYPE_INT, 100,
 *                NULL);
 * ]|
 *       </para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>#G_TYPE_UINT</term>
 *     <listitem>
 *       <para>#guint value</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", G_TYPE_UINT, 100,
 *                NULL);
 * ]|
 *       </para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>#G_TYPE_INT64</term>
 *     <listitem>
 *       <para>#gint64 value</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", G_TYPE_INT64, G_GINT64_CONSTANT(100),
 *                NULL);
 * ]|
 *       </para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>#G_TYPE_UINT64</term>
 *     <listitem>
 *       <para>#guint64 value</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", G_TYPE_UINT64, G_GUINT64_CONSTANT(100),
 *                NULL);
 * ]|
 *       </para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>#G_TYPE_GTYPE</term>
 *     <listitem>
 *       <para>#GType value</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", G_TYPE_GTYPE, G_TYPE_OBJECT,
 *                NULL);
 * ]|
 *       </para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>GFlags types</term>
 *     <listitem>
 *       <para>its type value.</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", GTK_TYPE_WIDGET_FLAGS, GTK_TOPLEVEL | GTK_MAPPED,
 *                NULL);
 * ]|
 *       </para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>GEnum types</term>
 *     <listitem>
 *       <para>its type value.</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", GTK_TYPE_WRAP_MODE, GTK_WRAP_NONE,
 *                NULL);
 * ]|
 *       </para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>#G_TYPE_POINTER</term>
 *     <listitem>
 *       <para>#gconstpointer value, #GDestroyNotify notify</para>
 *       <para>notify is called when value is destroyed.</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", G_TYPE_POINTER, my_structure_new(...), my_structure_free,
 *                NULL);
 * ]|
 *       </para>
 *       <para>NOTE: value's ownership is passed to Cutter. Don't free it.</para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>GBoxed types</term>
 *     <listitem>
 *       <para>its type value.</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", G_TYPE_HASH_TABLE,
 *                gcut_hash_table_string_string_new("name1", "value1",
 *                                                  "name2", "value2",
 *                                                  NULL),
 *                NULL);
 * ]|
 *       </para>
 *       <para>NOTE: value's ownership is passed to Cutter. Don't free it.</para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>#G_TYPE_BOOLEAN</term>
 *     <listitem>
 *       <para>#gboolean value</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", G_TYPE_BOOLEAN, TRUE,
 *                NULL);
 * ]|
 *       </para>
 *     </listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>#G_TYPE_DOUBLE</term>
 *     <listitem>
 *       <para>#gdouble value</para>
 *       <para>e.g.:
 * |[
 * gcut_add_datum("data name",
 *                "field-name", G_TYPE_DOUBLE, 2.9,
 *                NULL);
 * ]|
 *       </para>
 *     </listitem>
 *   </varlistentry>
 * </variablelist>
 *
 * Since: 1.0.6
 */
#define gcut_add_datum(name, first_field_name, ...)             \
    cut_test_context_add_data(                                  \
        cut_get_current_test_context(),                         \
        name,                                                   \
        gcut_dynamic_data_new(first_field_name, __VA_ARGS__),   \
        g_object_unref,                                         \
        NULL)

/**
 * gcut_data_has_field:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Returns %TRUE if @data has a field named by @field_name.
 *
 * Returns: %TRUE if @data has a @field_name field is
 *          available, %FALSE otherwise.
 *
 * Since: 1.1.5
 */
#define gcut_data_has_field(data, field_name)                          \
    gcut_dynamic_data_has_field(data, field_name)

/**
 * gcut_data_get_char:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as char.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.1.3
 */
#define gcut_data_get_char(data, field_name)                          \
    gcut_data_get_char_helper(                                        \
        data, field_name,                                             \
        (cut_push_backtrace(gcut_data_get_char(data, field_name)),    \
         cut_pop_backtrace))

/**
 * gcut_data_get_string:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as string.
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

/**
 * gcut_data_get_int:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as integer.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.0.6
 */
#define gcut_data_get_int(data, field_name)                             \
    gcut_data_get_int_helper(                                           \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_int(data, field_name)),       \
         cut_pop_backtrace))

/**
 * gcut_data_get_uint:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as
 * unsigned integer.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.0.6
 */
#define gcut_data_get_uint(data, field_name)                            \
    gcut_data_get_uint_helper(                                          \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_uint(data, field_name)),      \
         cut_pop_backtrace))

/**
 * gcut_data_get_int64:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as 64-bit
 * integer.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.1.3
 */
#define gcut_data_get_int64(data, field_name)                           \
    gcut_data_get_int64_helper(                                         \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_int64(data, field_name)),     \
         cut_pop_backtrace))

/**
 * gcut_data_get_uint64:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as
 * 64-bit unsigned integer.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.1.3
 */
#define gcut_data_get_uint64(data, field_name)                          \
    gcut_data_get_uint64_helper(                                        \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_uint64(data, field_name)),    \
         cut_pop_backtrace))

/**
 * gcut_data_get_size:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as size_t.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.1.3
 */
#define gcut_data_get_size(data, field_name)                            \
    gcut_data_get_size_helper(                                          \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_size(data, field_name)),      \
         cut_pop_backtrace))

/**
 * gcut_data_get_type:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as #GType.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.0.6
 */
#define gcut_data_get_type(data, field_name)                            \
    gcut_data_get_type_helper(                                          \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_type(data, field_name)),      \
         cut_pop_backtrace))

/**
 * gcut_data_get_flags:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as
 * unsigned integer of GFlags.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.0.6
 */
#define gcut_data_get_flags(data, field_name)                           \
    gcut_data_get_flags_helper(                                         \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_flags(data, field_name)),     \
         cut_pop_backtrace))

/**
 * gcut_data_get_enum:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as
 * integer of GEnum type.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.0.6
 */
#define gcut_data_get_enum(data, field_name)                            \
    gcut_data_get_enum_helper(                                          \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_enum(data, field_name)),      \
         cut_pop_backtrace))

/**
 * gcut_data_get_pointer:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as
 * pointer.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.0.6
 */
#define gcut_data_get_pointer(data, field_name)                         \
    gcut_data_get_pointer_helper(                                       \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_pointer(data, field_name)),   \
         cut_pop_backtrace))

/**
 * gcut_data_get_boxed:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as
 * GBoxed type value.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.0.7
 */
#define gcut_data_get_boxed(data, field_name)                           \
    gcut_data_get_boxed_helper(                                         \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_boxed(data, field_name)),     \
         cut_pop_backtrace))

/**
 * gcut_data_get_object:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as
 * #GObject type value.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.1.1
 */
#define gcut_data_get_object(data, field_name)                          \
    gcut_data_get_object_helper(                                        \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_object(data, field_name)),    \
         cut_pop_backtrace))

/**
 * gcut_data_get_boolean:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as boolean.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.1.3
 */
#define gcut_data_get_boolean(data, field_name)                         \
    gcut_data_get_boolean_helper(                                       \
        data, field_name,                                               \
        (cut_push_backtrace(gcut_data_get_boolean(data, field_name)),   \
         cut_pop_backtrace))

/**
 * gcut_data_get_double:
 * @data: the data added by gcut_add_datum().
 * @field_name: the field name.
 *
 * Gets a field value identified by @field_name as double
 * floating point number.
 *
 * Returns: a field value corresponded to @field_name.
 *
 * Since: 1.1.3
 */
#define gcut_data_get_double(data, field_name)                         \
    gcut_data_get_double_helper(                                       \
        data, field_name,                                              \
        (cut_push_backtrace(gcut_data_get_double(data, field_name)),   \
         cut_pop_backtrace))

G_END_DECLS

#endif /* __GCUT_DATA_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
