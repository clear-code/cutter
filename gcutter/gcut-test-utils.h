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
 * @object: the GObject * to be owned by Cutter.
 *
 * Passes ownership of @object to Cutter and returns @object.
 *
 * Returns: @object owned by Cutter. Don't g_object_unref() it.
 *
 * Since: 1.0.3
 */
#define gcut_take_object(object)                                        \
    cut_test_context_take_g_object(get_current_test_context(), (object))

/**
 * gcut_take_error:
 * @error: the GError * to be owned by Cutter.
 *
 * Passes ownership of @error to Cutter and returns
 * a GError * that has same contents of @error.
 *
 * Returns: a GError * owned by Cutter. Don't g_error_free()
 * it.
 *
 * Since: 1.0.3
 */
#define gcut_take_error(error)                                          \
    cut_test_context_take_g_error(get_current_test_context(), (error))

/**
 * gcut_take_list:
 * @list: the GList * to be owned by Cutter.
 * @destroy_function: the destroy function (#CutDestroyFunction) that
 *                    destroys the elements of @list, or NULL.
 *
 * Passes ownership of @list to Cutter and returns
 * a GList * that has same elements of @list.
 *
 * Returns: a GList * owned by Cutter. Don't g_list_free()
 * it.
 *
 * Since: 1.0.3
 */
#define gcut_take_list(list, destroy_function)                          \
    cut_test_context_take_g_list(get_current_test_context(),            \
                                 (list), (destroy_function))

/**
 * gcut_list_string_new:
 * @value: the first string.
 * @...: remaining strings in list, terminated by NULL.
 *
 * Creates a list from passed strings.
 *
 * Returns: a newly-allocated GList * that contains passed
 * strings and must be freed with gcut_list_string_free().
 *
 * Since: 1.0.3
 */
GList  *gcut_list_string_new  (const gchar *value, ...) CUT_GNUC_NULL_TERMINATED;

/**
 * gcut_list_string_free:
 * @list: the list that contains strings to be freed.
 *
 * Frees @list and contained strings.
 *
 * Since: 1.0.3
 */
void   gcut_list_string_free  (GList *list);


G_END_DECLS

#endif /* __GCUT_TEST_UTILS_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
