/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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

#include <string.h>

#include <gcutter.h>
#include <cutter/cut-utils.h>
#include <cutter/cut-backtrace-entry.h>
#include "../lib/cuttest-utils.h"

void test_inspect_memory (void);
void test_inspect_memory_with_printable (void);
void test_compare_string_array (void);
void test_inspect_string_array (void);
void test_strv_concat (void);
void test_remove_path_recursive (void);
void test_fold (void);
void test_equal_string (void);
void test_equal_substring (void);
void test_equal_double (void);
void data_parse_gdb_backtrace (void);
void test_parse_gdb_backtrace (gconstpointer data);
void test_compare_string (void);
void test_compare_direct (void);

static gchar *tmp_dir;
static gchar **actual_string_array;

void
cut_setup (void)
{
    tmp_dir = g_build_filename(cuttest_get_base_dir(), "tmp", NULL);
    cut_remove_path(tmp_dir, NULL);

    if (g_mkdir_with_parents(tmp_dir, 0700) == -1)
        cut_error_errno();

    actual_string_array = NULL;
}

void
cut_teardown (void)
{
    if (tmp_dir) {
        cut_remove_path(tmp_dir, NULL);
        g_free(tmp_dir);
    }

    if (actual_string_array)
        g_strfreev(actual_string_array);
}

void
test_inspect_memory (void)
{
    gchar binary[] = {0x00, 0x01, 0x02, 0x1f};
    gchar empty[] = {};

    cut_assert_equal_string_with_free("(null)",
                                      cut_utils_inspect_memory(empty, 0));
    cut_assert_equal_string_with_free("(null)",
                                      cut_utils_inspect_memory(NULL, 100));
    cut_assert_equal_string_with_free("0x00 0x01 0x02 0x1f",
                                      cut_utils_inspect_memory(binary,
                                                               sizeof(binary)));
    cut_assert_equal_string_with_free("0x00 0x01",
                                      cut_utils_inspect_memory(binary, 2));
}

void
test_inspect_memory_with_printable (void)
{
    gchar binary[] = {0x00, 0x01, 0x02, 0x1f, 'G', 'N', 'U', 0x01};

    cut_assert_equal_string_with_free(
        "0x00 0x01 0x02 0x1f 0x47 0x4e 0x55 0x01: ....GNU.",
        cut_utils_inspect_memory(binary, sizeof(binary)));

    cut_assert_equal_string_with_free(
        "0x00 0x01 0x02 0x1f 0x47",
        cut_utils_inspect_memory(binary, sizeof(binary) - 3));

    cut_assert_equal_string_with_free(
        "0x00 0x01 0x02 0x1f 0x47 0x4e: ....GN",
        cut_utils_inspect_memory(binary, sizeof(binary) - 2));
}

void
test_compare_string_array (void)
{
    gchar *strings1[] = {"a", "b", "c", NULL};
    gchar *strings2[] = {"a", "b", "c", NULL};
    gchar *strings3[] = {"a", "b", NULL};
    gchar *strings4[] = {"a", "b", "d", NULL};

    cut_assert_true(cut_utils_equal_string_array(strings1, strings2));
    cut_assert_false(cut_utils_equal_string_array(NULL, strings2));
    cut_assert_true(cut_utils_equal_string_array(NULL, NULL));
    cut_assert_false(cut_utils_equal_string_array(strings1, strings3));
    cut_assert_false(cut_utils_equal_string_array(strings1, strings4));
}


void
test_inspect_string_array (void)
{
    gchar *strings[] = {"a", "b", "c", NULL};

    cut_assert_equal_string_with_free("(null)",
                                      cut_utils_inspect_string_array(NULL));
    cut_assert_equal_string_with_free("[\"a\", \"b\", \"c\"]",
                                      cut_utils_inspect_string_array(strings));
}

void
test_strv_concat (void)
{
    const gchar *strings[] = {"a", "b", "c", NULL};
    gchar *expected[] = {"a", "b", "c", "d", "e", NULL};

    actual_string_array = cut_utils_strv_concat(strings, "d", "e", NULL);
    cut_assert_equal_string_array(expected, actual_string_array);
}

