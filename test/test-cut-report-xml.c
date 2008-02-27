#include "cutter.h"
#include "cut-runner.h"
#include "cut-report.h"

void test_report_success (void);
#if 0
void test_report_failure (void);
#endif

static CutRunner *runner;
static CutReport *report;
static CutTest *test_object;
static CutTestCase *test_case;

static void
dummy_success_test (void)
{
}

#if 0
static void
dummy_failure_test (void)
{
    cut_fail("This test should fail");
}
#endif

void
setup (void)
{
    const gchar *test_names[] = {"/.*/", NULL};
    test_object = NULL;
    cut_report_load(NULL);
    runner = cut_runner_new();
    cut_runner_set_target_test_names(runner, test_names);
    test_case = cut_test_case_new("dummy test case",
                                  NULL, NULL, 
                                  get_current_test_context,
                                  set_current_test_context);
    report = cut_report_new("xml", runner, NULL);
}

void
teardown (void)
{
    if (test_object)
        g_object_unref(test_object);
    g_object_unref(test_case);
    g_object_unref(report);
    g_object_unref(runner);
    cut_report_unload();
}

static void
cb_success_signal (CutTest *test, CutTestResult *result, gpointer data)
{
    g_object_set(G_OBJECT(result), "elapsed", 0.0001, NULL);
}

static gboolean
run_the_test (void)
{
    return cut_test_case_run(test_case, runner);
}

void
test_report_success (void)
{
    gchar expected[] = "  <test_log>\n"
                       "    <test_case>\n"
                       "      <name>dummy test case</name>\n"
                       "    </test_case>\n"
                       "    <name>dummy-success-test</name>\n"
                       "    <result>\n"
                       "      <status>success</status>\n"
                       "      <detail></detail>\n"
                       "      <elapsed>0.0001</elapsed>\n"
                       "    </result>\n"
                       "  </test_log>\n";
    test_object = cut_test_new("dummy-success-test", NULL, dummy_success_test);
    g_signal_connect_after(test_object, "success", G_CALLBACK(cb_success_signal), NULL);
    cut_test_case_add_test(test_case, test_object);
    cut_assert(run_the_test());
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_success_signal),
                                         NULL);

    cut_assert_equal_string_with_free(expected, cut_report_get_success_results(report));
}

#if 0
void
test_report_failure (void)
{
    gchar expected[] = "  <test_log>\n"
                       "    <test_case>\n"
                       "      <name>dummy_report_test</name>\n"
                       "    </test_case>\n"
                       "    <name>dummy-failure-test</name>\n"
                       "    <result>\n"
                       "      <status>failure</status>\n"
                       "      <detail></detail>\n"
                       "      <elapsed>0.0001</elapsed>\n"
                       "    </result>\n"
                       "  </test_log>\n";
    CutTest *test;

    test = cut_test_new("dummy-failure-test", NULL, dummy_failure_test);
    cut_test_case_add_test(test_case, test);
    cut_assert(run_the_test());

    cut_assert_equal_string_with_free(expected, cut_report_get_failure_results(report));
}
#endif

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
