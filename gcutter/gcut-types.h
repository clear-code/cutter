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

#ifndef __GCUT_TYPES_H__
#define __GCUT_TYPES_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-types
 * @title: Types for GLib support
 * @short_description: Types that is used in test with GLib support.
 *
 * There are some types to be used in test with GLib support.
 */

/**
 * GCutInspectFunction:
 * @string: the #GString to be stored inspected result.
 * @data: the data element to be inspected.
 * @user_data: user data to pass to the function.
 *
 * Specifies the type of function which is called when the
 * @data element is inspected. It is passed the pointer to
 * the data element, should inspect @data and append
 * inspected string to @string.
 *
 * Since: 1.0.5
 */
typedef void    (*GCutInspectFunction) (GString *string,
                                        gconstpointer data,
                                        gpointer user_data);

#ifndef CUTTER_DISABLE_DEPRECATED
#  define GCutInspectFunc GCutInspectFunction
#endif

G_END_DECLS

#endif /* __GCUT_TYPES_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
