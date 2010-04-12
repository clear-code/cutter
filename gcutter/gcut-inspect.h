/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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
 * @title: Object inspection functions
 * @short_description: Functions to inspect an object for
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
 * gint int_value = 100;
 * gcut_inspect_int(string, &int_value, NULL) -> "100"
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
 * guint uint_value = 100;
 * gcut_inspect_uint(string, &uint_value, NULL) -> "100"
 * ]|
 *
 * Since: 1.0.6
 */
void        gcut_inspect_uint         (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);

/**
 * gcut_inspect_size:
 * @string: the output string.
 * @data: the interested target.
 * @user_data: the data passed by user. (ignored)
 *
 * Shows @data as unsigned integer.
 *
 * e.g.:
 * |[
 * gsize size_value = 100;
 * gcut_inspect_size(string, &size_value, NULL) -> "100"
 * ]|
 *
 * Since: 1.1.3
 */
void        gcut_inspect_size         (GString       *string,
                                       gconstpointer  data,
                                       gpointer       user_data);

/**
 * gcut_inspect_char:
 * @string: the output string.
 * @data: the interested target.
 * @user_data: the data passed by user. (ignored)
 *
 * Shows @data as character.
 *
 * e.g.:
 * |[
 * gcut_inspect_char(string, 'C', NULL) -> "'C'"
 * gcut_inspect_char(string, '\0', NULL) -> "'\0'"
 * gcut_inspect_char(string, '\n', NULL) -> "'\n'"
 * ]|
 *
 * Since: 1.1.3
 */
void        gcut_inspect_char         (GString       *string,
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
 * gcut_inspect_type(string, GTK_TYPE_WINDOW, NULL) -> "&lt;GtkWindow&gt;"
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
 * @user_data: the pointer of GFlags type.
 *
 * Shows @data as value a GFlags type.
 *
 * e.g.:
 * |[
 * GType flags_type;
 * GtkWidgetFlags flags;
 *
 * flags_type = GTK_TYPE_WIDGET_FLAGS;
 * flags = GTK_TOPLEVEL | GTK_VISIBLE;
 * gcut_inspect_flags(string, &flags, &flags_type);
 * -> #&lt;GtkWidgetFlags: toplevel|visible (GTK_TOPLEVEL:0x10)|(GTK_VISIBLE:0x100)&gt;
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
 * @user_data: the pointer of GEnum type.
 *
 * Shows @data as value of a GEnum type.
 *
 * e.g.:
 * |[
 * GType enum_type;
 * GtkWidgetHelpType value;
 *
 * enum_type = GTK_TYPE_WIDGET_HELP_TYPE;
 * value = GTK_WIDGET_HELP_TOOLTIP;
 * gcut_inspect_enum(string, &value, &enum_type);
 * -> #&lt;GtkWidgetHelpType: tooltip(GTK_WIDGET_HELP_TOOLTIP:0)&gt;
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
