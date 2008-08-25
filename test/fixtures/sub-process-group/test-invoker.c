/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>
#include <gcutter.h>

#define TEST_DIR_KEY "CUTTEST_SUB_PROCESS_GROUP_TEST_DIR"

void test_run(void);
void test_run_async(void);

static CutSubProcessGroup *
create_group (void)
{
    int i;
    int n_processes = 3;
    CutSubProcessGroup *group;

    group = cut_take_new_sub_process_group();
    for (i = 0; i < n_processes; i++) {
        CutSubProcess *sub_process;

        sub_process = cut_take_new_sub_process(g_getenv(TEST_DIR_KEY));
        cut_sub_process_group_add(group, sub_process);
    }
    return group;
}

void
test_run (void)
{
    CutSubProcessGroup *group;

    group = create_group();
    cut_sub_process_group_run(group);
}

void
test_run_async (void)
{
    CutSubProcessGroup *group;

    group = create_group();
    cut_sub_process_group_run_async(group);
    cut_sub_process_group_wait(group);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
