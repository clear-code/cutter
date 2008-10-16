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

#ifndef __GCUT_LIST_H__
#define __GCUT_LIST_H__

#include <glib-object.h>

#include <gcutter/gcut-types.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-list
 * @title: Assertion Utilities for GList
 * @short_description: Utilities to write assertions related
 * to #GList more easily.
 *
 * To write assertions, you need to check equality and show
 * expected and actual values.
 *
 * The utilities help you to write assertions that are
 * related to #GList.
 */


/**
 * gcut_list_equal:
 * @list1: a #GList to be compared.
 * @list2: a #GList to be compared.
 * @equal_func: a function that compares two values.
 *
 * Compares two #GList, @list1 and @list2. @equal_func is
 * called for each corresponding values of @list1 and @list2.
 *
 * e.g.:
 * |[
 * TODO
 * ]|
 *
 * Returns: TRUE if all corresponding values of @list1 and
 * @list2 are reported TRUE by @equal_func, FALSE otherwise.
 *
 * Since: 1.0.5
 */
gboolean         gcut_list_equal                (const GList *list1,
                                                 const GList *list2,
                                                 GEqualFunc   equal_func);

/**
 * gcut_list_inspect:
 * @list: a #GList to be inspected.
 * @inspect_func: a function that inspects each value.
 * @user_data: user data to pass to the function.
 *
 * Inspects @list. Each value of @list is inspected by
 * @inspect_func. The returned string should be freed when
 * no longer needed.
 *
 * e.g.:
 * |[
 * TODO
 * ]|
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.5
 */
gchar           *gcut_list_inspect              (const GList *list,
                                                 GCutInspectFunc inspect_func,
                                                 gpointer user_data);

/**
 * gcut_list_equal_int:
 * @list1: a #GList of #gint to be compared.
 * @list2: a #GList of #gint to be compared.
 *
 * Compares two #GList, @list1 and @list2. @list1 and @list2
 * should be #GList of #gint.
 *
 * Returns: TRUE if all corresponding integers of @list1 and
 * @list2 are same value, FALSE otherwise.
 *
 * Since: 1.0.6
 */
gboolean         gcut_list_equal_int            (const GList *list1,
                                                 const GList *list2);

/**
 * gcut_list_inspect_int:
 * @list: a #GList of #gint to be inspected.
 *
 * Inspects @list. @list should be #GList of #gint. The
 * returned string should be freed when no longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.6
 */
gchar           *gcut_list_inspect_int          (const GList *list);

/**
 * gcut_list_equal_uint:
 * @list1: a #GList of #guint to be compared.
 * @list2: a #GList of #guint to be compared.
 *
 * Compares two #GList, @list1 and @list2. @list1 and @list2
 * should be #GList of #guint.
 *
 * Returns: TRUE if all corresponding unsigned integers of
 * @list1 and @list2 are same value, FALSE otherwise.
 *
 * Since: 1.0.6
 */
gboolean         gcut_list_equal_uint           (const GList *list1,
                                                 const GList *list2);

/**
 * gcut_list_inspect_uint:
 * @list: a #GList of #guint to be inspected.
 *
 * Inspects @list. @list should be #GList of #guint. The
 * returned string should be freed when no longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.6
 */
gchar           *gcut_list_inspect_uint         (const GList *list);

/**
 * gcut_list_equal_string:
 * @list1: a #GList of string to be compared.
 * @list2: a #GList of string to be compared.
 *
 * Compares two #GList, @list1 and @list2. @list1 and @list2
 * should be #GList of string.
 *
 * Returns: TRUE if all corresponding string of @list1 and
 * @list2 are same content string, FALSE otherwise.
 *
 * Since: 1.0.6
 */
gboolean         gcut_list_equal_string         (const GList *list1,
                                                 const GList *list2);

/**
 * gcut_list_inspect_string:
 * @list: a #GList of string to be inspected.
 *
 * Inspects @list. @list should be #GList of string. The
 * returned string should be freed when no longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.6
 */
gchar           *gcut_list_inspect_string       (const GList *list);

/**
 * gcut_list_inspect_object:
 * @list: a #GList of #GObject to be inspected.
 *
 * Inspects @list. @list should be #GList of #GObject. The
 * returned string should be freed when no longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.6
 */
gchar           *gcut_list_inspect_object       (const GList *list);

/**
 * gcut_list_inspect_enum:
 * @type: a #GEnum type.
 * @list: a #GList of enum value to be inspected.
 *
 * Inspects @list. @list should be #GList of enum value of
 * #GEnum. The returned string should be freed when no
 * longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.6
 */
gchar           *gcut_list_inspect_enum         (GType        type,
                                                 const GList *list);

