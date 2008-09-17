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
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <string.h>

#include "gcut-enum.h"

gchar *
gcut_enum_inspect (GType enum_type, gint enum_value)
{
    GEnumClass *enum_class;
    GEnumValue *value;
    gchar *inspected;

    enum_class = g_type_class_ref(enum_type);
    if (!enum_class)
        return g_strdup_printf("unknown enum type: %s(%ld)",
                               g_type_name(enum_type), enum_type);

    value = g_enum_get_value(enum_class, enum_value);
    if (value) {
        inspected = g_strdup_printf("#<%s: %s(%s:%d)>",
                                    g_type_name(enum_type),
                                    value->value_nick,
                                    value->value_name,
                                    value->value);
    } else {
        inspected = g_strdup_printf("#<%s: %d>",
                                    g_type_name(enum_type), enum_value);
    }

    g_type_class_unref(enum_class);

    return inspected;
}

gchar *
gcut_flags_inspect (GType flags_type, guint flags)
{
    GFlagsClass *flags_class;
    guint i;
    GString *inspected;

    flags_class = g_type_class_ref(flags_type);
    if (!flags_class)
        return g_strdup_printf("unknown flags type: %s(%ld)",
                               g_type_name(flags_type), flags_type);


    inspected = g_string_new(NULL);
    g_string_append_printf(inspected, "#<%s", g_type_name(flags_type));
    if (flags > 0) {
        g_string_append(inspected, ":");
        if (flags & flags_class->mask) {
            g_string_append(inspected, " ");
            for (i = 0; i < flags_class->n_values; i++) {
                GFlagsValue *value = flags_class->values + i;
                if (value->value & flags)
                    g_string_append_printf(inspected, "%s|", value->value_nick);
            }
            inspected->str[inspected->len - 1] = ' ';
            for (i = 0; i < flags_class->n_values; i++) {
                GFlagsValue *value = flags_class->values + i;
                if (value->value & flags)
                    g_string_append_printf(inspected, "(%s:%u)|",
                                           value->value_name, value->value);
            }
            g_string_truncate(inspected, inspected->len - 1);
        }

        if (flags & ~(flags_class->mask)) {
            g_string_append_printf(inspected,
                                   " (unknown flags: 0x%x)",
                                   flags & ~(flags_class->mask));
        }
    }
    g_string_append(inspected, ">");

    g_type_class_unref(flags_class);

    return g_string_free(inspected, FALSE);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/

