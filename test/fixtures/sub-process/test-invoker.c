/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>
#include <gcutter.h>

void test_invoke(void);

void
test_invoke (void)
{
    CutSubProcess *sub_process;

    sub_process = cut_sub_process_new(g_getenv("CUTTEST_SUB_PROCESS_TEST_DIR"));
    cut_sub_process_run(sub_process);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
