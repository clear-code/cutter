#include "cutter.h"
#include <cutter/cut-test-context.h>

static CutTestContext *context;

void
setup (void)
{
    context = cut_test_context_new(NULL, NULL, NULL);
}

void
teardown (void)
{
    g_object_unref(context);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
