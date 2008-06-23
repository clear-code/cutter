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
#include "cut-gpublic.h"
#include "cut-main.h"

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
cut_utils_inspect_string (const gchar *string)
{
    if (string)
        return string;
    else
        return "(null)";
}

gchar *
cut_utils_inspect_g_error (GError *error)
{
    GString *inspected;

    if (!error)
        return g_strdup("No error");

    inspected = g_string_new(g_quark_to_string(error->domain));
    g_string_append_printf(inspected, ":%d", error->code);
    if (error->message) {
        g_string_append(inspected, ": ");
        g_string_append(inspected, error->message);
    }

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

const gchar *
cut_utils_get_fixture_data_string(CutTestContext *context,
                                  const char *function,
                                  const char *file,
                                  unsigned int line,
                                  const char *path,
                                  ...)
{
    GError *error = NULL;
    const gchar *data;
    va_list args;

    va_start(args, path);
    data = cut_test_context_get_fixture_data_stringv(context, &error,
                                                     path, &args);
    va_end(args);

    if (error) {
        gchar *inspected, *message;

        inspected = cut_utils_inspect_g_error(error);
        message = g_strdup_printf("can't get fixture data: %s", inspected);
        g_error_free(error);
        cut_test_context_register_result(context,
                                         CUT_TEST_RESULT_ERROR,
                                         function, file, line,
                                         message, NULL);
        g_free(inspected);
        g_free(message);
        cut_test_context_long_jump(context);
    }
    return data;
}

void
cut_utils_get_fixture_data_string_and_path (CutTestContext *context,
                                            const gchar *function,
                                            const gchar *file,
                                            unsigned int line,
                                            gchar **data,
                                            gchar **fixture_path,
                                            const gchar *path,
                                            ...)
{
    GError *error = NULL;
    va_list args, copied_args;
    const gchar *fixture_data;

    va_start(args, path);

    va_copy(copied_args, args);
    *fixture_path = cut_test_context_build_fixture_pathv(context,
                                                         path, &copied_args);
    va_end(copied_args);

    fixture_data = cut_test_context_get_fixture_data_stringv(context, &error,
                                                             path, &args);
    *data = (gchar *)fixture_data;
    va_end(args);

    if (error) {
        gchar *inspected, *message;

        inspected = cut_utils_inspect_g_error(error);
        message = g_strdup_printf("can't get fixture data: %s", inspected);
        g_error_free(error);
        cut_test_context_register_result(context,
                                         CUT_TEST_RESULT_ERROR,
                                         function, file, line,
                                         message, NULL);
        g_free(inspected);
        g_free(message);
        cut_test_context_long_jump(context);
    }
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

const gchar *
cut_utils_get_cutter_command_path (void)
{
    const gchar *cutter_command;

    cutter_command = g_getenv("CUTTER");
    if (cutter_command)
        return cutter_command;

    cutter_command = cut_get_cutter_command_path();
    if (cutter_command)
        return cutter_command;

    return g_get_prgname();
}

gchar *
cut_utils_build_pathv (const gchar *path, va_list *args)
{
    GArray *elements;
    gchar *element, *concatenated_path;

    if (!path)
        return NULL;

    elements = g_array_new(TRUE, FALSE, sizeof(char *));

    g_array_append_val(elements, path);
    while ((element = va_arg(*args, gchar *))) {
        g_array_append_val(elements, element);
    }
    concatenated_path = g_build_filenamev((gchar **)(elements->data));
    g_array_free(elements, TRUE);

    return concatenated_path;
}

gchar *
cut_utils_expand_path (const gchar *path)
{
    if (!path)
        return NULL;

    if (g_path_is_absolute(path)) {
        return g_strdup(path);
    } else {
        gchar *current_dir, *full_path;

        current_dir = g_get_current_dir();
        full_path = g_build_filename(current_dir, path, NULL);
        g_free(current_dir);

        return full_path;
    }
}

gchar *
cut_utils_expand_pathv (const gchar *path, va_list *args)
{
    gchar *concatenated_path, *expanded_path;

    if (!path)
        return NULL;

    concatenated_path = cut_utils_build_pathv(path, args);
    expanded_path = cut_utils_expand_path(concatenated_path);
    g_free(concatenated_path);
    return expanded_path;
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
