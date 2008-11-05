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

#include <glib.h>
#include <string.h>

#include "gcut-enum.h"
#include "gcut-key-file.h"

gint
gcut_key_file_get_enum (GKeyFile *key_file,
                        const gchar *group_name, const gchar *key,
                        GType enum_type, GError **error)
{
    gchar *value;
    gint enum_value;
    GError *local_error = NULL;


    value = g_key_file_get_value(key_file, group_name, key, &local_error);
    if (local_error) {
        if (error)
            *error = local_error;
        return 0;
    }

    enum_value = gcut_enum_parse(enum_type, value, error);
    g_free(value);

    return enum_value;
}


guint
gcut_key_file_get_flags (GKeyFile *key_file,
                         const gchar *group_name, const gchar *key,
                         GType flags_type, GError **error)
{
    gchar *value;
    gint flags_value;
    GError *local_error = NULL;


    value = g_key_file_get_value(key_file, group_name, key, &local_error);
    if (local_error) {
        if (error)
            *error = local_error;
        return 0;
    }

    flags_value = gcut_flags_parse(flags_type, value, error);
    g_free(value);

    return flags_value;
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/

