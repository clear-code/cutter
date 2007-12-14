#include <gcutter.h>
#include "cut-test.h"
#include "cut-test-result.h"
#include "cut-utils.h"
#include "cut-runner.h"

void test_equal_g_type(void);

static CutTest *test;
static CutRunner *runner;
static CutTestContext *test_context;
static CutTestResult *test_result;

static gboolean
run (CutTest *_test)
{
    gboolean success;
    CutTestContext *original_test_context;

    test = _test;

    runner = cut_runner_new();

    test_context = cut_test_context_new(NULL, NULL, test);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, runner);
    set_current_test_context(original_test_context);

    return success;
}

void
setup (void)
{
    test = NULL;
    runner = NULL;
    test_context = NULL;
    test_result = NULL;
}

void
teardown (void)
{
    if (test)
        g_object_unref(test);
    if (runner)
        g_object_unref(runner);
    if (test_context)
        g_object_unref(test_context);
    if (test_result)
        g_object_unref(test_result);
}

static void
cut_assert_test_result_summary (gint n_tests, gint n_assertions,
                                gint n_failures, gint n_errors,
                                gint n_pendings, gint n_notifications)
{
    cut_assert_equal_int(n_tests, cut_runner_get_n_tests(runner));
    cut_assert_equal_int(n_assertions, cut_runner_get_n_assertions(runner));
    cut_assert_equal_int(n_failures, cut_runner_get_n_failures(runner));
    cut_assert_equal_int(n_errors, cut_runner_get_n_errors(runner));
    cut_assert_equal_int(n_pendings, cut_runner_get_n_pendings(runner));
    cut_assert_equal_int(n_notifications,
                         cut_runner_get_n_notifications(runner));
}

static void
cut_assert_test_result (gint i, CutTestResultStatus status,
                        const gchar *test_name,
                        const gchar *user_message, const gchar *system_message,
                        const gchar *function_name)
{
    const GList *results;
    CutTestResult *result;

    results = cut_runner_get_results(runner);
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

static void
equal_g_type_test (void)
{
    cut_assert_equal_g_type(G_TYPE_INT, G_TYPE_INT);
    cut_assert_equal_g_type(G_TYPE_INT, G_TYPE_STRING);
}

void
test_equal_g_type (void)
{
    CutTest *test;

    test = cut_test_new("equal_g_type test", NULL, equal_g_type_test);
    cut_assert(test);

    cut_assert(!run(test));
    cut_assert_test_result_summary(1, 1, 1, 0, 0, 0);
    cut_assert_test_result(0, CUT_TEST_RESULT_FAILURE,
                           "equal_g_type test",
                           NULL,
                           "<G_TYPE_INT == G_TYPE_STRING>\n"
                           "expected: <gint>\n"
                           " but was: <gchararray>",
                           "equal_g_type_test");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

