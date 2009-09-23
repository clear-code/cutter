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

#ifndef __CUTTEST_ASSERTIONS_H__
#define __CUTTEST_ASSERTIONS_H__

#include <gcutter.h>
#include <cutter/cut-assertions.h>
#include <cutter/cut-run-context.h>
#include <cutter/cut-test-result.h>
#include "cuttest-utils.h"

#define cut_take_result_summary_list(list)      \
    gcut_take_list(list, NULL)

#define cut_take_result_string_list(list)       \
    gcut_take_list(list, g_free)

#define cut_assert_test_result_summary(run_context, n_tests,            \
                                       n_assertions, n_successes,       \
                                       n_failures, n_errors,            \
                                       n_pendings, n_notifications,     \
                                       n_omissions)                     \
    cut_trace_with_info_expression(                                     \
        cut_assert_test_result_summary_helper(run_context, n_tests,     \
                                              n_assertions,             \
                                              n_successes,              \
                                              n_failures, n_errors,     \
                                              n_pendings,               \
                                              n_notifications,          \
                                              n_omissions),             \
        cut_assert_test_result_summary(run_context, n_tests,            \
                                       n_assertions, n_successes,       \
                                       n_failures, n_errors,            \
                                       n_pendings, n_notifications,     \
                                       n_omissions))

#define cut_assert_test_result_with_message(run_context, i, status,     \
                                            test_name, user_message,    \
                                            system_message, message,    \
                                            expected, actual,           \
                                            backtrace, ...)             \
    cut_trace_with_info_expression(                                     \
        cut_assert_test_result_helper(run_context, i, status,           \
                                      test_name, user_message,          \
                                      system_message, message,          \
                                      expected, actual,                 \
                                      backtrace, ## __VA_ARGS__, NULL), \
        cut_assert_test_result_with_message(run_context, i, status,     \
                                            test_name, user_message,    \
                                            system_message, message,    \
                                            expected, actual,           \
                                            backtrace, ## __VA_ARGS__))

#define cut_assert_test_result(run_context, i, status, test_name,       \
                               user_message, system_message,            \
                               expected, actual,                        \
                               backtrace, ...)                          \
    cut_trace_with_info_expression(                                     \
        cut_assert_test_result_helper(run_context, i, status,           \
                                      test_name, user_message,          \
                                      system_message, NULL,             \
                                      expected, actual,                 \
                                      backtrace, ## __VA_ARGS__, NULL), \
        cut_assert_test_result(run_context, i, status, test_name,       \
                               user_message, system_message,            \
                               expected, actual,                        \
                               backtrace, ## __VA_ARGS__))


void cut_assert_test_result_summary_helper (CutRunContext *run_context,
                                            guint n_tests,
                                            guint n_assertions,
                                            guint n_successes,
                                            guint n_failures,
                                            guint n_errors,
                                            guint n_pendings,
                                            guint n_notifications,
                                            guint n_omissions);
void cut_assert_test_result_helper         (CutRunContext *run_context,
                                            guint i,
                                            CutTestResultStatus status,
                                            const gchar *test_name,
                                            const gchar *user_message,
                                            const gchar *system_message,
                                            const gchar *message,
                                            const gchar *expected,
                                            const gchar *actual,
                                            const gchar *backtrace,
                                            ...);

#endif
