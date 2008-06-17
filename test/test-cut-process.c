#include "cutter.h"
#include <cutter/cut-process.h>

#include <unistd.h>

void test_fork (void);

static CutProcess *process;

void
setup (void)
{
    process = cut_process_new();
}

void
teardown (void)
{
    g_object_unref(process);
}

void
test_fork (void)
{
#ifndef G_OS_WIN32
    int pid;
    pid = cut_process_fork(process);

    if (pid == 0) {
        _exit(EXIT_SUCCESS);
    }

    cut_assert_equal_int (EXIT_SUCCESS, cut_process_wait(process, 0));
#endif
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
