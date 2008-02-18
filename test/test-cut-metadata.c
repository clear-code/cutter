#include "cutter.h"
#include "cut-test.h"
#include "cut-test-result.h"
#include "cut-utils.h"
#include "cut-runner.h"
#include "cuttest-assertions.h"

void test_set_metadata(void);

static CutTest *test;

static void
fail_test (void)
{
    cut_assert_equal_string("abc", "xyz");
}

void
setup (void)
{
    test = NULL;
}

void
teardown (void)
{
    if (test)
        g_object_unref(test);
}

void
test_set_metadata (void)
{
    test = cut_test_new("fail-test", NULL, fail_test);
    cut_assert(test);
    cut_assert_null(cut_test_get_metadata(test, "category"));
    cut_test_set_metadata(test, "category", "assertion");
    cut_assert_equal_string("assertion",
                            cut_test_get_metadata(test, "category"));
    cut_test_set_metadata(test, "category", "test");
    cut_assert_equal_string("test", cut_test_get_metadata(test, "category"));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

