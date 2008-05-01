#include <cutter.h>
#include <cutter/cut-runner.h>

#include "cuttest-utils.h"

void test_sort_test_cases(void);
void test_n_ (void);
void test_get_test_directory(void);
void test_get_source_directory(void);
void test_copy (void);
void test_order (void);
void test_ready_signal (void);

static CutRunner *runner;
static CutTestCase *test_case;
static GList *test_cases;
static gchar **expected_names, **actual_names;

static gint n_ready_test_suite_signals = 0;
static gint n_ready_test_case_signals = 0;

void
setup (void)
{
    runner = cut_runner_new();
    test_case = NULL;
    test_cases = NULL;
    expected_names = NULL;
    actual_names = NULL;

    n_ready_test_suite_signals = 0;
    n_ready_test_case_signals = 0;
}

void
teardown (void)
{
    g_object_unref(runner);
    if (test_case)
        g_object_unref(test_case);

    g_list_foreach(test_cases, (GFunc)g_object_unref, NULL);
    g_list_free(test_cases);

    g_strfreev(expected_names);
    g_strfreev(actual_names);
}

static void
dummy_success_function (void)
{
    cut_assert(1);
}

static void
dummy_failure_function (void)
{
    cut_fail("This test should fail");
}

static void
dummy_omit_function (void)
{
    cut_omit("This test should be omitted.");
}

static void
dummy_pending_function (void)
{
    cut_pend("This test has been pending ever!");
}

static void
dummy_notification_function (void)
{
    cut_notify("This test has been notifying ever!");
}

static void
dummy_error_function (void)
{
    cut_error("This test should error");
}

static gchar **
collect_test_case_names (GList *test_cases)
{
    gint i;
    GList *node;
    gchar **names;

    names = g_new0(gchar *, g_list_length(test_cases) + 1);
    for (i = 0, node = test_cases;
         node;
         i++, node = g_list_next(node)) {
        CutTestCase *test_case;

        test_case = CUT_TEST_CASE(node->data);
        names[i] = g_strdup(cut_test_get_name(CUT_TEST(test_case)));
    }
    names[i] = NULL;

    return names;
}

void
test_order (void)
{
    cut_runner_set_test_case_order(runner, CUT_ORDER_NAME_ASCENDING);
    cut_assert_equal_int(CUT_ORDER_NAME_ASCENDING,
                         cut_runner_get_test_case_order(runner));
    cut_runner_set_test_case_order(runner, CUT_ORDER_NAME_DESCENDING);
    cut_assert_equal_int(CUT_ORDER_NAME_DESCENDING,
                         cut_runner_get_test_case_order(runner));
}

void
test_sort_test_cases (void)
{
    test_cases = g_list_append(test_cases,
                               cut_test_case_new("abc", NULL, NULL,
                                                 NULL, NULL, NULL, NULL));
    test_cases = g_list_append(test_cases,
                               cut_test_case_new("xyz", NULL, NULL,
                                                 NULL, NULL, NULL, NULL));
    test_cases = g_list_append(test_cases,
                               cut_test_case_new("123", NULL, NULL,
                                                 NULL, NULL, NULL, NULL));
    test_cases = g_list_append(test_cases,
                               cut_test_case_new("XYZ", NULL, NULL,
                                                 NULL, NULL, NULL, NULL));

    expected_names = g_strsplit("abc xyz 123 XYZ", " ", -1);
    actual_names = collect_test_case_names(test_cases);
    cut_assert_equal_string_array(expected_names, actual_names);
    g_strfreev(actual_names);

    test_cases = cut_runner_sort_test_cases(runner, test_cases);
    actual_names = collect_test_case_names(test_cases);
    cut_assert_equal_string_array(expected_names, actual_names);
    g_strfreev(expected_names);
    g_strfreev(actual_names);

    cut_runner_set_test_case_order(runner, CUT_ORDER_NAME_ASCENDING);
    test_cases = cut_runner_sort_test_cases(runner, test_cases);
    expected_names = g_strsplit("123 XYZ abc xyz", " ", -1);
    actual_names = collect_test_case_names(test_cases);
    cut_assert_equal_string_array(expected_names, actual_names);
    g_strfreev(expected_names);
    g_strfreev(actual_names);

    cut_runner_set_test_case_order(runner, CUT_ORDER_NAME_DESCENDING);
    test_cases = cut_runner_sort_test_cases(runner, test_cases);
    expected_names = g_strsplit("xyz abc XYZ 123", " ", -1);
    actual_names = collect_test_case_names(test_cases);
    cut_assert_equal_string_array(expected_names, actual_names);
}

