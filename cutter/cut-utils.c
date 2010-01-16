/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2010  Kouhei Sutou <kou@clear-code.com>
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

#include <string.h>
#include <math.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <glib.h>
#include <glib/gstdio.h>
#ifdef G_OS_WIN32
#  include <windows.h>
#  include <io.h>
#  define close _close
#endif

#include <errno.h>

#include "cut-utils.h"
#include "cut-sub-process.h"
#include "cut-sub-process-group.h"
#include "cut-readable-differ.h"
#include "cut-main.h"
#include "cut-backtrace-entry.h"
#include "../gcutter/gcut-public.h"
#include "../gcutter/gcut-error.h"
#include "../gcutter/gcut-assertions-helper.h"

#ifndef CUT_DISABLE_SOCKET_SUPPORT
#  ifdef HAVE_WINSOCK2_H
#    include <winsock2.h>
#    include <ws2tcpip.h>
#  else
#    include <sys/types.h>
#    include <sys/socket.h>
#    include <arpa/inet.h>
#    ifdef HAVE_SYS_UN_H
#      include <sys/un.h>
#    endif
#  endif
#endif

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
    size_t i, n_printable_characters;
    size_t max_size = 1024;

    if (memory == NULL || size == 0)
        return g_strdup("(null)");

    buffer = g_string_sized_new(size * strlen("0xXX") +
                                (size - 1) * strlen(" ") +
                                strlen(": ") +
                                size);
    max_size = MIN(size, max_size);
    n_printable_characters = 0;
    for (i = 0; i < max_size; i++) {
        g_string_append_printf(buffer, "0x%02x ", binary[i]);
        if (g_ascii_isprint(binary[i]))
            n_printable_characters++;
    }
    if (size > max_size)
        g_string_append(buffer, "... ");

    if (n_printable_characters >= max_size * 0.3) {
        g_string_overwrite(buffer, buffer->len - 1, ": ");
        for (i = 0; i < max_size; i++) {
            if (g_ascii_isprint(binary[i])) {
                g_string_append_c(buffer, binary[i]);
            } else {
                g_string_append_c(buffer, '.');
            }
        }
        if (size > max_size)
            g_string_append(buffer, "...");
    } else {
        g_string_truncate(buffer, buffer->len - 1);
    }

    return g_string_free(buffer, FALSE);
}

gboolean
cut_utils_equal_string (const gchar *string1, const gchar *string2)
{
    if (string1 == string2)
        return TRUE;

    if (string1 == NULL || string2 == NULL)
        return FALSE;

    return g_str_equal(string1, string2);
}

gboolean
cut_utils_equal_substring (const gchar *string1, const gchar *string2,
                           size_t length)
{
    if (string1 == string2)
        return TRUE;

    if (string1 == NULL || string2 == NULL)
        return FALSE;

    return strncmp(string1, string2, length) == 0;
}

