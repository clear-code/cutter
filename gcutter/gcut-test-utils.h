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

#ifndef __GCUT_TEST_UTILS_H__
#define __GCUT_TEST_UTILS_H__

#include <gcutter/gcut-public.h>
#include <gcutter/gcut-string-io-channel.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-test-utils
 * @title: Test Utilities with GLib support
 * @short_description: Utilities to write tests more easily
 * with GLib support.
 *
 * To write tests, you need to write codes that
 * set up/tear down test environment, prepare expected and
 * actual values and so on. Cutter provides test utilities
 * to you write your tests more easily.
 *
 * The utilities work with GLib.
 */


/**
 * gcut_take_object:
 * @object: the #GObject to be owned by Cutter.
 *
 * Passes ownership of @object to Cutter and returns @object.
 *
 * Returns: @object owned by Cutter. Don't g_object_unref() it.
 *
 * Since: 1.0.3
 */
#define gcut_take_object(object)                                        \
    cut_test_context_take_g_object(cut_get_current_test_context(),      \
                                   (object))

/**
 * gcut_take_error:
 * @error: the #GError to be owned by Cutter.
 *
 * Passes ownership of @error to Cutter and returns
 * a @error itself.
 *
 * Returns: a #GError owned by Cutter. Don't g_error_free()
 * it.
 *
 * Since: 1.0.3
 */
#define gcut_take_error(error)                                          \
    cut_test_context_take_g_error(cut_get_current_test_context(),       \
                                  (error))

/**
 * gcut_take_list:
 * @list: the #GList to be owned by Cutter.
 * @destroy_function: the destroy function that destroys the
 *                    elements of @list, or %NULL.
 *
 * Passes ownership of @list to Cutter and returns
 * @list itself.
 *
 * Returns: a #GList owned by Cutter. Don't g_list_free()
 * it.
 *
 * Since: 1.0.3
 */
#define gcut_take_list(list, destroy_function)                          \
    cut_test_context_take_g_list(cut_get_current_test_context(),        \
                                 (list), (destroy_function))

/**
 * gcut_take_hash_table:
 * @hash_table: the #GHashTable to be owned by Cutter.
 *
 * Passes ownership of @hash_table to Cutter and returns
 * @hash_table itself.
 *
 * Returns: a #GHashTable owned by Cutter. Don't
 * g_hash_table_unref() it.
 *
 * Since: 1.0.4
 */
#define gcut_take_hash_table(hash_table)                                \
    cut_test_context_take_g_hash_table(cut_get_current_test_context(),  \
                                       (hash_table))

/**
 * gcut_list_string_new:
 * @value: the first string.
 * @...: remaining strings in list. %NULL-terminate.
 *
 * Creates a list from passed strings.
 *
 * Returns: a newly-allocated #GList that contains passed
 * strings and must be freed with gcut_list_string_free().
 *
 * Since: 1.0.3
 */
GList  *gcut_list_string_new  (const gchar *value, ...) G_GNUC_NULL_TERMINATED;

/**
 * gcut_list_string_new_array:
 * @strings: the string array. %NULL-terminate.
 *
 * Creates a list from passed string array.
 *
 * Returns: a newly-allocated #GList that contains passed
 * string array and must be freed with gcut_list_string_free().
 *
 * Since: 1.0.6
 */
GList  *gcut_list_string_new_array  (const gchar **strings);

/**
 * gcut_take_new_list_string:
 * @value: the first string.
 * @...: remaining strings in list. %NULL-terminate.
 *
 * Creates a list from passed strings that is owned by
 * Cutter.
 *
 * Returns: a newly-allocated #GList that contains passed
 * strings and is owned by Cutter.
 *
 * Since: 1.0.5
 */
#define gcut_take_new_list_string(value, ...)                           \
    gcut_take_new_list_string_backward_compatibility(value, __VA_ARGS__)

#if !defined(CUTTER_DISABLE_DEPRECATED) && defined(__GNUC__)
#define gcut_take_new_list_string_backward_compatibility(value, ...)    \
    gcut_take_list(gcut_list_string_new(value, ## __VA_ARGS__, NULL),   \
                   g_free)
#else
#define gcut_take_new_list_string_backward_compatibility(value, ...)    \
    gcut_take_list(gcut_list_string_new(value, __VA_ARGS__),            \
                   g_free)
#endif

/**
 * gcut_take_new_list_string_array:
 * @strings: the string array. %NULL-terminate.
 *
 * Creates a list from passed string array that is owned by
 * Cutter.
 *
 * Returns: a newly-allocated #GList that contains passed
 * string array and is owned by Cutter.
 *
 * Since: 1.0.6
 */
#define gcut_take_new_list_string_array(strings)                \
    gcut_take_list(gcut_list_string_new_array(strings), g_free)

/**
 * gcut_list_string_free:
 * @list: the list that contains strings to be freed.
 *
 * Frees @list and contained strings.
 *
 * Since: 1.0.3
 */
void    gcut_list_string_free (GList *list);

/**
 * gcut_hash_table_string_string_new:
 * @key: The first key string.
 * @...: A %NULL-terminated list of rest arguments. The
 *       first item is the string value of the first key,
 *       followed optionally by more key/value pairs. It
 *       must end with %NULL.
 *
 * Creates a hash table that has string key and string value
 * from passed key/value pairs.
 *
 * Returns: a newly-allocated #GHashTable that contains
 * passed key/value pairs and must be freed with
 * g_hash_table_unref().
 *
 * Since: 1.0.4
 */
GHashTable *gcut_hash_table_string_string_new(const gchar *key, ...) G_GNUC_NULL_TERMINATED;

/**
 * gcut_hash_table_string_string_new_va_list:
 * @key: The first key string.
 * @args: A %NULL-terminated list of rest arguments. The
 *        first item is the string value of the first key,
 *        followed optionally by more key/value pairs. It
 *        must end with %NULL.
 *
 * Creates a hash table that has string key and string value
 * from passed key/value pairs.
 *
 * Returns: a newly-allocated #GHashTable that contains
 * passed key/value pairs and must be freed with
 * g_hash_table_unref().
 *
 * Since: 1.0.5
 */
GHashTable *gcut_hash_table_string_string_new_va_list(const gchar *key,
                                                      va_list args);

/**
 * gcut_take_new_hash_table_string_string:
 * @key: The first key string.
 * @...: A %NULL-terminated list of rest arguments. The
 *       string value of the first key, followed optionally
 *       by more key/value pairs. It must end with %NULL.
 *
 * Creates a hash table that has string key and string value
 * from passed key/value pairs. The created hash table is
 * owned by Cutter. So don't g_hash_table_unref() it.
 *
 * Returns: a newly-allocated #GHashTable that contains
 * passed key/value pairs.
 *
 * Since: 1.0.5
 */
#define gcut_take_new_hash_table_string_string(key, ...)                \
    gcut_take_hash_table(gcut_hash_table_string_string_new(key, ## __VA_ARGS__))



/**
 * gcut_data_get_string:
 * @data: the %GCutData.
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

#endif /* __GCUT_TEST_UTILS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
