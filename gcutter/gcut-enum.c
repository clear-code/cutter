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

GQuark
gcut_enum_error_quark (void)
{
    return g_quark_from_static_string("gcut-enum-error-quark");
}


gchar *
gcut_enum_inspect (GType enum_type, gint enum_value)
{
    GEnumClass *enum_class;
    GEnumValue *value;
    gchar *inspected;

    enum_class = g_type_class_ref(enum_type);
    if (!enum_class)
        return g_strdup_printf("unknown enum type: %s(%" G_GSIZE_FORMAT ")",
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

gint
gcut_enum_parse (GType enum_type, const gchar *enum_value, GError **error)
{
    GEnumClass *enum_class;
    gint value = 0;
    guint i;
    gboolean found = FALSE;

    if (!enum_value) {
        g_set_error(error,
                    GCUT_ENUM_ERROR,
                    GCUT_ENUM_ERROR_INVALID_VALUE,
                    "enum value should not be NULL");
        return 0;
    }

    enum_class = g_type_class_ref(enum_type);
    if (!enum_class) {
        g_set_error(error,
                    GCUT_ENUM_ERROR,
                    GCUT_ENUM_ERROR_INVALID_TYPE,
                    "unknown enum type: %s(%" G_GSIZE_FORMAT ")",
                    g_type_name(enum_type), enum_type);
        return 0;
    }

    if (!G_TYPE_IS_ENUM(enum_type)) {
        g_set_error(error,
                    GCUT_ENUM_ERROR,
                    GCUT_ENUM_ERROR_INVALID_TYPE,
                    "invalid enum type: %s(%" G_GSIZE_FORMAT ")",
                    g_type_name(enum_type), enum_type);
        g_type_class_unref(enum_class);
        return 0;
    }

    for (i = 0; i < enum_class->n_values; i++) {
        GEnumValue *val;

        val = enum_class->values + i;
        if (g_ascii_strcasecmp(val->value_name, enum_value) == 0 ||
            g_ascii_strcasecmp(val->value_nick, enum_value) == 0) {
            found = TRUE;
            value = val->value;
            break;
        }
    }

    if (!found) {
        g_set_error(error,
                    GCUT_ENUM_ERROR,
                    GCUT_ENUM_ERROR_INVALID_VALUE,
                    "unknown enum value: <%s>(%s)",
                    enum_value, g_type_name(enum_type));
    }

    g_type_class_unref(enum_class);

    return value;
}

gchar *
gcut_flags_inspect (GType flags_type, guint flags)
{
    GFlagsClass *flags_class;
    guint i;
    GString *inspected;

    flags_class = g_type_class_ref(flags_type);
    if (!flags_class)
        return g_strdup_printf("unknown flags type: %s(%" G_GSIZE_FORMAT ")",
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
                    g_string_append_printf(inspected, "(%s:0x%x)|",
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

static GFlagsClass *
lookup_flags_class (GType flags_type, GError **error)
{
    GFlagsClass *flags_class;

    flags_class = g_type_class_ref(flags_type);
    if (!flags_class) {
        g_set_error(error,
                    GCUT_ENUM_ERROR,
                    GCUT_ENUM_ERROR_INVALID_TYPE,
                    "unknown flags type: %s(%" G_GSIZE_FORMAT ")",
                    g_type_name(flags_type), flags_type);
        return NULL;
    }

    if (!G_TYPE_IS_FLAGS(flags_type)) {
        g_set_error(error,
                    GCUT_ENUM_ERROR,
                    GCUT_ENUM_ERROR_INVALID_TYPE,
                    "invalid flags type: %s(%" G_GSIZE_FORMAT ")",
                    g_type_name(flags_type), flags_type);
        g_type_class_unref(flags_class);
        return NULL;
    }

    return flags_class;
}

guint
gcut_flags_parse (GType flags_type, const gchar *flags_value, GError **error)
{
    GFlagsClass *flags_class;
    guint value = 0;
    guint i;
    gchar **flags, **flag;
    GArray *unknown_flags;

    if (!flags_value) {
        g_set_error(error,
                    GCUT_ENUM_ERROR,
                    GCUT_ENUM_ERROR_INVALID_VALUE,
                    "flags value should not be NULL");
        return 0;
    }

    flags_class = lookup_flags_class(flags_type, error);
    if (!flags_class)
        return 0;

    unknown_flags = g_array_new(TRUE, TRUE, sizeof(gchar *));
    flags = g_strsplit(flags_value, "|", 0);
    for (flag = flags; *flag; flag++) {
        gboolean found = FALSE;

        for (i = 0; i < flags_class->n_values; i++) {
            GFlagsValue *val;

            val = flags_class->values + i;
            if (g_ascii_strcasecmp(val->value_name, *flag) == 0 ||
                g_ascii_strcasecmp(val->value_nick, *flag) == 0) {
                found = TRUE;
                value |= val->value;
                break;
            }
        }

        if (!found)
            g_array_append_val(unknown_flags, *flag);
    }

    if (unknown_flags->len > 0) {
        gchar *inspected_unknown_flags;
        gchar *inspected_flags;

        inspected_unknown_flags = g_strjoinv("|", (gchar **)unknown_flags->data);
        inspected_flags = gcut_flags_inspect(flags_type, value);
        g_set_error(error,
                    GCUT_ENUM_ERROR,
                    GCUT_ENUM_ERROR_INVALID_VALUE,
                    "unknown flags: <%s>(%s): <%s>: %s",
                    inspected_unknown_flags, g_type_name(flags_type),
                    flags_value, inspected_flags);
        g_free(inspected_unknown_flags);
        g_free(inspected_flags);
    }
    g_array_free(unknown_flags, TRUE);
    g_strfreev(flags);

    g_type_class_unref(flags_class);

    return value;
}

guint
gcut_flags_get_all (GType flags_type, GError **error)
{
    GFlagsClass *flags_class;
    guint all_available_values;

    flags_class = lookup_flags_class(flags_type, error);
    if (!flags_class)
        return 0;

    all_available_values = flags_class->mask;

    g_type_class_unref(flags_class);

    return all_available_values;
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/

