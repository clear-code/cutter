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
                                       n_omissions) do                  \
{                                                                       \
    CutRunContext *_run_context;                                        \
    GList *_result_summary;                                             \
    const GList *_expected_result_summary;                              \
    const GList *_actual_result_summary;                                \
                                                                        \
    _run_context = (run_context);                                       \
    _result_summary =                                                   \
        cuttest_result_summary_list_new((n_tests),                      \
                                        (n_assertions),                 \
                                        (n_successes),                  \
                                        (n_failures),                   \
                                        (n_errors),                     \
                                        (n_pendings),                   \
                                        (n_notifications),              \
                                        (n_omissions));                 \
    _expected_result_summary =                                          \
        cut_take_result_summary_list(_result_summary);                  \
                                                                        \
    _result_summary =                                                   \
        cuttest_result_summary_list_new_from_run_context(_run_context); \
                                                                        \
    _actual_result_summary =                                            \
        cut_take_result_summary_list(_result_summary);                  \
                                                                        \
    gcut_assert_equal_list_uint(_expected_result_summary,               \
                                _actual_result_summary);                \
} while (0)

#define cut_assert_test_result(run_context, i, status, test_name,       \
                               user_message, system_message,            \
                               function_name) do                        \
{                                                                       \
    const GList *_results;                                              \
    CutTestResult *_result;                                             \
    guint _i;                                                           \
    GList *_strings = NULL;                                             \
    const GList *_expected_strings, *_actual_strings;                   \
                                                                        \
    _results = cut_run_context_get_results(run_context);                \
    _i = (i);                                                           \
    cut_assert_operator_int(_i, <, g_list_length((GList *)_results));   \
                                                                        \
    _result = g_list_nth_data((GList *)_results, _i);                   \
    cut_assert(_result);                                                \
    cut_assert_equal_int((status),                                      \
                         cut_test_result_get_status(_result));          \
                                                                        \
    _strings = cuttest_result_string_list_new((test_name),              \
                                              (user_message),           \
                                              (system_message),         \
                                              (function_name));         \
    _expected_strings = cut_take_result_string_list(_strings);          \
                                                                        \
    _strings = cuttest_result_string_list_new_from_result(_result);     \
    _actual_strings = cut_take_result_string_list(_strings);            \
                                                                        \
    gcut_assert_equal_list_string(_expected_strings, _actual_strings);  \
} while (0)

#endif
