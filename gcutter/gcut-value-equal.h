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

#ifndef __GCUT_VALUE_EQUAL_H__
#define __GCUT_VALUE_EQUAL_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-value-equal
 * @title: Assertion Utilities for #GValue.
 * @short_description: Utilities to write assertions related
 * to #GValue more easily.
 *
 * To write assertions, you need to check equality and show
 * expected and actual values.
 *
 * The utilities help you to write assertions that are
 * related to #GValue.
 */

/**
 * gcut_value_equal:
 * @value1: a #GValue to be compared.
 * @value2: a #GValue to be compared.
 *
 * Compares two #GValue, @value1 and @value2 by registered
 * equal function. (See gcut_value_register_equal_func() for
 * detail of how to register equal function of #GValue)
 *
 * Returns: TRUE if @value1 equals to @value2, FALSE
 * otherwise.
 *
 * Since: 1.0.5
 */
gboolean         gcut_value_equal               (const GValue *value1,
                                                 const GValue *value2);

/**
 * gcut_value_register_equal_func:
 * @type1: a #GType of compared #GValue.
 * @type2: a #GType of compared #GValue.
 * @equal_func: a function that compares @type1's #GValue
 * with @type2's #GValue.
 *
 * e.g.:
 * |[
 * TODO
 * ]|
 *
 * Registers equality check function between #GValue of
 * @type1 and #GValue of @type2.
 *
 * Since: 1.0.5
 */
void             gcut_value_register_equal_func (GType type1,
                                                 GType type2,
                                                 GEqualFunc equal_func);


G_END_DECLS

#endif /* __GCUT_VALUE_EQUAL_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
