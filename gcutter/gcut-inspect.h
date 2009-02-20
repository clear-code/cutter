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

#ifndef __GCUT_INSPECT_H__
#define __GCUT_INSPECT_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-inspect
 * @title: Object inspection functions.
 * @short_description: Functions to inspect a object for
 * debugging.
 *
 * In test result, we need to know detail of inspected
 * objects for debugging. Functions of this section help us
 * to inspect interested objects.
 *
 * Since: 1.0.6
 */


/**
 * gcut_inspect_direct:
 * @string: the output string.
 * @data: the interested target.
 * @user_data: the data passed by user. (ignored)
 *
 * Shows @data as unsigned integer.
 *
 * e.g.:
 * |[
 * gcut_inspect_direct(string, GUINT_TO_POINTER(100), NULL) -> "100"
 * ]|
 *
 * Since: 1.0.6
 */
void        gcut_inspect_direct       (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);

/**
 * gcut_inspect_int:
 * @string: the output string.
 * @data: the interested target.
 * @user_data: the data passed by user. (ignored)
 *
 * Shows @data as integer.
 *
 * e.g.:
 * |[
 * gcut_inspect_nt(string, GINT_TO_POINTER(100), NULL) -> "100"
 * ]|
 *
 * Since: 1.0.6
 */
void        gcut_inspect_int          (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);

/**
 * gcut_inspect_uint:
 * @string: the output string.
 * @data: the interested target.
 * @user_data: the data passed by user. (ignored)
 *
 * Shows @data as unsigned integer.
 *
 * e.g.:
 * |[
 * gcut_inspect_uint(string, GUINT_TO_POINTER(100), NULL) -> "100"
 * ]|
 *
 * Since: 1.0.6
 */
void        gcut_inspect_uint         (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);

/**
 * gcut_inspect_string:
 * @string: the output string.
 * @data: the interested target.
 * @user_data: the data passed by user. (ignored)
 *
 * Shows @data as string. It also accepts %NULL.
 *
 * e.g.:
 * |[
 * gcut_inspect_string(string, "string", NULL) -> "\"string\""
 * ]|
 *
 * Since: 1.0.6
 */
void        gcut_inspect_string       (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);

/**
 * gcut_inspect_type:
 * @string: the output string.
 * @data: the interested target.
 * @user_data: the data passed by user. (ignored)
 *
 * Shows @data as %GType.
 *
 * e.g.:
 * |[
 * gcut_inspect_type(string, GTK_TYPE_WINDOW, NULL) -> "<Gtk::Window>" FIXME: confirm
 * ]|
 *
 * Since: 1.0.6
 */
void        gcut_inspect_type         (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);

/**
 * gcut_inspect_flags:
 * @string: the output string.
 * @data: the interested target.
 * @user_data: the data passed by user. (ignored)
 *
 * Shows @data as %GFlags.
 *
 * e.g.:
 * |[
 * FIXME
 * ]|
 *
 * Since: 1.0.6
 */
void        gcut_inspect_flags        (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);

/**
 * gcut_inspect_enum:
 * @string: the output string.
 * @data: the interested target.
 * @user_data: the data passed by user. (ignored)
 *
 * Shows @data as %GEnum.
 *
 * e.g.:
 * |[
 * FIXME
 * ]|
 *
 * Since: 1.0.6
 */
void        gcut_inspect_enum         (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);

/**
 * gcut_inspect_pointer
 * @string: the output string.
 * @data: the interested target.
 * @user_data: the data passed by user. (ignored)
 *
 * Shows @data as a pointer.
 *
 * e.g.:
 * |[
 * memory = malloc(1);
 * gcut_inspect_pointer(string, memory, NULL) -> "#<0xXXXXXXX>"
 * ]|
 *
 * Since: 1.0.6
 */
void        gcut_inspect_pointer      (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);

G_END_DECLS

#endif /* __GCUT_INSPECT_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
