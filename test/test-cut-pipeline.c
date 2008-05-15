#include "cutter.h"
#include <cutter/cut-pipeline.h>

#include <unistd.h>

void test_fork (void);

static CutPipeline *pipeline;

void
setup (void)
{
    pipeline = NULL;
}

void
teardown (void)
{
    if (pipeline)
        g_object_unref(pipeline);
}

void
test_fork (void)
{
    pipeline = cut_pipeline_new("pipeline_test/.libs");
    cut_assert(pipeline);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
