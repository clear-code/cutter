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

#ifndef __GCUT_ERROR_H__
#define __GCUT_ERROR_H__

#include <glib.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-error
 * @title: Assertion Utilities for GError
 * @short_description: Utilities to write assertions related
 * to #GError more easily.
 *
 * To write assertions, you need to check equality and show
 * expected and actual values.
 *
 * The utilities help you to write assertions that are
 * related to #GError.
 */

/**
 * gcut_error_equal:
 * @error1: a #GError to be compared.
 * @error2: a #GError to be compared.
 *
 * Compares two #GError, @error1 and @error2.
 *
 * Returns: TRUE if both of @error1 and @error2 have the same
 * domain, code and message, FALSE otherwise.
 *
 * Since: 1.0.5
 */
gboolean  gcut_error_equal         (const GError *error1,
                                    const GError *error2);

/**
 * gcut_error_inspect:
 * @error: a #GError value.
 *
 * Inspects @error. The returned string should be freed when
 * no longer needed.
 *
 * Returns: inspected @error as a string.
 *
 * Since: 1.0.5
 */
gchar    *gcut_error_inspect       (const GError *error);

G_END_DECLS

#endif /* __GCUT_ERROR_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
