#ifndef __CUTTEST_ASSERTIONS_H__
#define __CUTTEST_ASSERTIONS_H__

#include <gcutter.h>
#include <cutter/cut-assertions.h>
#include <cutter/cut-run-context.h>
#include <cutter/cut-test-result.h>
#include "cuttest-utils.h"

#define cut_take_result_summary_list(list)      \
    cut_take_g_list(list, NULL)

#define cut_assert_test_result_summary(run_context, n_tests,            \
                                       n_assertions,                    \
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
                                        0,                              \
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
    cut_assert_equal_g_list_uint(_expected_result_summary,              \
                                 _actual_result_summary);               \
} while (0)

static inline void
cut_assert_test_result (CutRunContext *run_context,
                        gint i, CutTestResultStatus status,
                        const gchar *test_name,
                        const gchar *user_message, const gchar *system_message,
                        const gchar *function_name)
{
    const GList *results;
    CutTestResult *result;

    results = cut_run_context_get_results(run_context);
    cut_assert_operator_int(i, <, g_list_length((GList *)results));

    result = g_list_nth_data((GList *)results, i);
    cut_assert(result);
    cut_assert_equal_int(status, cut_test_result_get_status(result));
    cut_assert_equal_string(test_name, cut_test_result_get_test_name(result));
    cut_assert_equal_string(user_message,
                            cut_test_result_get_user_message(result));
    cut_assert_equal_string(system_message,
                            cut_test_result_get_system_message(result));
    cut_assert_equal_string(function_name,
                            cut_test_result_get_function_name(result));
}

#endif