/**
 * gcut_list_inspect_flags:
 * @type: a #GFlags type.
 * @list: a #GList of flags value to be inspected.
 *
 * Inspects @list. @list should be #GList of flags value of
 * #GFlags. The returned string should be freed when no
 * longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.6
 */
gchar           *gcut_list_inspect_flags        (GType        type,
                                                 const GList *list);

#ifndef CUTTER_DISABLE_DEPRECATED
/**
 * gcut_list_int_equal:
 * @list1: a #GList of #gint to be compared.
 * @list2: a #GList of #gint to be compared.
 *
 * Compares two #GList, @list1 and @list2. @list1 and @list2
 * should be #GList of #gint.
 *
 * Returns: TRUE if all corresponding integers of @list1 and
 * @list2 are same value, FALSE otherwise.
 *
 * Since: 1.0.5
 *
 * Deprecated: 1.0.6: Use gcut_list_equal_int() instead.
 */
#define gcut_list_int_equal(list1, list2)       \
    gcut_list_equal_int(list1, list2)

/**
 * gcut_list_int_inspect:
 * @list: a #GList of #gint to be inspected.
 *
 * Inspects @list. @list should be #GList of #gint. The
 * returned string should be freed when no longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.5
 *
 * Deprecated: 1.0.6: Use gcut_list_inspect_int() instead.
 */
#define gcut_list_int_inspect(list)             \
    gcut_list_inspect_int(list)

/**
 * gcut_list_uint_equal:
 * @list1: a #GList of #guint to be compared.
 * @list2: a #GList of #guint to be compared.
 *
 * Compares two #GList, @list1 and @list2. @list1 and @list2
 * should be #GList of #guint.
 *
 * Returns: TRUE if all corresponding unsigned integers of
 * @list1 and @list2 are same value, FALSE otherwise.
 *
 * Since: 1.0.5
 *
 * Deprecated: 1.0.6: Use gcut_list_equal_uint() instead.
 */
#define gcut_list_uint_equal(list1, list2)       \
    gcut_list_equal_uint(list1, list2)

/**
 * gcut_list_uint_inspect:
 * @list: a #GList of #guint to be inspected.
 *
 * Inspects @list. @list should be #GList of #guint. The
 * returned string should be freed when no longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.5
 *
 * Deprecated: 1.0.6: Use gcut_list_inspect_uint() instead.
 */
#define gcut_list_uint_inspect(list)            \
    gcut_list_inspect_uint(list)

/**
 * gcut_list_string_equal:
 * @list1: a #GList of string to be compared.
 * @list2: a #GList of string to be compared.
 *
 * Compares two #GList, @list1 and @list2. @list1 and @list2
 * should be #GList of string.
 *
 * Returns: TRUE if all corresponding string of @list1 and
 * @list2 are same content string, FALSE otherwise.
 *
 * Since: 1.0.5
 *
 * Deprecated: 1.0.6: Use gcut_list_equal_string() instead.
 */
#define gcut_list_string_equal(list1, list2)       \
    gcut_list_equal_string(list1, list2)

/**
 * gcut_list_string_inspect:
 * @list: a #GList of string to be inspected.
 *
 * Inspects @list. @list should be #GList of string. The
 * returned string should be freed when no longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.5
 *
 * Deprecated: 1.0.6: Use gcut_list_inspect_string() instead.
 */
#define gcut_list_string_inspect(list)          \
    gcut_list_inspect_string(list)

/**
 * gcut_list_object_inspect:
 * @list: a #GList of #GObject to be inspected.
 *
 * Inspects @list. @list should be #GList of #GObject. The
 * returned string should be freed when no longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.5
 *
 * Deprecated: 1.0.6: Use gcut_list_inspect_object() instead.
 */
#define gcut_list_object_inspect(list)          \
    gcut_list_inspect_object(list)

/**
 * gcut_list_enum_inspect:
 * @list: a #GList of enum value to be inspected.
 * @type: a #GEnum type.
 *
 * Inspects @list. @list should be #GList of enum value of
 * #GEnum. The returned string should be freed when no
 * longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.5
 *
 * Deprecated: 1.0.6: Use gcut_list_inspect_enum() instead.
 */
#define gcut_list_enum_inspect(list, type)      \
    gcut_list_inspect_enum(type, list)

/**
 * gcut_list_flags_inspect:
 * @list: a #GList of flags value to be inspected.
 * @type: a #GFlags type.
 *
 * Inspects @list. @list should be #GList of flags value of
 * #GFlags. The returned string should be freed when no
 * longer needed.
 *
 * Returns: inspected @list as a string.
 *
 * Since: 1.0.5
 *
 * Deprecated: 1.0.6: Use gcut_list_inspect_flags() instead.
 */
#define gcut_list_flags_inspect(list, type)     \
    gcut_list_inspect_flags(type, list)
#endif

G_END_DECLS

#endif /* __GCUT_LIST_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
