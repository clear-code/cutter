#include "cutter.h"

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

void test_fork (void);

void
setup (void)
{
}

void
teardown (void)
{
}

void
test_fork (void)
{
    pid_t pid;
    int status;

    pid = cut_fork(10);

    if (pid == 0) {
        cut_notify("Walk in child process");
        cut_fail("An error was occured");
        _exit(0);
    } else if (pid > 0) {
        int wr;
        cut_notify("Go on parent process");
        do
            wr = waitpid(pid, &status, 0);
        while (wr < 0 && errno == EINTR);
        cut_notify("End of child process");
    } else {
        cut_fail("Failed to create a child process");
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
