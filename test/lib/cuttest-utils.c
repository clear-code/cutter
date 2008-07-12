#include "cuttest-utils.h"

const gchar *
cuttest_get_base_dir(void)
{
    const gchar *dir;

    dir = g_getenv("BASE_DIR");
    return dir ? dir : ".";
}

void
cuttest_add_test (CutTestCase *test_case, const gchar *test_name,
                  CutTestFunction test_function)
{
    CutTest *test;

    test = cut_test_new(test_name, test_function);
    cut_test_case_add_test(test_case, test);
    g_object_unref(test);
}

GList *
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
    APPEND(n_failures);
    APPEND(n_errors);
    APPEND(n_pendings);
    APPEND(n_notifications);
    APPEND(n_omissions);

#undef APPEND

    return list;
}

GList *
cuttest_result_summary_list_new_from_run_context (CutRunContext *run_context)
{
    return cuttest_result_summary_list_new(
        cut_run_context_get_n_tests(run_context),
        cut_run_context_get_n_assertions(run_context),
        0,
        cut_run_context_get_n_failures(run_context),
        cut_run_context_get_n_errors(run_context),
        cut_run_context_get_n_pendings(run_context),
        cut_run_context_get_n_notifications(run_context),
        cut_run_context_get_n_omissions(run_context));
}

