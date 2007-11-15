/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <glib.h>

#include "cut-utils.h"
#include "cut-public.h"

gchar *
cut_utils_create_regex_pattern (const gchar *string)
{
    gchar *pattern;

    if (!string) {
        pattern = g_strdup(".*");
    } else if (strlen(string) > 1 &&
        g_str_has_prefix(string, "/") && g_str_has_suffix(string, "/")) {
        pattern = g_strndup(string + 1, strlen(string) - 2);
    } else {
        gchar *escaped_string;
        escaped_string = g_regex_escape_string(string, -1);
        pattern = g_strdup_printf("^%s$", escaped_string);
        g_free(escaped_string);
    }

    return pattern;
}

gboolean
cut_utils_compare_string_array (const gchar **strings1, const gchar **strings2)
{
    gint i, length;

    length = g_strv_length((gchar **)strings1);

    if (length != g_strv_length((gchar **)strings2))
        return FALSE;

    for (i = 0; i < length; i++) {
        if (!strings1[i] || !strings2[i])
            return FALSE;
        if (strcmp(strings1[i], strings2[i]))
            return FALSE;
    }

    return TRUE;
}

GList *
cut_test_list_intersection (GList *list1, GList *list2)
{
    GList *ret = NULL, *node;

    for (node = list1; node; node = g_list_next(node)) {
        if (g_list_find(list2, node->data))
            ret = g_list_prepend(ret, node->data);
    }

    return ret;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
