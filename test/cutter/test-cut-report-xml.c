/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2012  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cutter.h>
#include <cutter/cut-test-runner.h>
#include <cutter/cut-listener.h>
#include <cutter/cut-report.h>
#include "../lib/cuttest-utils.h"

void test_report_success (void);
void test_report_failure (void);
void test_report_error (void);
void test_report_pending (void);
void test_report_notification (void);
void test_plural_reports (void);

#ifdef __clang__
#  define FUNCTION(name) "void " name "()"
#else
#  define FUNCTION(name) name "()"
#endif

static CutRunContext *run_context;
static CutReport *report;
static CutTest *test;
static CutTestCase *test_case;
static CutTestContext *test_context;

static void
stub_success_test (void)
{
}

static void
stub_failure_test (void)
{
    cut_fail("This test should fail");
}

static void
stub_error_test (void)
{
    cut_error("This test should error");
}

static void
stub_pending_test (void)
{
    cut_pend("This test has been pending ever!");
}

static void
stub_notification_test (void)
{
    cut_notify("This test has been notifable ever!");
}

void
cut_setup (void)
{
    const gchar *test_names[] = {"/.*/", NULL};
    test = NULL;
    test_context = NULL;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());
    cut_run_context_set_target_test_names(run_context, test_names);

    report = cut_report_new("xml", NULL);
    cut_listener_attach_to_run_context(CUT_LISTENER(report), run_context);

    test_case = cut_test_case_new("stub test case",
                                  NULL, NULL, NULL, NULL);
}

void
cut_teardown (void)
{
    if (test)
        g_object_unref(test);
    if (test_context)
        g_object_unref(test_context);
    cut_listener_detach_from_run_context(CUT_LISTENER(report), run_context);
    g_object_unref(report);
    g_object_unref(run_context);
}

static void
cb_test_signal (CutTest *test, CutTestContext *context, CutTestResult *result,
                gpointer data)
{
    g_object_set(G_OBJECT(result), "elapsed", 0.0001, NULL);
}

static gboolean
run (void)
{
    gboolean success;

    test_context = cut_test_context_new(run_context,
                                        NULL, test_case, NULL, test);
    cut_test_context_current_push(test_context);
    success = cut_test_runner_run_test(CUT_TEST_RUNNER(run_context),
                                       test, test_context);
    cut_test_context_current_pop();

    g_object_unref(test_context);
    test_context = NULL;
    return success;
}

static const gchar *
normalize_result (const gchar *result)
{
  const gchar *normalized_result;

  normalized_result = result;
  normalized_result = cut_take_replace(normalized_result,
                                       ISO8601_PATTERN,
                                       "1970-01-01T00:00:00Z");
  normalized_result = cut_take_replace(normalized_result,
                                       "<elapsed>.+?</elapsed>",
                                       "<elapsed>0.0</elapsed>");

  return normalized_result;
}

void
test_report_success (void)
{
    gchar expected[] =
        "  <result>\n"
        "    <test-case>\n"
        "      <name>stub test case</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test-case>\n"
        "    <test>\n"
        "      <name>stub-success-test</name>\n"
        "      <description>A success test</description>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
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
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>0.0</elapsed>\n"
        "  </result>\n";
    gchar *result;
    const gchar *normalized_result;

    test = cut_test_new("stub-success-test", stub_success_test);
    cut_test_set_attribute(test, "description", "A success test");
    cut_test_set_attribute(test, "bug", "1234");
    cut_test_set_attribute(test, "price", "$199");
    g_signal_connect_after(test, "success",
                           G_CALLBACK(cb_test_signal), NULL);
    cut_test_case_add_test(test_case, test);
    cut_assert_true(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_test_signal),
                                         NULL);

    result = cut_report_get_success_results(report);
    normalized_result = normalize_result(result);
    g_free(result);
    cut_assert_equal_string(expected, normalized_result);
}

void
test_report_failure (void)
{
    gchar expected[] =
        "  <result>\n"
        "    <test-case>\n"
        "      <name>stub test case</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test-case>\n"
        "    <test>\n"
        "      <name>stub-failure-test</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test>\n"
        "    <status>failure</status>\n"
        "    <detail>This test should fail</detail>\n"
        "    <backtrace>\n"
        "      <entry>\n"
        "        <file>test-cut-report-xml.c</file>\n"
        "        <line>28</line>\n"
        "        <info>" FUNCTION("stub_failure_test") ": "
                      "cut_fail(&quot;This test should fail&quot;)</info>\n"
        "      </entry>\n"
        "    </backtrace>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>0.0</elapsed>\n"
        "  </result>\n";
    gchar *result;
    const gchar *normalized_result;

    test = cut_test_new("stub-failure-test", stub_failure_test);
    g_signal_connect_after(test, "failure",
                           G_CALLBACK(cb_test_signal), NULL);
    cut_test_case_add_test(test_case, test);
    cut_assert_false(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_test_signal),
                                         NULL);

    result = cut_report_get_failure_results(report);
    normalized_result = normalize_result(result);
    g_free(result);
    cut_assert_equal_string(expected, normalized_result);
}

