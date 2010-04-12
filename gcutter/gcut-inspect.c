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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "gcut-inspect.h"
#include "gcut-enum.h"

void
gcut_inspect_direct (GString *string, gconstpointer data, gpointer user_data)
{
    g_string_append_printf(string, "%u", GPOINTER_TO_UINT(data));
}

void
gcut_inspect_int (GString *string, gconstpointer data, gpointer user_data)
{
    const gint *int_value = data;

    g_string_append_printf(string, "%d", *int_value);
}

void
gcut_inspect_uint (GString *string, gconstpointer data, gpointer user_data)
{
    const guint *uint_value = data;

    g_string_append_printf(string, "%u", *uint_value);
}

void
gcut_inspect_size (GString *string, gconstpointer data, gpointer user_data)
{
    const gsize *size_value = data;

    g_string_append_printf(string, "%" G_GSIZE_FORMAT, *size_value);
}

void
gcut_inspect_char (GString *string, gconstpointer data, gpointer user_data)
{
    const gchar *value = data;

    switch (value[0]) {
    case '\a':
        g_string_append(string, "'\\a'");
        break;
    case '\b':
        g_string_append(string, "'\\b'");
        break;
    case '\t':
        g_string_append(string, "'\\t'");
        break;
    case '\n':
        g_string_append(string, "'\\n'");
        break;
    case '\v':
        g_string_append(string, "'\\v'");
        break;
    case '\f':
        g_string_append(string, "'\\f'");
        break;
    case '\r':
        g_string_append(string, "'\\r'");
        break;
    case '\e':
        g_string_append(string, "'\\e'");
        break;
    case '\'':
        g_string_append(string, "'\\''");
        break;
    case '\\':
        g_string_append(string, "'\\\\'");
        break;
    default:
        if (0x20 <= value[0] && value[0] <= 0x7e) {
            g_string_append_printf(string, "\'%c\'", value[0]);
        } else {
            g_string_append_printf(string, "'\\%d'", value[0]);
        }
        break;
    }
}

void
gcut_inspect_string (GString *string, gconstpointer data, gpointer user_data)
{
    const gchar *value = data;

    if (value)
        g_string_append_printf(string, "\"%s\"", value);
    else
        g_string_append(string, "NULL");
}

void
gcut_inspect_type (GString *string, gconstpointer data, gpointer user_data)
{
    const GType *type = data;

    g_string_append_printf(string, "<%s>", g_type_name(*type));
}

void
gcut_inspect_flags (GString *string, gconstpointer data, gpointer user_data)
{
    const guint *flags = data;
    GType *flags_type = user_data;
    gchar *inspected_flags;

    inspected_flags = gcut_flags_inspect(*flags_type, *flags);
    g_string_append(string, inspected_flags);
    g_free(inspected_flags);
}

void
gcut_inspect_enum (GString *string, gconstpointer data, gpointer user_data)
{
    const gint *enum_value = data;
    GType *enum_type = user_data;
    gchar *inspected_enum;

    inspected_enum = gcut_enum_inspect(*enum_type, *enum_value);
    g_string_append(string, inspected_enum);
    g_free(inspected_enum);
}

void
gcut_inspect_pointer (GString *string, gconstpointer data, gpointer user_data)
{
    g_string_append_printf(string, "#<%p>", data);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/

