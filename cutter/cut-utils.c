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
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
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
cut_utils_filter_to_regexs (const gchar **filter)
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

const gchar *
cut_utils_inspect_string (gchar *string)
{
    if (string)
        return string;
    else
        return "(null)";
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

void
cut_utils_append_indent (GString *string, guint size)
{
    guint i;

    for (i = 0; i < size; i++)
        g_string_append_c(string, ' ');
}

void
cut_utils_append_xml_element_with_value (GString *string, guint indent,
                                         const gchar *element_name,
                                         const gchar *value)
{
    gchar *escaped;

    cut_utils_append_indent(string, indent);
    escaped = g_markup_printf_escaped("<%s>%s</%s>\n",
                                      element_name,
                                      value,
                                      element_name);
    g_string_append(string, escaped);
    g_free(escaped);
}

void
cut_utils_append_xml_element_with_boolean_value (GString *string, guint indent,
                                                 const gchar *element_name,
                                                 gboolean boolean)
{
    cut_utils_append_xml_element_with_value(string, indent, element_name,
                                            boolean ? "TRUE" : "FALSE");
}

gchar **
cut_utils_strv_concat (const gchar **string_array, ...)
{
    guint length, i;
    guint args_length = 0;
    va_list args;
    gchar *string;
    gchar **new_string_array;

    if (!string_array)
        return NULL;

    length = g_strv_length((gchar **)string_array);
    va_start(args, string_array);
    string = va_arg(args, gchar*);
    while (string) {
        args_length++;
        string = va_arg(args, gchar*);
    }
    va_end(args);

    new_string_array = g_new(gchar*, length + args_length + 1);

    for (i = 0; i < length; i++) {
        new_string_array[i] = g_strdup(string_array[i]);
    }

    va_start(args, string_array);
    string = va_arg(args, gchar*);
    while (string) {
        new_string_array[i] = g_strdup(string);
        i++;
        string = va_arg(args, gchar*);
    }
    va_end(args);

    new_string_array[i] = NULL;

    return new_string_array;
}

void
cut_utils_close_pipe (int *pipe, CutPipeMode mode)
{
    if (pipe[mode] == -1)
        return;
    close(pipe[mode]);
    pipe[mode] = -1;
}

#ifdef G_OS_WIN32
static gchar *win32_base_path = NULL;

const gchar *
cut_win32_base_path (void)
{
    if (win32_base_path)
        return win32_base_path;

    win32_base_path = g_win32_get_package_installation_directory(PACKAGE, NULL);

    return win32_base_path;
}

gchar *
cut_win32_build_module_dir_name (const gchar *type)
{
    return g_build_filename(cut_win32_base_path(), "lib", PACKAGE,
                            "module", type, NULL);
}

gchar *
cut_win32_build_factory_module_dir_name (const gchar *type)
{
    gchar *module_dir, *factory_module_dir;

    module_dir = cut_win32_build_module_dir_name("factory");
    factory_module_dir = g_build_filename(module_dir, type, NULL);
    g_free(module_dir);
    return factory_module_dir;
}
#endif


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
