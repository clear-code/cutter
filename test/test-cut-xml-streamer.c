#include "cutter.h"
#include "cut-runner.h"
#include "cut-listener.h"
#include "cut-streamer.h"

#include <unistd.h>
#include <stdlib.h>

void test_streamer_success (void);

static CutStreamer *streamer;
static CutRunContext *run_context;
static CutTest *test_object;
static CutTestCase *test_case;
static CutTestContext *test_context;

static void
dummy_success_test (void)
{
}

void
setup (void)
{
    gchar *test_names[] = {"/.*/", NULL};
    test_object = NULL;
    test_context = NULL;
    streamer = NULL;

    run_context = CUT_RUN_CONTEXT(cut_runner_new());
    cut_run_context_set_target_test_names(run_context, test_names);

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
    if (streamer)
        g_object_unref(streamer);
    g_object_unref(run_context);
}

static void
cb_test_signal (CutTest *test, CutTestContext *context, CutTestResult *result,
                gpointer data)
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
    success = cut_test_run(test, test_context, run_context);
    set_current_test_context(original_test_context);

    g_object_unref(test_context);
    test_context = NULL;
    return success;
}

void
test_streamer_success (void)
{
    gchar expected[] = "  <result>\n"
                       "    <test-case>\n"
                       "      <name>dummy test case</name>\n"
                       "    </test-case>\n"
                       "    <test>\n"
                       "      <name>dummy-success-test</name>\n"
                       "    </test>\n"
                       "    <status>success</status>\n"
                       "    <elapsed>.*?</elapsed>\n"
                       "  </result>\n"
                       "\n";
    int pid;

    pid = cut_fork();
    cut_assert_errno();

    if (pid == 0) {
        streamer = cut_streamer_new("xml", NULL);

        test_object = cut_test_new("dummy-success-test", dummy_success_test);
        g_signal_connect_after(test_object, "success",
                               G_CALLBACK(cb_test_signal), NULL);
        cut_test_case_add_test(test_case, test_object);
        cut_listener_attach_to_run_context(CUT_LISTENER(streamer), run_context);
        cut_assert(run_the_test(test_object));
        g_signal_handlers_disconnect_by_func(test_object,
                                             G_CALLBACK(cb_test_signal),
                                             NULL);
        cut_listener_detach_from_run_context(CUT_LISTENER(streamer),
                                             run_context);
        _exit(EXIT_SUCCESS);
    }

    cut_assert_match(expected, cut_fork_get_stdout_message(pid));
    cut_assert_equal_int(EXIT_SUCCESS, cut_wait_process(pid, 0));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
