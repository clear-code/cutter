/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2020  Sutou Kouhei <kou@clear-code.com>
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
 * gcut_take_string:
 * @string: the #GString to be owned by Cutter.
 *
 * Passes ownership of @string to Cutter and returns
 * @string itself.
 *
 * Returns: a #GString owned by Cutter. Don't
 * g_string_free() it.
 *
 * Since: 1.1.6
 */
#define gcut_take_string(string)                                        \
    cut_test_context_take_g_string(cut_get_current_test_context(),      \
                                   (string))

/**
 * gcut_take_new_string:
 * @string: the string. It can be %NULL.
 *
 * Creates a #GString object from passed string.
 *
 * Returns: a #GString owned by Cutter. Don't
 * g_string_free() it.
 *
 * Since: 1.1.6
 */
#define gcut_take_new_string(string)            \
    gcut_take_string(g_string_new(string))

/**
 * gcut_take_bytes:
 * @bytes: the #GBytes to be owned by Cutter.
 *
 * Passes ownership of @bytes to Cutter and returns
 * @bytes itself.
 *
 * Returns: a #GBytes owned by Cutter. Don't g_bytes_unref() it.
 *
 * Since: 1.2.8
 */
#define gcut_take_bytes(bytes)                                          \
    cut_test_context_take_g_bytes(cut_get_current_test_context(),       \
                                  (bytes))
/**
 * gcut_list_new:
 * @element: the first #gpointer.
 * @...: remaining elements in list. %NULL-terminate.
 *
 * Creates a list from passed elements.
 *
 * e.g.:
 * |[
 * GCutEgg *echo_egg, *cat_egg;
 *
 * echo_egg = gcut_egg_new("echo", "Hello", NULL);
 * cat_egg = gcut_egg_new("cat", "/etc/hosts", NULL);
 * egg_list = gcut_list_new(echo_egg, cat_egg, NULL);
 * ]|
 *
 * Returns: a newly-allocated #GList that contains passed
 * elements.
 *
 * Since: 1.1.1
 */
GList  *gcut_list_new         (const gpointer element,
                               ...) G_GNUC_NULL_TERMINATED;

/**
 * gcut_list_int_new:
 * @n: the number of integer values.
 * @value: the first integer value.
 * @...: remaining integer values in list.
 *
 * Creates a list from passed integer values.
 *
 * e.g.:
 * |[
 * gcut_list_int_new(3, -10, 1, 29); -> (-10, 1, 29)
 * ]|
 *
 * Returns: a newly-allocated #GList that contains passed
 * integer values and must be freed with g_list_free().
 *
 * Since: 1.1.5
 */
GList  *gcut_list_int_new  (guint n, gint value, ...);

/**
 * gcut_list_uint_new:
 * @n: the number of unsigned integer values.
 * @value: the first unsigned integer value.
 * @...: remaining unsigned integer values in list.
 *
 * Creates a list from passed unsigned integer values.
 *
 * e.g.:
 * |[
 * gcut_list_uint_new(3, 0, 1, 2); -> (0, 1, 2)
 * ]|
 *
 * Returns: a newly-allocated #GList that contains passed
 * unsigned integer values and must be freed with
 * g_list_free().
 *
 * Since: 1.1.5
 */
GList  *gcut_list_uint_new  (guint n, guint value, ...);

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
 * gcut_take_new_list_int:
 * @n: the number of integer values.
 * @value: the first integer value.
 * @...: remaining strings in list.
 *
 * Creates a list from passed integer values that is owned
 * by Cutter.
 *
 * e.g.:
 * |[
 * gcut_take_new_list_int(3, -10, 1, 29); -> (-10, 1, 29)
 * ]|
 *
 * Returns: a newly-allocated #GList that contains passed
 * integer values and is owned by Cutter.
 *
 * Since: 1.1.5
 */
#define gcut_take_new_list_int(n, value, ...)                           \
    gcut_take_list(gcut_list_int_new(n, value, __VA_ARGS__), NULL)

/**
 * gcut_take_new_list_uint:
 * @n: the number of unsigned integer values.
 * @value: the first unsigned integer value.
 * @...: remaining strings in list.
 *
 * Creates a list from passed unsigned integer values that
 * is owned by Cutter.
 *
 * e.g.:
 * |[
 * gcut_take_new_list_uint(3, 0, 1, 2); -> (0, 1, 2)
 * ]|
 *
 * Returns: a newly-allocated #GList that contains passed
 * unsigned integer values and is owned by Cutter.
 *
 * Since: 1.1.5
 */
#define gcut_take_new_list_uint(n, value, ...)                          \
    gcut_take_list(gcut_list_uint_new(n, value, __VA_ARGS__), NULL)

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
 * gcut_take_new_list_object:
 * @object: the first #GObject.
 * @...: remaining objects in list. %NULL-terminate.
 *
 * Creates a list from passed objects that is owned by
 * Cutter.
 *
 * Returns: a newly-allocated #GList that contains passed
 * objects and is owned by Cutter.
 *
 * Since: 1.1.1
 */
#define gcut_take_new_list_object(object, ...)                      \
    gcut_take_list(gcut_list_new(object, __VA_ARGS__), g_object_unref)

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
 * gcut_list_object_free:
 * @list: the list that contains #GObject's to be unrefed.
 *
 * Frees @list and contained objects. It's safe that @list
 * contains %NULL.
 *
 * Since: 1.1.1
 */
void    gcut_list_object_free (GList *list);

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
 * gcut_get_fixture_data:
 * @path: a first element of the path to the fixture data.
 * @...: remaining elements in path.
 *       %NULL-terminate is required.
 *
 * Reads the fixture data at "@path/..." and returns it as a
 * #GString that is owned by Cutter. The description of
 * cut_build_fixture_path() shows how the fixture data path
 * is determined.
 *
 * Returns: a content of the fixture data as #GString owend by
 * Cutter. Don't free it.
 *
 * Since: 1.1.6
 */
#define gcut_get_fixture_data(...)                                      \
    gcut_utils_get_fixture_data(cut_get_current_test_context(),         \
                                NULL, __VA_ARGS__)

G_END_DECLS

#endif /* __GCUT_TEST_UTILS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