void
test_report_pending (void)
{
    gchar expected[] =
        "  <result>\n"
        "    <test-case>\n"
        "      <name>stub test case</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test-case>\n"
        "    <test>\n"
        "      <name>stub-pending-test</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test>\n"
        "    <status>pending</status>\n"
        "    <detail>This test has been pending ever!</detail>\n"
        "    <backtrace>\n"
        "      <entry>\n"
        "        <file>test-cut-report-xml.c</file>\n"
        "        <line>40</line>\n"
        "        <info>" FUNCTION("stub_pending_test") ": "
                  "cut_pend(&quot;This test has been pending ever!&quot;)"
                "</info>\n"
        "      </entry>\n"
        "    </backtrace>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>0.0</elapsed>\n"
        "  </result>\n";
    gchar *result;
    const gchar *normalized_result;

    test = cut_test_new("stub-pending-test", stub_pending_test);
    g_signal_connect_after(test, "pending",
                           G_CALLBACK(cb_test_signal), NULL);
    cut_test_case_add_test(test_case, test);
    cut_assert_false(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_test_signal),
                                         NULL);

    result = cut_report_get_pending_results(report);
    normalized_result = normalize_result(result);
    g_free(result);
    cut_assert_equal_string(expected, normalized_result);
}

void
test_report_notification (void)
{
    gchar expected[] =
        "  <result>\n"
        "    <test-case>\n"
        "      <name>stub test case</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test-case>\n"
        "    <test>\n"
        "      <name>stub-notification-test</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test>\n"
        "    <status>notification</status>\n"
        "    <detail>This test has been notifable ever!</detail>\n"
        "    <backtrace>\n"
        "      <entry>\n"
        "        <file>test-cut-report-xml.c</file>\n"
        "        <line>46</line>\n"
        "        <info>" FUNCTION("stub_notification_test") ": "
                  "cut_notify(&quot;This test has been "
                                   "notifable ever!&quot;)"
                "</info>\n"
        "      </entry>\n"
        "    </backtrace>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>0.0</elapsed>\n"
        "  </result>\n";
    gchar *result;
    const gchar *normalized_result;

    test = cut_test_new("stub-notification-test", stub_notification_test);
    g_signal_connect_after(test, "notification",
                           G_CALLBACK(cb_test_signal), NULL);
    cut_test_case_add_test(test_case, test);
    cut_assert_true(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_test_signal),
                                         NULL);

    result = cut_report_get_notification_results(report);
    normalized_result = normalize_result(result);
    g_free(result);
    cut_assert_equal_string(expected, normalized_result);
}

void
test_report_error (void)
{
    gchar expected[] =
        "  <result>\n"
        "    <test-case>\n"
        "      <name>stub test case</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test-case>\n"
        "    <test>\n"
        "      <name>stub-error-test</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test>\n"
        "    <status>error</status>\n"
        "    <detail>This test should error</detail>\n"
        "    <backtrace>\n"
        "      <entry>\n"
        "        <file>test-cut-report-xml.c</file>\n"
        "        <line>34</line>\n"
        "        <info>" FUNCTION("stub_error_test") ": "
                      "cut_error(&quot;This test should error&quot;)"
                "</info>\n"
        "      </entry>\n"
        "    </backtrace>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>0.0</elapsed>\n"
        "  </result>\n";
    gchar *result;
    const gchar *normalized_result;

    test = cut_test_new("stub-error-test", stub_error_test);
    g_signal_connect_after(test, "error",
                           G_CALLBACK(cb_test_signal), NULL);
    cut_test_case_add_test(test_case, test);
    cut_assert_false(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_test_signal),
                                         NULL);

    result = cut_report_get_error_results(report);
    normalized_result = normalize_result(result);
    g_free(result);
    cut_assert_equal_string(expected, normalized_result);
}

void
test_plural_reports (void)
{
    gchar expected[] =
        "  <result>\n"
        "    <test-case>\n"
        "      <name>stub test case</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test-case>\n"
        "    <test>\n"
        "      <name>stub-success-test</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test>\n"
        "    <status>success</status>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>0.0</elapsed>\n"
        "  </result>\n"
        "  <result>\n"
        "    <test-case>\n"
        "      <name>stub test case</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test-case>\n"
        "    <test>\n"
        "      <name>stub-error-test</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>0.0</elapsed>\n"
        "    </test>\n"
        "    <status>error</status>\n"
        "    <detail>This test should error</detail>\n"
        "    <backtrace>\n"
        "      <entry>\n"
        "        <file>test-cut-report-xml.c</file>\n"
        "        <line>34</line>\n"
        "        <info>" FUNCTION("stub_error_test") ": "
                  "cut_error(&quot;This test should error&quot;)"
                "</info>\n"
        "      </entry>\n"
        "    </backtrace>\n"
        "    <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "    <elapsed>0.0</elapsed>\n"
        "  </result>\n";
    gchar *result;
    const gchar *normalized_result;

    test = cut_test_new("stub-success-test", stub_success_test);
    g_signal_connect_after(test, "success",
                           G_CALLBACK(cb_test_signal), NULL);
    cut_test_case_add_test(test_case, test);

    test = cut_test_new("stub-error-test", stub_error_test);
    g_signal_connect_after(test, "error",
                           G_CALLBACK(cb_test_signal), NULL);
    cut_test_case_add_test(test_case, test);
    cut_assert_false(cut_test_runner_run_test_case(CUT_TEST_RUNNER(run_context),
                                                   test_case));

    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_test_signal),
                                         NULL);

    result = cut_report_get_all_results(report);
    normalized_result = normalize_result(result);
    g_free(result);
    cut_assert_equal_string(expected, normalized_result);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
