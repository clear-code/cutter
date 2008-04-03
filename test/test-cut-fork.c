#include "cutter.h"
#include <cutter/cut-test.h>
#include <cutter/cut-runner.h>
#include <cutter/cut-test-result.h>

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <glib.h>

void test_message_from_forked_process (void);
void test_fail_in_forked_process (void);

static CutRunner *runner;
static CutTest *test_object;

void
setup (void)
{
    runner = cut_runner_new();

    test_object = NULL;
}

void
teardown (void)
{
    if (test_object)
        g_object_unref(test_object);
    g_object_unref(runner);
}

void
test_message_from_forked_process (void)
{
    int pid;
    int status;

    pid = cut_fork(0);

    if (pid == 0) {
        g_print("Walk in child process");
        g_printerr("An error was occured");
        _exit(0);
    } else if (pid > 0) {
        int wr;
        do
            wr = waitpid(pid, &status, 0);
        while (wr < 0 && errno == EINTR);
        cut_assert_equal_string("Walk in child process", cut_fork_get_stdout_message(pid));
        cut_assert_equal_string("An error was occured", cut_fork_get_stderr_message(pid));
    } else {
        cut_fail("Failed to create a child process");
    }
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

static void
cut_fail_in_forked_process (void)
{
    int pid;
    int status;

    pid = cut_fork(0);

    if (pid == 0) {
        cut_notify("Notification in child process");
        cut_fail("Failure in child process");
        _exit(0);
    } else if (pid > 0) {
        int wr;
        do
            wr = waitpid(pid, &status, 0);
        while (wr < 0 && errno == EINTR);
    } else {
        cut_fail("Failed to create a child process");
    }
}

static void
cb_notification_signal (CutTest *test, CutTestContext *context, CutTestResult *result, gpointer data)
{
    cut_assert_equal_string("Notification in child process",
                            cut_test_result_get_user_message(result));
}

void
test_fail_in_forked_process (void)
{
    test_object = cut_test_new("failure", cut_fail_in_forked_process);
    g_signal_connect(test_object, "notification", G_CALLBACK(cb_notification_signal), NULL);
    cut_assert(run(test_object));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_notification_signal),
                                         NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
