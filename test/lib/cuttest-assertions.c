/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@clear-code.com>
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

#include <cutter/cut-helper.h>
#include "cuttest-assertions.h"
#include <cutter/cut-backtrace-entry.h>

static GList *
cuttest_result_summary_list_new (guint n_tests,
                                 guint n_assertions,
                                 guint n_successes,
                                 guint n_failures,
                                 guint n_errors,
                                 guint n_pendings,
                                 guint n_notifications,
                                 guint n_omissions)
{
    GList *list = NULL;

#define APPEND(uint_value)                                      \
    list = g_list_append(list, GUINT_TO_POINTER(uint_value));   \

    APPEND(n_tests);
    APPEND(n_assertions);
    APPEND(n_successes);
    APPEND(n_failures);
    APPEND(n_errors);
    APPEND(n_pendings);
    APPEND(n_notifications);
    APPEND(n_omissions);

#undef APPEND

    return list;
}

static GList *
cuttest_result_summary_list_new_from_run_context (CutRunContext *run_context)
{
    return cuttest_result_summary_list_new(
        cut_run_context_get_n_tests(run_context),
        cut_run_context_get_n_assertions(run_context),
        cut_run_context_get_n_successes(run_context),
        cut_run_context_get_n_failures(run_context),
        cut_run_context_get_n_errors(run_context),
        cut_run_context_get_n_pendings(run_context),
        cut_run_context_get_n_notifications(run_context),
        cut_run_context_get_n_omissions(run_context));
}

static GList *
cuttest_result_string_list_new_va_list (const gchar *test_name,
                                        const gchar *user_message,
                                        const gchar *system_message,
                                        const gchar *message,
                                        const gchar *expected,
                                        const gchar *actual,
                                        const gchar *backtrace,
                                        va_list args)
{
    GList *strings = NULL;
    const gchar *file_line, *function;

#define APPEND(value)                           \
    strings = g_list_append(strings, g_strdup(value))

    APPEND(test_name);
    APPEND(user_message);
    APPEND(system_message);
    if (message) {
        APPEND(message);
    } else {
        GString *computed_message;

        computed_message = g_string_new(NULL);
        if (user_message)
            g_string_append(computed_message, user_message);

        if (system_message) {
            if (computed_message->len > 0)
                g_string_append(computed_message, "\n");
            g_string_append(computed_message, system_message);
        }

        if (expected) {
            if (computed_message->len > 0)
                g_string_append(computed_message, "\n");
            g_string_append_printf(computed_message, "expected: <%s>", expected);
        }

        if (actual) {
            if (computed_message->len > 0)
                g_string_append(computed_message, "\n");
            g_string_append_printf(computed_message, "  actual: <%s>", actual);
        }

        if (computed_message->len > 0)
            APPEND(computed_message->str);
        else
            APPEND(NULL);
        g_string_free(computed_message, TRUE);
    }
    APPEND(expected);
    APPEND(actual);

    file_line = backtrace;
    while (file_line) {
        function = va_arg(args, const gchar *);
        APPEND(file_line);
        APPEND(function);
        file_line = va_arg(args, const gchar *);
    }

#undef APPEND

    return strings;
}

static GList *
cuttest_result_string_list_new (const gchar *test_name,
                                const gchar *user_message,
                                const gchar *system_message,
                                const gchar *message,
                                const gchar *expected,
                                const gchar *actual,
                                const gchar *backtrace,
                                ...)
{
    GList *result_list;
    va_list args;

    va_start(args, backtrace);
    result_list = cuttest_result_string_list_new_va_list(test_name,
                                                         user_message,
                                                         system_message,
                                                         message,
                                                         expected,
                                                         actual,
                                                         backtrace,
                                                         args);
    va_end(args);

    return result_list;
}

static GList *
cuttest_result_string_list_new_from_result (CutTestResult *result)
{
    GList *result_list;
    const GList *backtrace, *node;

    result_list = cuttest_result_string_list_new(
        cut_test_result_get_test_name(result),
        cut_test_result_get_user_message(result),
        cut_test_result_get_system_message(result),
        cut_test_result_get_message(result),
        cut_test_result_get_expected(result),
        cut_test_result_get_actual(result),
        NULL);

    backtrace = cut_test_result_get_backtrace(result);
    for (node = backtrace; node; node = g_list_next(node)) {
        CutBacktraceEntry *entry = node->data;
        const gchar *file, *function_name;
        guint line;

        file = cut_backtrace_entry_get_file(entry);
        line = cut_backtrace_entry_get_line(entry);
        function_name = cut_backtrace_entry_get_function(entry);
        result_list = g_list_append(result_list, g_strdup_printf("%s:%u",
                                                                 file, line));
        result_list = g_list_append(result_list, g_strdup(function_name));
    }

    return result_list;
}

void
cut_assert_test_result_summary_helper(CutRunContext *run_context,
                                      guint n_tests,
                                      guint n_assertions,
                                      guint n_successes,
                                      guint n_failures,
                                      guint n_errors,
                                      guint n_pendings,
                                      guint n_notifications,
                                      guint n_omissions)
{
    GList *result_summary;
    const GList *expected_result_summary;
    const GList *actual_result_summary;

    result_summary =
        cuttest_result_summary_list_new(n_tests,
                                        n_assertions,
                                        n_successes,
                                        n_failures,
                                        n_errors,
                                        n_pendings,
                                        n_notifications,
                                        n_omissions);
    expected_result_summary = cut_take_result_summary_list(result_summary);

    result_summary =
        cuttest_result_summary_list_new_from_run_context(run_context);
    actual_result_summary = cut_take_result_summary_list(result_summary);

    gcut_assert_equal_list_uint(expected_result_summary, actual_result_summary);
}

void
cut_assert_test_result_helper(CutRunContext *run_context,
                              guint i,
                              CutTestResultStatus status,
                              const gchar *test_name,
                              const gchar *user_message,
                              const gchar *system_message,
                              const gchar *message,
                              const gchar *expected,
                              const gchar *actual,
                              const gchar *backtrace,
                              ...)
{
    const GList *results;
    CutTestResult *result;
    GList *strings = NULL;
    const GList *expected_strings, *actual_strings;
    va_list args;

    results = cut_run_context_get_results(run_context);
    cut_assert_operator_int(i, <, g_list_length((GList *)results));

    result = g_list_nth_data((GList *)results, i);
    cut_assert(result);
    cut_assert_equal_int(status, cut_test_result_get_status(result));

    va_start(args, backtrace);
    strings = cuttest_result_string_list_new_va_list(test_name,
                                                     user_message,
                                                     system_message,
                                                     message,
                                                     expected,
                                                     actual,
                                                     backtrace,
                                                     args);
    va_end(args);
    expected_strings = cut_take_result_string_list(strings);

    strings = cuttest_result_string_list_new_from_result(result);
    actual_strings = cut_take_result_string_list(strings);

    gcut_assert_equal_list_string(expected_strings, actual_strings);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
