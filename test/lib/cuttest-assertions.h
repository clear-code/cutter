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
                                            backtrace, ...)             \
    cut_trace_with_info_expression(                                     \
        cut_assert_test_result_helper(run_context, i, status,           \
                                      test_name, user_message,          \
                                      system_message, message,          \
                                      backtrace, ## __VA_ARGS__, NULL), \
        cut_assert_test_result_with_message(run_context, i, status,     \
                                            test_name, user_message,    \
                                            system_message, message,    \
                                            backtrace, ## __VA_ARGS__))

#define cut_assert_test_result(run_context, i, status, test_name,       \
                               user_message, system_message,            \
                               backtrace, ...)                          \
    cut_trace_with_info_expression(                                     \
        cut_assert_test_result_helper(run_context, i, status,           \
                                      test_name, user_message,          \
                                      system_message, NULL,             \
                                      backtrace, ## __VA_ARGS__, NULL), \
        cut_assert_test_result(run_context, i, status, test_name,       \
                               user_message, system_message, backtrace, \
                               ## __VA_ARGS__))


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
                                            const gchar *backtrace,
                                            ...);

#endif
