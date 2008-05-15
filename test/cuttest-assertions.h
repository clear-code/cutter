#ifndef __CUTTEST__UTILS_H__
#define __CUTTEST__UTILS_H__

#include <cutter/cut-assertions.h>
#include <cutter/cut-run-context.h>

static inline void
cut_assert_test_result_summary (CutRunContext *run_context,
                                guint n_tests, guint n_assertions,
                                guint n_failures, guint n_errors,
                                guint n_pendings, guint n_notifications,
                                guint n_omissions)
{
    cut_assert_equal_uint(n_tests, cut_run_context_get_n_tests(run_context));
    cut_assert_equal_uint(n_assertions,
                          cut_run_context_get_n_assertions(run_context));
    cut_assert_equal_uint(n_failures,
                          cut_run_context_get_n_failures(run_context));
    cut_assert_equal_uint(n_errors,
                          cut_run_context_get_n_errors(run_context));
    cut_assert_equal_uint(n_pendings,
                          cut_run_context_get_n_pendings(run_context));
    cut_assert_equal_uint(n_notifications,
                          cut_run_context_get_n_notifications(run_context));
    cut_assert_equal_uint(n_omissions,
                          cut_run_context_get_n_omissions(run_context));
}

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
    cut_assert_equal_string_or_null(user_message,
                                    cut_test_result_get_user_message(result));
    cut_assert_equal_string_or_null(system_message,
                                    cut_test_result_get_system_message(result));
    cut_assert_equal_string(function_name,
                            cut_test_result_get_function_name(result));
}

#endif
