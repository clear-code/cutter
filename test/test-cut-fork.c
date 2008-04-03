#include "cutter.h"
#include <cutter/cut-test.h>
#include <cutter/cut-runner.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-test-context.h>

#include <unistd.h>
#include <stdlib.h>
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

    pid = cut_fork();
    cut_assert_errno();

    if (pid == 0) {
        g_print("Walk in child process");
        g_printerr("An error was occured");
        _exit(EXIT_SUCCESS);
    }

    cut_assert_equal_int(EXIT_SUCCESS,
                         WEXITSTATUS(cut_wait_process(pid, 100)));
    cut_assert_equal_string("Walk in child process",
                            cut_fork_get_stdout_message(pid));
    cut_assert_equal_string("An error was occured",
                            cut_fork_get_stderr_message(pid));
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

    pid = cut_fork();
    cut_assert_errno();

    if (pid == 0) {
        cut_notify("Notification in child process");
        cut_fail("Failure in child process");
        _exit(EXIT_SUCCESS);
    }

    cut_assert_equal_int(EXIT_SUCCESS, WEXITSTATUS(cut_wait_process(pid, 100)));
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

    test_object = cut_test_new("failure", cut_fail_in_forked_process);

    g_signal_connect(test_object, "notification",
                     G_CALLBACK(cb_collect_message), &notification_message);
    g_signal_connect(test_object, "failure",
                     G_CALLBACK(cb_collect_message), &failure_message);
    g_signal_connect(test_object, "omission",
                     G_CALLBACK(cb_collect_message), &omission_message);

    cut_assert(run(test_object));

    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_collect_message),
                                         &notification_message);
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_collect_message),
                                         &failure_message);
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_collect_message),
                                         &omission_message);

    if (cut_test_context_get_multi_thread(get_current_test_context())) {
        cut_assert_null(failure_message);
        cut_assert_null(notification_message);
        cut_assert_equal_string("can't use cut_fork() in multi thread mode",
                                omission_message);
    } else {
        cut_assert_equal_string("Failure in child process", failure_message);
        cut_assert_equal_string("Notification in child process",
                                notification_message);
        cut_assert_null(omission_message);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
