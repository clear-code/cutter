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

#ifndef __GCUT_HASH_TABLE_H__
#define __GCUT_HASH_TABLE_H__

#include <glib-object.h>
#include <gcutter/gcut-list.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-hash-table
 * @title: Assertion Utilities for GHashTable
 * @short_description: Utilities to write assertions related
 * to #GHashTable more easily.
 *
 * To write assertions, you need to check equality and show
 * expected and actual values.
 *
 * The utilities help you to write assertions that are
 * related to #GHashTable.
 */


/**
 * gcut_hash_table_equal:
 * @hash1: a #GHashTable to be compared.
 * @hash2: a #GHashTable to be compared.
 * @equal_func: a function that compares two values.
 *
 * Compares two #GHashTable, @hash1 and
 * @hash2. @equal_func is called for each values of
 * the same key of @hash1 and @hash2.
 *
 * e.g.:
 * |[
 * TODO
 * ]|
 *
 * Returns: TRUE if all same key's values of @hash1 and
 * @hash2 are reported TRUE by @equal_func, FALSE
 * otherwise.
 *
 * Since: 1.0.5
 */
gboolean         gcut_hash_table_equal          (GHashTable *hash1,
                                                 GHashTable *hash2,
                                                 GEqualFunc equal_func);

/**
 * gcut_hash_table_inspect:
 * @hash: a #GHashTable to be inspected.
 * @key_inspect_func: a function that inspects each key.
 * @value_inspect_func: a function that inspects each value.
 * @user_data: user data to pass to the function.
 *
 * Inspects @hash. Each key of @hash is inspected by
 * @key_inspect_func and each value of @hash is inspected by
 * @value_inspect_func. The returned string should be freed
 * when no longer needed.
 *
 * e.g.:
 * |[
 * TODO
 * ]|
 *
 * Returns: inspected @hash as a string.
 *
 * Since: 1.0.5
 */
gchar           *gcut_hash_table_inspect        (GHashTable *hash,
                                                 GCutInspectFunction key_inspect_func,
                                                 GCutInspectFunction value_inspect_func,
                                                 gpointer user_data);

/**
 * gcut_hash_table_string_equal:
 * @hash1: a #GHashTable to be compared.
 * @hash2: a #GHashTable to be compared.
 *
 * Compares two #GHashTable, @hash1 and
 * @hash2. @hash1 and @hash2 should have string key and
 * string value.
 *
 * Returns: TRUE if all same key's values of @hash1 and
 * @hash2 are same string content, FALSE otherwise.
 *
 * Since: 1.0.5
 */
gboolean         gcut_hash_table_string_equal   (GHashTable *hash1,
                                                 GHashTable *hash2);

/**
 * gcut_hash_table_string_string_inspect:
 * @hash: a #GHashTable to be inspected.
 *
 * Inspects @hash. @hash should have string key and string
 * value. The returned string should be freed when no longer
 * needed.
 *
 * Returns: inspected @hash as a string.
 *
 * Since: 1.0.5
 */
gchar           *gcut_hash_table_string_string_inspect
                                                (GHashTable *hash);

/**
 * gcut_hash_table_string_string_copy:
 * @hash: a #GHashTable to be copied.
 *
 * Copy @hash. @hash should have string key and string
 * value.
 *
 * Returns: copied #GHashTable.
 *
 * Since: 1.0.5
 */
GHashTable      *gcut_hash_table_string_string_copy
                                                (GHashTable *hash);


G_END_DECLS

#endif /* __GCUT_HASH_TABLE_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
