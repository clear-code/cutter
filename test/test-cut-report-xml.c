#include "cutter.h"
#include "cut-runner.h"
#include "cut-report.h"

void test_report_success (void);
void test_report_failure (void);

static CutRunner *runner;
static CutReport *report;
static CutTest *test_object;
static CutTestCase *test_case;
static CutTestContext *test_context;

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
    const gchar *test_names[] = {"/.*/", NULL};
    test_object = NULL;
    test_context = NULL;

    runner = cut_runner_new();
    cut_runner_set_target_test_names(runner, test_names);

    report = cut_report_new("xml", NULL);
    cut_listener_attach_to_runner(CUT_LISTENER(report), runner);

    test_case = cut_test_case_new("dummy test case",
                                  NULL, NULL, 
                                  get_current_test_context,
                                  set_current_test_context,
                                  NULL, NULL);
}

void
teardown (void)
{
    if (test_object)
        g_object_unref(test_object);
    if (test_context)
        g_object_unref(test_context);
    g_object_unref(report);
    g_object_unref(runner);
}

static void
cb_success_signal (CutTest *test, CutTestContext *context, CutTestResult *result, gpointer data)
{
    g_object_set(G_OBJECT(result), "elapsed", 0.0001, NULL);
}

static void
cb_failure_signal (CutTest *test, CutTestContext *context, CutTestResult *result, gpointer data)
{
    g_object_set(G_OBJECT(result), "elapsed", 0.0001, NULL);
}

static gboolean
run_the_test (CutTest *test)
{
    gboolean success;
    CutTestContext *original_test_context;

    test_context = cut_test_context_new(NULL, test_case, test);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, runner);
    set_current_test_context(original_test_context);

    g_object_unref(test_context);
    test_context = NULL;
    return success;
}

void
test_report_success (void)
{
    gchar expected[] = "  <result>\n"
                       "    <test-case>\n"
                       "      <name>dummy test case</name>\n"
                       "    </test-case>\n"
                       "    <test>\n"
                       "      <name>dummy-success-test</name>\n"
                       "      <description>A success test</description>\n"
                       "      <option>\n"
                       "        <name>price</name>\n"
                       "        <value>$199</value>\n"
                       "      </option>\n"
                       "      <option>\n"
                       "        <name>bug</name>\n"
                       "        <value>1234</value>\n"
                       "      </option>\n"
                       "    </test>\n"
                       "    <status>success</status>\n"
                       "    <elapsed>0.000100</elapsed>\n"
                       "  </result>\n";
    test_object = cut_test_new("dummy-success-test", dummy_success_test);
    cut_test_set_attribute(test_object, "description", "A success test");
    cut_test_set_attribute(test_object, "bug", "1234");
    cut_test_set_attribute(test_object, "price", "$199");
    g_signal_connect_after(test_object, "success", G_CALLBACK(cb_success_signal), NULL);
    cut_test_case_add_test(test_case, test_object);
    cut_assert(run_the_test(test_object));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_success_signal),
                                         NULL);

    cut_assert_equal_string_with_free(expected, cut_report_get_success_results(report));
}

void
test_report_failure (void)
{
    gchar expected[] = "  <result>\n"
                       "    <test-case>\n"
                       "      <name>dummy test case</name>\n"
                       "    </test-case>\n"
                       "    <test>\n"
                       "      <name>dummy-failure-test</name>\n"
                       "    </test>\n"
                       "    <status>failure</status>\n"
                       "    <detail>This test should fail</detail>\n"
                       "    <backtrace>\n"
                       "      <file>test-cut-report-xml.c</file>\n"
                       "      <line>22</line>\n"
                       "      <info>dummy_failure_test()</info>\n"
                       "    </backtrace>\n"
                       "    <elapsed>0.000100</elapsed>\n"
                       "  </result>\n";

    test_object = cut_test_new("dummy-failure-test", dummy_failure_test);
    g_signal_connect_after(test_object, "failure", G_CALLBACK(cb_failure_signal), NULL);
    cut_test_case_add_test(test_case, test_object);
    cut_assert(!run_the_test(test_object));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_failure_signal),
                                         NULL);

    cut_assert_equal_string_with_free(expected, cut_report_get_failure_results(report));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
