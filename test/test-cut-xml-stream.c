#include "cutter.h"
#include "cut-test-runner.h"
#include "cut-listener.h"
#include "cut-streamer.h"

#include <unistd.h>
#include <stdlib.h>

void test_ready_test_suite (void);
void test_ready_test_case (void);
void test_streamer_success (void);

static CutStreamer *streamer;
static CutRunContext *run_context;
static CutTest *test;
static CutTestCase *test_case;
static CutTestSuite *test_suite;
static CutTestContext *test_context;

static GString *xml;

static void
stub_success_test (void)
{
}

void
setup (void)
{
    gchar *test_names[] = {"/.*/", NULL};

    test = NULL;
    test_context = NULL;
    streamer = NULL;
    xml = NULL;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());
    cut_run_context_set_target_test_names(run_context, test_names);

    test_case = cut_test_case_new("stub test case",
                                  NULL, NULL,
                                  get_current_test_context,
                                  set_current_test_context,
                                  NULL, NULL);
    test_suite = cut_test_suite_new_empty();
    cut_test_suite_add_test_case(test_suite, test_case);
}

void
teardown (void)
{
    if (test)
        g_object_unref(test);
    if (test_context)
        g_object_unref(test_context);
    if (streamer)
        g_object_unref(streamer);
    if (test_case)
        g_object_unref(test_case);
    if (test_suite)
        g_object_unref(test_suite);
    g_object_unref(run_context);
    if (xml)
        g_string_free(xml, TRUE);
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
    CutTestContext *original_test_context;

    test_context = cut_test_context_new(NULL, test_case, NULL, test);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, run_context);
    set_current_test_context(original_test_context);

    g_object_unref(test_context);
    test_context = NULL;
    return success;
}

static gboolean
stream_to_string (const gchar *message, GError **error, gpointer user_data)
{
    GString *string = user_data;

    g_string_append(string, message);

    return TRUE;
}

void
test_ready_test_suite (void)
{
    gchar expected[] =
        "  <ready-test-suite>\n"
        "    <test-suite>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>.*?</elapsed>\n"
        "    </test-suite>\n"
        "    <n-test-cases>1</n-test-cases>\n"
        "    <n-tests>1</n-tests>\n"
        "  </ready-test-suite>\n";

    xml = g_string_new(NULL);

    streamer = cut_streamer_new("xml",
                                "stream-function", stream_to_string,
                                "stream-function-user-data", xml,
                                NULL);
    test = cut_test_new("stub-success-test", stub_success_test);
    cut_test_case_add_test(test_case, test);
    cut_listener_attach_to_run_context(CUT_LISTENER(streamer), run_context);
    cut_assert(cut_test_suite_run(test_suite, run_context));
    cut_listener_detach_from_run_context(CUT_LISTENER(streamer), run_context);

    cut_assert_match(expected, xml->str);
}

void
test_ready_test_case (void)
{
    gchar expected[] =
        "  <ready-test-case>\n"
        "    <test-case>\n"
        "      <name>stub test case</name>\n"
        "      <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "      <elapsed>.*?</elapsed>\n"
        "    </test-case>\n"
        "    <n-tests>1</n-tests>\n"
        "  </ready-test-case>\n";

    xml = g_string_new(NULL);

    streamer = cut_streamer_new("xml",
                                "stream-function", stream_to_string,
                                "stream-function-user-data", xml,
                                NULL);
    test = cut_test_new("stub-success-test", stub_success_test);
    cut_test_case_add_test(test_case, test);
    cut_listener_attach_to_run_context(CUT_LISTENER(streamer), run_context);
    cut_assert(cut_test_suite_run(test_suite, run_context));
    cut_listener_detach_from_run_context(CUT_LISTENER(streamer), run_context);

    cut_assert_match(expected, xml->str);
}

void
test_streamer_success (void)
{
    gchar expected[] =
        "  <test-result>\n"
        "    <test>\n"
        "      <name>stub-success-test</name>\n"
        "      <start-time>"
                 "\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}Z"
              "</start-time>\n"
        "      <elapsed>.*?</elapsed>\n"
        "    </test>\n"
        "    <test-context>\n"
        "      <test-case>\n"
        "        <name>stub test case</name>\n"
        "        <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "        <elapsed>.*?</elapsed>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>stub-success-test</name>\n"
        "        <start-time>"
                   "\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}Z"
                "</start-time>\n"
        "        <elapsed>.*?</elapsed>\n"
        "      </test>\n"
        "      <failed>FALSE</failed>\n"
        "    </test-context>\n"
        "    <result>\n"
        "      <test-case>\n"
        "        <name>stub test case</name>\n"
        "        <start-time>1970-01-01T00:00:00Z</start-time>\n"
        "        <elapsed>.*?</elapsed>\n"
        "      </test-case>\n"
        "      <test>\n"
        "        <name>stub-success-test</name>\n"
        "        <start-time>"
                   "\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}Z"
                "</start-time>\n"
        "        <elapsed>.*?</elapsed>\n"
        "      </test>\n"
        "      <status>success</status>\n"
        "      <start-time>"
                "\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}Z"
              "</start-time>\n"
        "      <elapsed>.*?</elapsed>\n"
        "    </result>\n"
        "  </test-result>\n";

    xml = g_string_new(NULL);

    streamer = cut_streamer_new("xml",
                                "stream-function", stream_to_string,
                                "stream-function-user-data", xml,
                                NULL);

    test = cut_test_new("stub-success-test", stub_success_test);
    g_signal_connect_after(test, "success",
                           G_CALLBACK(cb_test_signal), NULL);
    cut_test_case_add_test(test_case, test);
    cut_listener_attach_to_run_context(CUT_LISTENER(streamer), run_context);
    cut_assert_true(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_test_signal),
                                         NULL);
    cut_listener_detach_from_run_context(CUT_LISTENER(streamer), run_context);

    cut_assert_match(expected, xml->str);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
