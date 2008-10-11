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

#define CUT_HELPER 1
#include "cuttest-assertions.h"

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
