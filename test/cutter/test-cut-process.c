/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <errno.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include <cutter.h>
#include <cutter/cut-process.h>

void test_fork (void);

static CutProcess *process;

void
cut_setup (void)
{
    process = cut_process_new();
}

void
cut_teardown (void)
{
    g_object_unref(process);
}

void
test_fork (void)
{
    int pid;

    pid = cut_process_fork(process);
    if (pid == -1 && errno == ENOSYS)
        cut_omit("cut_process_fork() can't be used on the environment.");

    if (pid == 0) {
        _exit(EXIT_SUCCESS);
    }

    cut_assert_equal_int (EXIT_SUCCESS, cut_process_wait(process, 0));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
