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

#ifndef __GCUT_OBJECT_H__
#define __GCUT_OBJECT_H__

#include <glib-object.h>

#include <gcutter/gcut-types.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-object
 * @title: Assertion Utilities for GObject
 * @short_description: Utilities to write assertions related
 * to #GObject more easily.
 *
 * To write assertions, you need to check equality and show
 * expected and actual values.
 *
 * The utilities help you to write assertions that are
 * related to #GObject.
 */


/**
 * gcut_object_equal:
 * @object1: a #GObject to be compared.
 * @object2: a #GObject to be compared.
 * @equal_func: a function that compares two #GObject. NULL
 * is OK.
 *
 * Compares two #GObject, @object1 and @object2 by
 * @equal_func. If @equal_func is NULL, @object1 and
 * @object2 is just only compared by memory location.
 *
 * Returns: TRUE if @object1 == @object2 or
 * @equal_func(@object1, @object2), FALSE otherwise.
 *
 * Since: 1.0.5
 */
gboolean         gcut_object_equal                (const GObject *object1,
                                                   const GObject *object2,
                                                   GEqualFunc   equal_func);

/**
 * gcut_object_inspect:
 * @object: a #GObject.
 *
 * Inspects @object's property. The returned string should
 * be freed when no longer needed.
 *
 * Returns: inspected @object as a string.
 *
 * Since: 1.0.5
 */
gchar           *gcut_object_inspect              (const GObject *object);

/**
 * gcut_object_inspect_custom:
 * @object: a #GObject.
 * @inspect_func: a function that inspects @object.
 * @user_data: user data to pass to the function.
 *
 * Inspects @object by @inspect_func. The returned string
 * should be freed when no longer needed.
 *
 * Returns: inspected @object as a string.
 *
 * Since: 1.0.5
 */
gchar           *gcut_object_inspect_custom       (const GObject *object,
                                                   GCutInspectFunction inspect_func,
                                                   gpointer user_data);


G_END_DECLS

#endif /* __GCUT_OBJECT_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
