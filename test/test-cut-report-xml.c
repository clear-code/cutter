#include "cutter.h"
#include "cut-runner.h"
#include "cut-report.h"

void test_report_success (void);
void test_report_failure (void);

static CutRunner *runner;
static CutReport *report;
static CutTest *test_object;

static void
dummy_success_test (void)
{
}

static void
dummy_failure_test (void)
{
    cut_fail("This test should fail");
}

void
setup (void)
{
    test_object = NULL;
    cut_report_load(NULL);
    runner = cut_runner_new();
    report = cut_report_new("xml", runner, NULL);
}

void
teardown (void)
{
    g_object_unref(report);
    g_object_unref(runner);
    if (test_object)
        g_object_unref(test_object);
    cut_report_unload();
}

static gboolean
run (CutTest *test)
{
    gboolean success;
    CutTestContext *original_test_context;
    CutTestContext *test_context;

    test_context = cut_test_context_new(NULL, NULL, test);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, runner);
    set_current_test_context(original_test_context);

    g_object_unref(test_context);

    return success;
}

void
test_report_success (void)
{
    gchar expected[] = "  <test_log>\n"
                       "    <test_case>\n"
                       "      <name>dummy_report_test</name>\n"
                       "    </test_case>\n"
                       "    <name>dummy-success-test</name>\n"
                       "    <result>\n"
                       "      <status>success</status>\n"
                       "      <detail></detail>\n"
                       "      <elapsed>0.0001</elapsed>\n"
                       "    </result>\n"
                       "  </test_log>\n";
    CutTest *test;

    test = cut_test_new("dummy-success-test", NULL, dummy_success_test);
    cut_assert(run(test));

    cut_assert_equal_string_with_free(expected, cut_report_get_success_results(report));
}

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
    cut_assert(!run(test));

    cut_assert_equal_string_with_free(expected, cut_report_get_failure_results(report));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
