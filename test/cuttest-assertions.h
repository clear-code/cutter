#ifndef __CUTTEST__UTILS_H__
#define __CUTTEST__UTILS_H__

#include <cutter/cut-assertions.h>
#include <cutter/cut-run-context.h>

#define cut_assert_test_result_summary(run_context, n_tests, n_assertions, \
                                       n_failures, n_errors, n_pendings, \
                                       n_notifications, n_omissions) do \
{                                                                       \
    CutRunContext *_run_context;                                        \
    guint _n_tests, _n_assertions, _n_failures, _n_errors, _n_pendings; \
    guint _n_notifications, _n_omissions;                               \
                                                                        \
    _run_context = (run_context);                                       \
    _n_tests = (n_tests);                                               \
    _n_assertions = (n_assertions);                                     \
    _n_failures = (n_failures);                                         \
    _n_errors = (n_errors);                                             \
    _n_pendings = (n_pendings);                                         \
    _n_notifications = (n_notifications);                               \
    _n_omissions = (n_omissions);                                       \
                                                                        \
    cut_assert_equal_uint(_n_tests,                                     \
                          cut_run_context_get_n_tests(_run_context));   \
    cut_assert_equal_uint(_n_assertions,                                \
                          cut_run_context_get_n_assertions(_run_context)); \
    cut_assert_equal_uint(_n_failures,                                  \
                          cut_run_context_get_n_failures(_run_context)); \
    cut_assert_equal_uint(_n_errors,                                    \
                          cut_run_context_get_n_errors(_run_context));  \
    cut_assert_equal_uint(_n_pendings,                                  \
                          cut_run_context_get_n_pendings(_run_context)); \
    cut_assert_equal_uint(_n_notifications,                             \
                          cut_run_context_get_n_notifications(_run_context)); \
    cut_assert_equal_uint(_n_omissions,                                 \
                          cut_run_context_get_n_omissions(_run_context)); \
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
    cut_assert_equal_string_or_null(user_message,
                                    cut_test_result_get_user_message(result));
    cut_assert_equal_string_or_null(system_message,
                                    cut_test_result_get_system_message(result));
    cut_assert_equal_string(function_name,
                            cut_test_result_get_function_name(result));
}

#endif
