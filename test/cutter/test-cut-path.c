/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cutter.h>
#include <gcutter.h>
#include <cutter/cut-pipeline.h>
#include <cutter/cut-runner.h>

#include "../lib/cuttest-assertions.h"

void test_run (void);

static CutRunContext *pipeline;
static gchar *test_dir;

void
cut_setup (void)
{
    pipeline = cut_pipeline_new();
    test_dir = g_build_filename(cuttest_get_base_dir(),
                                "fixtures",
                                "path",
                                NULL);
}

void
cut_teardown (void)
{
    if (pipeline)
        g_object_unref(pipeline);
    if (test_dir)
        g_free(test_dir);
}

static gboolean
run (void)
{
    const gchar *exclude_directories[] = {NULL};

    cut_run_context_set_exclude_directories(pipeline, exclude_directories);

    cut_run_context_set_test_directory(pipeline, test_dir);
    cut_run_context_set_source_directory(pipeline, test_dir);

    return cut_run_context_start(pipeline);
}

void
test_run (void)
{
    cut_assert_false(run());

    cut_assert_test_result_summary(pipeline, 1, 0, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result_with_message(
        pipeline, 0, CUT_TEST_RESULT_FAILURE,
        "test_fail_in_deep_path",
        NULL,
        NULL,
        "fail in parent" G_DIR_SEPARATOR_S "sub" G_DIR_SEPARATOR_S "test-deep.c",
        cut_take_printf("%s" G_DIR_SEPARATOR_S "parent" G_DIR_SEPARATOR_S "sub"
                        G_DIR_SEPARATOR_S "test-deep.c:%d", test_dir, 12),
        "test_fail_in_deep_path");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