gboolean
cut_utils_equal_double (gdouble double1, gdouble double2, gdouble error)
{
    return fabs(double1 - double2) <= error;
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

#ifndef CUT_DISABLE_SOCKET_SUPPORT
gboolean
cut_utils_equal_sockaddr (const struct sockaddr *address1,
                          const struct sockaddr *address2)
{
    if (address1 == address2)
        return TRUE;
    if (!address1 || !address2)
        return FALSE;
    if (address1->sa_family != address2->sa_family)
        return FALSE;

    switch (address1->sa_family) {
#ifdef HAVE_SYS_UN_H
    case AF_UNIX:
    {
        struct sockaddr_un *address_unix1 = (struct sockaddr_un *)address1;
        struct sockaddr_un *address_unix2 = (struct sockaddr_un *)address2;

        return cut_utils_equal_string(address_unix1->sun_path,
                                      address_unix2->sun_path);
        break;
    }
#endif
    case AF_INET:
    {
        struct sockaddr_in *address_inet1 = (struct sockaddr_in *)address1;
        struct sockaddr_in *address_inet2 = (struct sockaddr_in *)address2;

        if (address_inet1->sin_addr.s_addr != address_inet2->sin_addr.s_addr)
            return FALSE;
        if (address_inet1->sin_port != address_inet2->sin_port)
            return FALSE;

        return TRUE;
        break;
    }
    case AF_INET6:
    {
        struct sockaddr_in6 *address_inet6_1 = (struct sockaddr_in6 *)address1;
        struct sockaddr_in6 *address_inet6_2 = (struct sockaddr_in6 *)address2;

        if (memcmp(address_inet6_1->sin6_addr.s6_addr,
                   address_inet6_2->sin6_addr.s6_addr,
                   sizeof(address_inet6_1->sin6_addr.s6_addr)) != 0)
            return FALSE;
        if (address_inet6_1->sin6_port != address_inet6_2->sin6_port)
            return FALSE;

        return TRUE;
        break;
    }
    case AF_UNSPEC:
        return TRUE;
        break;
    default:
        return FALSE;
        break;
    }

    return FALSE;
}

gchar *
cut_utils_inspect_sockaddr (const struct sockaddr *address)
{
    gchar *spec = NULL;

    if (!address)
        return g_strdup("(null)");

    switch (address->sa_family) {
#ifdef HAVE_SYS_UN_H
    case AF_UNIX:
    {
        struct sockaddr_un *address_unix = (struct sockaddr_un *)address;
        spec = g_strdup_printf("unix:%s", address_unix->sun_path);
        break;
    }
#endif
    case AF_INET:
    {
        struct sockaddr_in *address_inet = (struct sockaddr_in *)address;
        gchar ip_address_string[INET_ADDRSTRLEN];

        if (inet_ntop(AF_INET, &address_inet->sin_addr,
                      (gchar *)ip_address_string, INET_ADDRSTRLEN)) {
            spec = g_strdup_printf("inet:%s:%d",
                                   ip_address_string,
                                   g_ntohs(address_inet->sin_port));
        }
        break;
    }
    case AF_INET6:
    {
        struct sockaddr_in6 *address_inet6 = (struct sockaddr_in6 *)address;
        gchar ip_address_string[INET6_ADDRSTRLEN];

        if (inet_ntop(AF_INET6, &address_inet6->sin6_addr,
                      (gchar *)ip_address_string, INET6_ADDRSTRLEN)) {
            spec = g_strdup_printf("inet6:[%s]:%d",
                                   ip_address_string,
                                   g_ntohs(address_inet6->sin6_port));
        }
        break;
    }
    case AF_UNSPEC:
        spec = g_strdup("unknown");
        break;
    default:
        spec = g_strdup_printf("unexpected:%d", address->sa_family);
        break;
    }

    return spec;
}
#endif

gboolean
cut_utils_path_exist (const gchar *path)
{
    return g_file_test(path, G_FILE_TEST_EXISTS);
}

gboolean
cut_utils_regex_match (const gchar *pattern, const gchar *string)
{
    return g_regex_match_simple(pattern, string, G_REGEX_MULTILINE, 0);
}

gchar *
cut_utils_regex_replace (const gchar *pattern, const gchar *string,
                         const gchar *replacement, GError **error)
{
    GRegex *regex;
    gchar *replaced;

    regex = g_regex_new(pattern, G_REGEX_MULTILINE, 0, error);
    if (!regex)
        return NULL;

    replaced = g_regex_replace(regex, string, -1, 0, replacement, 0, error);
    g_regex_unref(regex);

    return replaced;
}

const gchar *
cut_utils_get_fixture_data_string (CutTestContext *context,
                                   gchar **fixture_data_path,
                                   const char *path,
                                   ...)
{
    const gchar *data;
    va_list args;

    va_start(args, path);
    data = cut_utils_get_fixture_data_string_va_list(context, fixture_data_path,
                                                     path, args);
    va_end(args); /* FIXME: should be freed */

    return data;
}

const gchar *
cut_utils_get_fixture_data_string_va_list (CutTestContext *context,
                                           gchar **fixture_data_path,
                                           const gchar *path,
                                           va_list args)
{
    GError *error = NULL;
    const gchar *fixture_data;

    fixture_data =
        cut_test_context_get_fixture_data_string_va_list(context, &error,
                                                         fixture_data_path,
                                                         path, args);
    if (error) {
        gchar *inspected, *message;

        g_free(*fixture_data_path);

        inspected = gcut_error_inspect(error);
        message = g_strdup_printf("can't get fixture data: %s", inspected);
        g_error_free(error);
        cut_test_context_register_result(context,
                                         CUT_TEST_RESULT_ERROR,
                                         message);
        g_free(inspected);
        g_free(message);
        cut_test_context_long_jump(context);
    }

    return fixture_data;
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
    built_path = cut_utils_build_path_va_list(path, args);
    va_end(args);

    return built_path;
}

gchar *
cut_utils_build_path_va_list (const gchar *path, va_list args)
{
    GArray *elements;
    gchar *element, *concatenated_path;

    if (!path)
        return NULL;

    elements = g_array_new(TRUE, FALSE, sizeof(char *));

    g_array_append_val(elements, path);
    while ((element = va_arg(args, gchar *))) {
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
cut_utils_expand_path_va_list (const gchar *path, va_list args)
{
    gchar *concatenated_path, *expanded_path;

    if (!path)
        return NULL;

    concatenated_path = cut_utils_build_path_va_list(path, args);
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
        gboolean success = TRUE;

        dir = g_dir_open(path, 0, error);
        if (!dir)
            return FALSE;

        while ((name = g_dir_read_name(dir))) {
            gchar *full_path;

            full_path = g_build_filename(path, name, NULL);
            success = cut_utils_remove_path_recursive(full_path, error);
            g_free(full_path);
            if (!success)
                break;
        }

        g_dir_close(dir);

        if (!success)
            return FALSE;
    }

    return cut_utils_remove_path(path, error);
}

void
cut_utils_remove_path_recursive_force (const gchar *path)
{
    cut_utils_remove_path_recursive(path, NULL);
}

void
cut_utils_make_directory_recursive_force (const gchar *path)
{
    g_mkdir_with_parents(path, 0700);
}

gchar *
cut_utils_append_diff (const gchar *message, const gchar *from, const gchar *to)
{
    gchar *diff, *result;

    diff = cut_diff_readable(from, to);
    if (cut_diff_readable_is_interested(diff)) {
        result = g_strdup_printf("%s\n"
                                 "\n"
                                 "diff:\n"
                                 "%s",
                                 message, diff);
        if (cut_diff_readable_need_fold(diff)) {
            gchar *folded_diff, *original_result;

            original_result = result;
            folded_diff = cut_diff_readable_folded(from, to);
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

typedef enum
{
    GDB_BACKTRACE_START,
    GDB_BACKTRACE_IN,
    GDB_BACKTRACE_FUNCTION,
    GDB_BACKTRACE_ARGUMENTS,
    GDB_BACKTRACE_ARGUMENTS_END,
    GDB_BACKTRACE_AT,
    GDB_BACKTRACE_LINE
} GdbBacktraceState;

GList *
cut_utils_parse_gdb_backtrace (const gchar *gdb_backtrace)
{
    GList *backtraces = NULL;

    while (gdb_backtrace && gdb_backtrace[0]) {
        CutBacktraceEntry *entry;
        gchar *file = NULL;
        guint line = 0;
        gchar *function = NULL;
        gchar *info = NULL;
        const gchar *start_point = NULL;
        GdbBacktraceState state = GDB_BACKTRACE_START;

        while (gdb_backtrace[0] && gdb_backtrace[0] != '\n') {
            switch (state) {
            case GDB_BACKTRACE_START:
                if (gdb_backtrace[0] == 'i' && gdb_backtrace[1] == 'n') {
                    gdb_backtrace++;
                    state = GDB_BACKTRACE_IN;
                }
                break;
            case GDB_BACKTRACE_IN:
                if (start_point) {
                    if (gdb_backtrace[0] == ' ') {
                        function = g_strndup(start_point,
                                             gdb_backtrace - start_point);
                        state = GDB_BACKTRACE_ARGUMENTS;
                        start_point = NULL;
                    }
                } else {
                    if (gdb_backtrace[0] != ' ')
                        start_point = gdb_backtrace;
                }
                break;
            case GDB_BACKTRACE_ARGUMENTS:
                if (gdb_backtrace[0] == ')')
                    state = GDB_BACKTRACE_ARGUMENTS_END;
                break;
            case GDB_BACKTRACE_ARGUMENTS_END:
                if (gdb_backtrace[0] == 'a' && gdb_backtrace[1] == 't') {
                    gdb_backtrace ++;
                    state = GDB_BACKTRACE_AT;
                }
                break;
            case GDB_BACKTRACE_AT:
                if (start_point) {
                    if (gdb_backtrace[0] == ':') {
                        file = g_strndup(start_point,
                                         gdb_backtrace - start_point);
                        state = GDB_BACKTRACE_LINE;
                        start_point = NULL;
                    }
                } else {
                    if (gdb_backtrace[0] != ' ')
                        start_point = gdb_backtrace;
                }
                break;
            case GDB_BACKTRACE_LINE:
                if (start_point) {
                    if (gdb_backtrace[1] == '\n') {
                        line = atoi(start_point);
                        start_point = NULL;
                    }
                } else {
                    start_point = gdb_backtrace;
                }
                break;
            default:
                break;
            }
            gdb_backtrace++;
        }

        entry = cut_backtrace_entry_new(file ? file : "unknown",
                                        line,
                                        function,
                                        info);
        backtraces = g_list_append(backtraces, entry);
        if (file)
            g_free(file);
        if (function)
            g_free(function);
        if (info)
            g_free(info);

        gdb_backtrace++;
    }

    return backtraces;
}

gchar *
cut_utils_double_to_string (gdouble value)
{
    gint i;
    gchar *string;

    string = g_strdup_printf("%f", value);
    for (i = 0; string[i]; i++) {
        if (string[i] == ',')
            string[i] = '.';
    }

    return string;
}

gint
cut_utils_compare_string (gconstpointer data1, gconstpointer data2)
{
    if (data1 == NULL && data2 == NULL)
        return 0;

    if (data1 == NULL)
        return -1;
    if (data2 == NULL)
        return 1;

    return strcmp(data1, data2);
}

gint
cut_utils_compare_direct (gconstpointer data1, gconstpointer data2)
{
    guint value1, value2;

    value1 = GPOINTER_TO_UINT(data1);
    value2 = GPOINTER_TO_UINT(data2);
    if (value1 == value2) {
        return 0;
    } else if (value1 < value2) {
        return -1;
    } else {
        return 1;
    }
}

#ifdef G_OS_WIN32
static gchar *win32_base_path = NULL;

const gchar *
cut_win32_base_path (void)
{
    if (win32_base_path)
        return win32_base_path;

    win32_base_path = g_win32_get_package_installation_directory_of_module(NULL);

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

gboolean
cut_win32_kill_process (GPid pid, guint exit_code)
{
    return TerminateProcess(pid, exit_code) ? TRUE : FALSE;
}
#endif


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
