#include "cutter.h"
#include "cut-test-suite.h"

static CutTestSuite *test_suite;

void
setup (void)
{
    test_suite = cut_test_suite_new();
}

void
teardown (void)
{
    g_object_unref(test_suite);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