void
test_remove_path_recursive (void)
{
    GError *error = NULL;
    const gchar *file, *sub_dir, *sub_file;

    file = cut_take_string(g_build_filename(tmp_dir, "file", NULL));
    sub_dir = cut_take_string(g_build_filename(tmp_dir, "sub", NULL));
    sub_file = cut_take_string(g_build_filename(sub_dir, "file", NULL));

    cut_assert_path_exist(tmp_dir);
    cut_assert_path_not_exist(file);
    cut_assert_path_not_exist(sub_dir);
    cut_assert_path_not_exist(sub_file);

    g_file_set_contents(file, "top file", -1, &error);
    gcut_assert_error(error);

    if (g_mkdir_with_parents(sub_dir, 0700) == -1)
        cut_assert_errno();

    g_file_set_contents(sub_file, "sub file", -1, &error);
    gcut_assert_error(error);

    cut_utils_remove_path_recursive(tmp_dir, &error);
    gcut_assert_error(error);

    cut_assert_path_not_exist(sub_file);
    cut_assert_path_not_exist(sub_dir);
    cut_assert_path_not_exist(file);
    cut_assert_path_not_exist(tmp_dir);
}

void
test_fold (void)
{
    cut_assert_equal_string_with_free("0123456789"
                                      "1123456789"
                                      "2123456789"
                                      "3123456789"
                                      "4123456789"
                                      "5123456789"
                                      "6123456789"
                                      "71234567\n"
                                      "89"
                                      "8123456789",
                                      cut_utils_fold("0123456789"
                                                     "1123456789"
                                                     "2123456789"
                                                     "3123456789"
                                                     "4123456789"
                                                     "5123456789"
                                                     "6123456789"
                                                     "7123456789"
                                                     "8123456789"));
}

void
test_equal_string (void)
{
    cut_assert_true(cut_utils_equal_string(NULL, NULL));
    cut_assert_true(cut_utils_equal_string("string", "string"));
    cut_assert_false(cut_utils_equal_string("string", NULL));
    cut_assert_false(cut_utils_equal_string(NULL, "string"));
}

void
test_equal_substring (void)
{
    cut_assert_true(cut_utils_equal_substring(NULL, NULL, 10));
    cut_assert_true(cut_utils_equal_substring("string", "string-garbage",
                                              strlen("string")));
    cut_assert_false(cut_utils_equal_substring("string", "string-garbage",
                                               strlen("string") + 1));
    cut_assert_false(cut_utils_equal_substring("string", NULL, 5));
    cut_assert_false(cut_utils_equal_substring(NULL, "string", 5));
}

void
test_equal_double (void)
{
    cut_assert_true(cut_utils_equal_double(0, 0, 0.1));
    cut_assert_true(cut_utils_equal_double(0.11, 0.19, 0.1));
    cut_assert_true(cut_utils_equal_double(0.11, 0.12, 0.01));
    cut_assert_false(cut_utils_equal_double(0.11, 0.12, 0.009));
}

static GList *
backtraces_new (const gchar *file, ...)
{
    GList *backtraces = NULL;
    va_list args;

    va_start(args, file);
    while (file) {
        guint line;
        const gchar *function;
        const gchar *info;
        CutBacktraceEntry *entry;

        line = va_arg(args, guint);
        function = va_arg(args, gchar *);
        info = va_arg(args, gchar *);
        entry = cut_backtrace_entry_new(file, line, function, info);
        backtraces = g_list_append(backtraces, entry);

        file = va_arg(args, gchar *);
    }
    va_end(args);

    return backtraces;
}

static void
backtraces_free (GList *backtraces)
{
    g_list_foreach(backtraces, (GFunc)g_object_unref, NULL);
    g_list_free(backtraces);
}

