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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "gcut-inspect.h"

void
gcut_direct_inspect (GString *string, gconstpointer data, gpointer user_data)
{
    g_string_append_printf(string, "%u", GPOINTER_TO_UINT(data));
}

void
gcut_int_inspect (GString *string, gconstpointer data, gpointer user_data)
{
    const gint *int_value = data;

    g_string_append_printf(string, "%d", *int_value);
}

void
gcut_string_inspect (GString *string, gconstpointer data, gpointer user_data)
{
    const gchar *value = data;

    if (value)
        g_string_append_printf(string, "\"%s\"", value);
    else
        g_string_append(string, "NULL");
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/

