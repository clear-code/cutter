#include "cutter.h"

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <glib.h>

void test_message_from_forked_process (void);
void test_fail_in_forked_process (void);

void
setup (void)
{
}

void
teardown (void)
{
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

void
test_fail_in_forked_process (void)
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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
