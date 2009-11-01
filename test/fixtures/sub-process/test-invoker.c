/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>
#include <gcutter.h>

#include "../../lib/cuttest-utils.h"

void test_invoke(void);

void
test_invoke (void)
{
    CutSubProcess *sub_process;
    const gchar *test_dir;

    test_dir = g_getenv(CUTTEST_TEST_DIR_KEY);
    cut_assert_not_null(test_dir, cut_message("ENV[%s]", CUTTEST_TEST_DIR_KEY));
    sub_process = cut_take_new_sub_process(test_dir);
    cut_sub_process_run(sub_process);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
