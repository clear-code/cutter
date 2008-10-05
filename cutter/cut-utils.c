/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2008  Kouhei Sutou <kou@cozmixng.org>
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

#include <string.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <glib.h>
#include <glib/gstdio.h>

#include <errno.h>

#include "cut-utils.h"
#include "cut-sub-process.h"
#include "cut-sub-process-group.h"
#include "cut-diff.h"
#include "cut-main.h"
#include "../gcutter/gcut-public.h"
#include "../gcutter/gcut-error.h"

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
        GError *error = NULL;

        if (*filter[0] == '\0')
            continue;

        pattern = cut_utils_create_regex_pattern(*filter);
        regex = g_regex_new(pattern, 0, 0, &error);
        if (regex) {
            regexs = g_list_prepend(regexs, regex);
        } else {
            cut_utils_report_error(error);
        }
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

gchar *
cut_utils_inspect_memory (const void *memory, size_t size)
{
    const guchar *binary = memory;
    GString *buffer;
    size_t i;

    if (memory == NULL || size == 0)
        return g_strdup("(null)");

    buffer = g_string_sized_new(size * 5);
    for (i = 0; i < size; i++) {
        g_string_append_printf(buffer, "0x%02x ", binary[i]);
    }
    g_string_truncate(buffer, buffer->len - 1);

    return g_string_free(buffer, FALSE);
}

gboolean
cut_utils_equal_string_array (gchar **strings1, gchar **strings2)
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
cut_utils_get_fixture_data_string (CutTestContext *context,
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

        inspected = gcut_error_inspect(error);
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
                                            gchar **fixture_data_path,
                                            const gchar *path,
                                            ...)
{
    GError *error = NULL;
    va_list args, copied_args;
    const gchar *fixture_data;

    va_start(args, path);

    va_copy(copied_args, args);
    *fixture_data_path = cut_test_context_build_fixture_data_pathv(context,
                                                                   path,
                                                                   &copied_args);
    va_end(copied_args);

    fixture_data = cut_test_context_get_fixture_data_stringv(context, &error,
                                                             path, &args);
    *data = (gchar *)fixture_data;
    va_end(args);

    if (error) {
        gchar *inspected, *message;

        g_free(*fixture_data_path);

        inspected = gcut_error_inspect(error);
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
                                            boolean ? "true" : "false");
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
cut_utils_build_path (const gchar *path, ...)
{
    char *built_path;
    va_list args;

    va_start(args, path);
    built_path = cut_utils_build_pathv(path, &args);
    va_end(args);

    return built_path;
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

gboolean
cut_utils_remove_path (const char *path, GError **error)
{
    if (!g_file_test(path, G_FILE_TEST_EXISTS)) {
        g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_NOENT,
                    "path doesn't exist: %s", path);
        return FALSE;
    }

    if (g_file_test(path, G_FILE_TEST_IS_DIR)) {
        if (g_rmdir(path) == -1) {
            g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                        "can't remove directory: %s", path);
            return FALSE;
        }
    } else {
        if (g_unlink(path) == -1) {
            g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                        "can't remove path: %s", path);
            return FALSE;
        }
    }

    return TRUE;
}

gboolean
cut_utils_remove_path_recursive (const char *path, GError **error)
{
    if (g_file_test(path, G_FILE_TEST_IS_DIR)) {
        GDir *dir;
        const gchar *name;

        dir = g_dir_open(path, 0, error);
        if (!dir)
            return FALSE;

        while ((name = g_dir_read_name(dir))) {
            gchar *full_path;
            gboolean success;

            full_path = g_build_filename(path, name, NULL);
            success = cut_utils_remove_path_recursive(full_path, error);
            g_free(full_path);
            if (!success)
                return FALSE;
        }

        g_dir_close(dir);

        return cut_utils_remove_path(path, error);
    } else {
        return cut_utils_remove_path(path, error);
    }

    return TRUE;
}

void
cut_utils_remove_path_recursive_force (const gchar *path)
{
    cut_utils_remove_path_recursive(path, NULL);
}

gchar *
cut_utils_append_diff (const gchar *message, const gchar *from, const gchar *to)
{
    gchar *diff, *result;

    diff = cut_diff_readable(from, to);
    if (cut_diff_is_interested(diff)) {
        result = g_strdup_printf("%s\n"
                                 "\n"
                                 "diff:\n"
                                 "%s",
                                 message, diff);
        if (cut_diff_need_fold(diff)) {
            gchar *folded_diff, *original_result;

            original_result = result;
            folded_diff = cut_diff_folded_readable(from, to);
            result = g_strdup_printf("%s\n"
                                     "\n"
                                     "folded diff:\n"
                                     "%s",
                                     original_result, folded_diff);
            g_free(original_result);
            g_free(folded_diff);
        }
    } else {
        result = g_strdup(message);
    }
    g_free(diff);

    return result;
}

gchar *
cut_utils_fold (const gchar *string)
{
    GRegex *fold_re;
    GArray *folded_lines;
    gchar **lines, **line;
    gchar *folded_string;
    guint i;

    fold_re = g_regex_new("(.{78})", 0, 0, NULL);
    folded_lines = g_array_new(TRUE, FALSE, sizeof(gchar *));

    lines = g_regex_split_simple("\r?\n", string, 0, 0);
    for (line = lines; *line; line++) {
        gchar *folded_line;

        folded_line = g_regex_replace(fold_re, *line, -1, 0, "\\1\n", 0, NULL);
        g_array_append_val(folded_lines, folded_line);
    }
    g_strfreev(lines);

    folded_string = g_strjoinv("\n", (gchar **)(folded_lines->data));
    for (i = 0; i < folded_lines->len; i++) {
        gchar *folded_line;

        folded_line = g_array_index(folded_lines, gchar *, i);
        g_free(folded_line);
    }
    g_array_free(folded_lines, TRUE);
    g_regex_unref(fold_re);

    return folded_string;
}

CutSubProcess *
cut_utils_take_new_sub_process (const char     *test_directory,
                                CutTestContext *test_context)
{
    CutSubProcess *sub_process;

    sub_process = cut_sub_process_new(test_directory, test_context);
    cut_test_context_take_g_object(test_context, G_OBJECT(sub_process));
    return sub_process;
}

CutSubProcessGroup *
cut_utils_take_new_sub_process_group (CutTestContext *test_context)
{
    CutSubProcessGroup *group;

    group = cut_sub_process_group_new(test_context);
    cut_test_context_take_g_object(test_context, G_OBJECT(group));
    return group;
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
