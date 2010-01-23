/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2010  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __GCUT_DATA_HELPER_H__
#define __GCUT_DATA_HELPER_H__

#include <gcutter/gcut-dynamic-data.h>

G_BEGIN_DECLS

const gchar     *gcut_data_get_string_helper    (gconstpointer data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
gint             gcut_data_get_int_helper       (gconstpointer data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
guint            gcut_data_get_uint_helper      (gconstpointer data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
GType            gcut_data_get_type_helper      (gconstpointer data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
guint            gcut_data_get_flags_helper     (gconstpointer data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
gint             gcut_data_get_enum_helper      (gconstpointer data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
gconstpointer    gcut_data_get_pointer_helper   (gconstpointer data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
gconstpointer    gcut_data_get_boxed_helper     (gconstpointer data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);
gpointer         gcut_data_get_object_helper    (gconstpointer  data,
                                                 const gchar   *field_name,
                                                 CutCallbackFunction callback);

G_END_DECLS

#endif /* __GCUT_DATA_HELPER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