void
test_n_ (void)
{
    test_case = cut_test_case_new("dummy test case",
                                  NULL,
                                  NULL,
                                  get_current_test_context,
                                  set_current_test_context,
                                  NULL, NULL);
    cuttest_add_test(test_case, "test_1", dummy_success_function);
    cuttest_add_test(test_case, "test_2", dummy_failure_function);
    cuttest_add_test(test_case, "test_3", dummy_omit_function);
    cuttest_add_test(test_case, "test_4", dummy_pending_function);
    cuttest_add_test(test_case, "test_5", dummy_notification_function);
    cuttest_add_test(test_case, "test_6", dummy_error_function);

    cut_assert(!cut_test_case_run(test_case, runner));

    cut_assert_equal_int(3, cut_runner_get_n_successes(runner));
    cut_assert_equal_int(1, cut_runner_get_n_failures(runner));
    cut_assert_equal_int(1, cut_runner_get_n_errors(runner));
    cut_assert_equal_int(1, cut_runner_get_n_pendings(runner));
    cut_assert_equal_int(1, cut_runner_get_n_notifications(runner));
    cut_assert_equal_int(1, cut_runner_get_n_omissions(runner));
    cut_assert_equal_int(6, cut_runner_get_n_tests(runner));
    cut_assert_equal_int(1, cut_runner_get_n_assertions(runner));

    g_object_unref(test_case);
    test_case = NULL;
}
 
void
test_get_test_directory (void)
{
    cut_runner_set_test_directory(runner, ".");
    cut_assert_equal_string(".", cut_runner_get_test_directory(runner));
}

void
test_get_source_directory (void)
{
    cut_runner_set_source_directory(runner, ".");
    cut_assert_equal_string(".", cut_runner_get_source_directory(runner));
}

void
test_copy (void)
{
    CutRunner *new_runner;

    new_runner = cut_runner_copy(runner);
    cut_assert(new_runner);

    cut_assert_equal_int(cut_runner_get_multi_thread(runner),
                         cut_runner_get_multi_thread(new_runner));
    cut_assert_equal_int(cut_runner_is_multi_thread(runner),
                         cut_runner_is_multi_thread(new_runner));

    cut_assert_equal_string(cut_runner_get_test_directory(runner),
                            cut_runner_get_test_directory(new_runner));
    cut_assert_equal_string(cut_runner_get_source_directory(runner),
                            cut_runner_get_source_directory(new_runner));

    g_object_unref(new_runner);
}

static void
cb_ready_test_suite_signal (CutRunner *runner,
                            CutTestSuite *suite,
                            guint n_test_cases,
                            guint n_tests,
                            gpointer data)
{
    n_ready_test_suite_signals++;
}

static void
cb_ready_test_case_signal (CutRunner *runner,
                           CutTestCase *test_case,
                           guint n_tests,
                           gpointer data)
{
    n_ready_test_case_signals++;
}

void
test_ready_signal (void)
{
    CutTestSuite *suite;
    CutTestCase *test_case;

    suite = cut_test_suite_new();

    test_case = cut_test_case_new("dummy test case",
                                  NULL,
                                  NULL,
                                  get_current_test_context,
                                  set_current_test_context,
                                  NULL, NULL);
    cuttest_add_test(test_case, "test_1", dummy_success_function);
    cut_test_suite_add_test_case(suite, test_case);
    g_object_unref(test_case);
    test_case = NULL;

    cut_runner_set_test_suite(runner, suite);
    g_signal_connect(runner, "ready-test-suite", G_CALLBACK(cb_ready_test_suite_signal), NULL);
    g_signal_connect(runner, "ready-test-case", G_CALLBACK(cb_ready_test_case_signal), NULL);
    cut_assert(cut_test_suite_run(suite, runner));
    g_signal_handlers_disconnect_by_func(runner,
                                         G_CALLBACK(cb_ready_test_suite_signal),
                                         NULL);
    g_signal_handlers_disconnect_by_func(runner,
                                         G_CALLBACK(cb_ready_test_case_signal),
                                         NULL);

    cut_assert_equal_int(1, n_ready_test_suite_signals);
    g_object_unref(suite);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
