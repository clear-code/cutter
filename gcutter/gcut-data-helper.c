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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "gcut-data-helper.h"
#include "gcut-assertions-helper.h"

#define DEFINE_GETTER_HELPER(type_name, type)                           \
type                                                                    \
gcut_data_get_ ## type_name ## _helper (gconstpointer data,             \
                                        const gchar *field_name,        \
                                        CutCallbackFunction callback)   \
{                                                                       \
    GError *error = NULL;                                               \
    type value;                                                         \
                                                                        \
    value = gcut_dynamic_data_get_ ## type_name(GCUT_DYNAMIC_DATA(data), \
                                                field_name,             \
                                                &error);                \
    gcut_assert_error_helper(error, "error");                           \
    callback();                                                         \
                                                                        \
    return value;                                                       \
}

DEFINE_GETTER_HELPER(string, const gchar *)
DEFINE_GETTER_HELPER(uint, guint)
DEFINE_GETTER_HELPER(int, gint)
DEFINE_GETTER_HELPER(size, gsize)
#define gcut_data_get_data_type_helper gcut_data_get_type_helper
DEFINE_GETTER_HELPER(data_type, GType)
#undef gcut_data_get_data_type_helper
DEFINE_GETTER_HELPER(flags, guint)
DEFINE_GETTER_HELPER(enum, gint)
DEFINE_GETTER_HELPER(pointer, gconstpointer)
DEFINE_GETTER_HELPER(boxed, gconstpointer)
DEFINE_GETTER_HELPER(object, gpointer)

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