void
data_parse_gdb_backtrace (void)
{
#define ADD(label, expected, gdb_backtrace)                             \
    gcut_add_datum(label,                                               \
                   "expected", G_TYPE_POINTER, expected, backtraces_free, \
                   "gdb-backtrace", G_TYPE_STRING, gdb_backtrace,       \
                   NULL)

    ADD("NULL", NULL, NULL);
    ADD("known line",
        backtraces_new("test-cut-stream-parser.c", 1099, "test_crash_test", NULL,
                       NULL),
        "#4  0x00007fd67b4fbfc5 in test_crash_test () at test-cut-stream-parser.c:1099\n");
    ADD("unknown line",
        backtraces_new("unknown", 0, "cut_test_run", NULL,
                       NULL),
        "#5  0x00007fd68285ea77 in cut_test_run (test=0xfc0e30, test_context=0xf90840, \n");
    ADD("full",
        backtraces_new("test-cut-stream-parser.c", 1099, "test_crash_test", NULL,
                       "unknown", 0, "cut_test_run", NULL,
                       "unknown", 0, "run", NULL,
                       "unknown", 0, "cut_test_case_run_with_filter", NULL,
                       "cut-test-suite.c", 129, "run", NULL,
                       "unknown", 0, "cut_test_suite_run_test_cases", NULL,
                       "unknown", 0, "cut_test_suite_run_with_filter", NULL,
                       "cut-runner.c", 67, "cut_runner_run", NULL,
                       "unknown", 0, "cut_run_context_start", NULL,
                       "unknown", 0, "cut_start_run_context", NULL,
                       "cut-main.c", 317, "cut_run", NULL,
                       NULL),
        "#4  0x00007fd67b4fbfc5 in test_crash_test () at test-cut-stream-parser.c:1099\n"
        "#5  0x00007fd68285ea77 in cut_test_run (test=0xfc0e30, test_context=0xf90840, \n"
        "#6  0x00007fd682860cc4 in run (test_case=0xfb3400, test=0xfc0e30, \n"
        "#7  0x00007fd682860e9d in cut_test_case_run_with_filter (test_case=0xfb3400, \n"
        "#8  0x00007fd682862c66 in run (data=0xfc3560) at cut-test-suite.c:129\n"
        "#9  0x00007fd68286313e in cut_test_suite_run_test_cases (test_suite=0xf88c60, \n"
        "#10 0x00007fd6828631e0 in cut_test_suite_run_with_filter (test_suite=0xf88c60, \n"
        "#11 0x00007fd68285dbe8 in cut_runner_run (runner=0xf8d840) at cut-runner.c:67\n"
        "#12 0x00007fd68285bc7f in cut_run_context_start (context=0xf8d840)\n"
        "#13 0x00007fd68285e072 in cut_start_run_context (run_context=0xf8d840)\n"
        "#14 0x00007fd68285e1be in cut_run () at cut-main.c:317\n");

#undef ADD
}

static gboolean
backtrace_entry_equal (gconstpointer data1, gconstpointer data2)
{
    CutBacktraceEntry *entry1;
    CutBacktraceEntry *entry2;

    entry1 = CUT_BACKTRACE_ENTRY(data1);
    entry2 = CUT_BACKTRACE_ENTRY(data2);

    if (!cut_utils_equal_string(cut_backtrace_entry_get_file(entry1),
                                cut_backtrace_entry_get_file(entry2)))
        return FALSE;

    if (cut_backtrace_entry_get_line(entry1) !=
        cut_backtrace_entry_get_line(entry2))
        return FALSE;

    if (!cut_utils_equal_string(cut_backtrace_entry_get_function(entry1),
                                cut_backtrace_entry_get_function(entry2)))
        return FALSE;

    if (!cut_utils_equal_string(cut_backtrace_entry_get_info(entry1),
                                cut_backtrace_entry_get_info(entry2)))
        return FALSE;

    return TRUE;
}

void
test_parse_gdb_backtrace (gconstpointer data)
{
    const gchar *gdb_backtrace;
    GList *backtraces;
    const GList *expected_backtraces;

    gdb_backtrace = gcut_data_get_string(data, "gdb-backtrace");
    backtraces = cut_utils_parse_gdb_backtrace(gdb_backtrace);
    gcut_take_list(backtraces, (CutDestroyFunction)g_object_unref);

    expected_backtraces = gcut_data_get_pointer(data, "expected");
    gcut_assert_equal_list_object_custom(backtraces,
                                         expected_backtraces,
                                         backtrace_entry_equal);
}

void
test_compare_string (void)
{
    cut_assert_equal_int(0, cut_utils_compare_string(NULL, NULL));
    cut_assert_equal_int(-1, cut_utils_compare_string(NULL, "a"));
    cut_assert_equal_int(1, cut_utils_compare_string("a", NULL));
    cut_assert_equal_int(-1, cut_utils_compare_string("a", "b"));
    cut_assert_equal_int(1, cut_utils_compare_string("b", "a"));
}

void
test_compare_direct (void)
{
    guint data1 = 10;
    guint data2 = 20;

    cut_assert_equal_int(0, cut_utils_compare_direct(GUINT_TO_POINTER(data1),
                                                     GUINT_TO_POINTER(data1)));
    cut_assert_equal_int(-1, cut_utils_compare_direct(GUINT_TO_POINTER(data1),
                                                      GUINT_TO_POINTER(data2)));
    cut_assert_equal_int(1, cut_utils_compare_direct(GUINT_TO_POINTER(data2),
                                                     GUINT_TO_POINTER(data1)));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
