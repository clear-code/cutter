#include "cutter.h"
#include "cut-runner.h"
#include "cut-report.h"

void test_result_success_log (void);

static CutRunner *runner;
static CutReport *report;

void
setup (void)
{
    runner = NULL;
    report = NULL;
    cut_report_load(NULL);
}

void
teardown (void)
{
    if (runner)
        g_object_unref(runner);
    if (report)
        g_object_unref(report);
    cut_report_unload();
}

void
test_result_success_log (void)
{
    gchar expected[] = "<result>"
                       "  <status>success</status>"
                       "  <detail></detail>"
                       "  <elapsed>0.0001</elapsed>"
                       "</result>";

    runner = cut_runner_new();
    cut_runner_set_test_directory(runner, "report_test_dir");
    report = cut_report_new("xml", runner, NULL);
    cut_runner_run(runner);

    cut_assert_equal_string_with_free(expected, cut_report_get_success_results(report));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
