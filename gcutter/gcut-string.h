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

#ifndef __GCUT_STRING_H__
#define __GCUT_STRING_H__

#include <glib.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-string
 * @title: Assertion Utilities for GString
 * @short_description: Utilities to write assertions related
 * to #GString more easily.
 *
 * To write assertions, you need to check equality and show
 * expected and actual values.
 *
 * The utilities help you to write assertions that are
 * related to #GString.
 */

/**
 * gcut_string_equal:
 * @string1: a #GString to be compared.
 * @string2: a #GString to be compared.
 *
 * Compares two #GString, @string1 and @string2.
 *
 * Returns: %TRUE if both of @string1 and @string2 have the same
 * content, %FALSE otherwise.
 *
 * Since: 1.1.5
 */
gboolean  gcut_string_equal         (const GString *string1,
                                     const GString *string2);

/**
 * gcut_string_inspect:
 * @string: a #GString value.
 *
 * Inspects @string. The returned string should be freed when
 * no longer needed.
 *
 * Returns: inspected @string as a string.
 *
 * Since: 1.1.5
 */
gchar *gcut_string_inspect       (const GString *string);

G_END_DECLS

#endif /* __GCUT_STRING_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
