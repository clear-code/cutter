/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>
#include <gcutter.h>

#define TEST_DIR_KEY "CUTTEST_SUB_PROCESS_GROUP_TEST_DIR"

void test_invoke(void);

void
test_invoke (void)
{
    int i;
    int n_processes = 3;
    CutSubProcessGroup *group;

    group = cut_sub_process_group_new();
    for (i = 0; i < n_processes; i++) {
        CutSubProcess *sub_process;

        sub_process = cut_sub_process_new(g_getenv(TEST_DIR_KEY));
        cut_sub_process_group_add(group, sub_process);
    }
    cut_sub_process_group_run(group);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
