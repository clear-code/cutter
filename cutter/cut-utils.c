/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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

GList *
cut_utils_filter_to_regexs (gchar **filter)
{
    GList *regexs = NULL;

    for (; *filter; filter++) {
        GRegex *regex;
        gchar *pattern;

        if (*filter[0] == '\0')
            continue;

        pattern = cut_utils_create_regex_pattern(*filter);
        regex = g_regex_new(pattern, G_REGEX_EXTENDED, 0, NULL);
        if (regex)
            regexs = g_list_prepend(regexs, regex);
        g_free(pattern);
    }

    return regexs;
}

gboolean
cut_utils_filter_match (GList *regexs, const gchar *name)
{
    GList *node;

    for (node = regexs; node; node = g_list_next(node)) {
        GRegex *regex = node->data;

        if (g_regex_match(regex, name, 0, NULL))
            return TRUE;
    }

    return FALSE;
}

gboolean
cut_utils_compare_string_array (gchar **strings1, gchar **strings2)
{
    gint i, length;

    if (!strings1 && !strings2)
        return TRUE;

    if (!strings1 || !strings2)
        return FALSE;

    length = g_strv_length(strings1);

    if (length != g_strv_length(strings2))
        return FALSE;

    for (i = 0; i < length; i++) {
        if (strcmp(strings1[i], strings2[i]))
            return FALSE;
    }

    return TRUE;
}

gchar *
cut_utils_inspect_string_array (gchar **strings)
{
    GString *inspected;
    gchar **string, **next_string;

    if (!strings)
        return g_strdup("(null)");

    inspected = g_string_new("[");
    string = strings;
    while (*string) {
        g_string_append_printf(inspected, "\"%s\"", *string);
        next_string = string + 1;
        if (*next_string)
            g_string_append(inspected, ", ");
        string = next_string;
    }
    g_string_append(inspected, "]");

    return g_string_free(inspected, FALSE);
}

gboolean
cut_utils_is_interested_diff (const gchar *diff)
{
    if (!diff)
        return FALSE;

    if (!g_regex_match_simple("^[-+]", diff, G_REGEX_MULTILINE, 0))
        return FALSE;

    if (g_regex_match_simple("^[ ?]", diff, G_REGEX_MULTILINE, 0))
        return TRUE;

    if (g_regex_match_simple("(?:.*\n){2,}", diff, G_REGEX_MULTILINE, 0))
        return TRUE;

    return FALSE;
}

gboolean
cut_utils_file_exist (const gchar *path)
{
    return g_file_test(path, G_FILE_TEST_EXISTS);
}

gboolean
cut_utils_regex_match (const gchar *pattern, const gchar *string)
{
    return g_regex_match_simple(pattern, string, G_REGEX_MULTILINE, 0);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
