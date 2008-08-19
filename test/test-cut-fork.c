#include "cutter.h"
#include <cutter/cut-test.h>
#include <cutter/cut-test-runner.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-test-context.h>

#include <unistd.h>
#include <stdlib.h>
#include <glib.h>

void test_message_from_forked_process (void);
#ifndef G_OS_WIN32
void test_fail_in_forked_process (void);
#endif

static CutRunContext *run_context;
static CutTest *test;
static CutTestContext *test_context;

void
setup (void)
{
    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());

    test = NULL;
    test_context = NULL;
}

void
teardown (void)
{
    if (test)
        g_object_unref(test);

    if (test_context)
        g_object_unref(test_context);

    g_object_unref(run_context);
}

void
test_message_from_forked_process (void)
{
    int pid;

    pid = cut_fork();
    cut_assert_errno();

    if (pid == 0) {
        g_print("Walk in child process");
        g_printerr("An error was occured");
        _exit(EXIT_SUCCESS);
    }

    cut_assert_equal_string("Walk in child process",
                            cut_fork_get_stdout_message(pid));
    cut_assert_equal_string("An error was occured",
                            cut_fork_get_stderr_message(pid));
    cut_assert_equal_int(EXIT_SUCCESS, cut_wait_process(pid, 100));
}

#ifndef G_OS_WIN32
static gboolean
run (void)
{
    gboolean success, is_multi_thread;
    CutTestContext *original_test_context;

    original_test_context = get_current_test_context();
    test_context = cut_test_context_new(run_context, NULL, NULL, NULL, test);

    is_multi_thread = cut_test_context_is_multi_thread(original_test_context);
    cut_run_context_set_multi_thread(run_context, is_multi_thread);
    cut_test_context_set_multi_thread(test_context, is_multi_thread);

    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, run_context);
    set_current_test_context(original_test_context);

    return success;
}

static void
cut_fail_in_forked_process (void)
{
    int pid;

    pid = cut_fork();
    cut_assert_errno();

    if (pid == 0) {
        cut_notify("Notification in child process");
        cut_fail("Failure in child process");
        _exit(EXIT_SUCCESS);
    }

    cut_assert_equal_int(EXIT_SUCCESS, cut_wait_process(pid, 100));
}

static void
cb_collect_message (CutTest *test, CutTestContext *context,
                    CutTestResult *result, gpointer data)
{
    const gchar **message = data;
    *message = cut_take_string(g_strdup(cut_test_result_get_message(result)));
}

void
test_fail_in_forked_process (void)
{
    const gchar *failure_message = NULL;
    const gchar *notification_message = NULL;
    const gchar *omission_message = NULL;

    test = cut_test_new("failure", cut_fail_in_forked_process);

    g_signal_connect(test, "notification",
                     G_CALLBACK(cb_collect_message), &notification_message);
    g_signal_connect(test, "failure",
                     G_CALLBACK(cb_collect_message), &failure_message);
    g_signal_connect(test, "omission",
                     G_CALLBACK(cb_collect_message), &omission_message);

    cut_assert_true(run());

    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_message),
                                         &notification_message);
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_message),
                                         &failure_message);
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_message),
                                         &omission_message);

    if (cut_test_context_is_multi_thread(get_current_test_context())) {
        cut_assert_equal_string(NULL, failure_message);
        cut_assert_equal_string(NULL, notification_message);
        cut_assert_equal_string("can't use cut_fork() in multi thread mode",
                                omission_message);
    } else {
        cut_assert_equal_string("Failure in child process", failure_message);
        cut_assert_equal_string("Notification in child process",
                                notification_message);
        cut_assert_equal_string(NULL, omission_message);
    }
}
#endif

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
