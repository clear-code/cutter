#include "cut.h"
#include "cut-loader.h"

void test_load_function (void);

static CutLoader *test_loader;

void
setup (void)
{
    test_loader = cut_loader_new("loader_test_dir/.libs");
}

void
teardown (void)
{
    g_object_unref(test_loader);
}

void
test_load_function (void)
{
    CutTestCase *test_case;

    test_case = cut_loader_load_test_case(test_loader);

    cut_assert(test_case);
    cut_assert_equal_int(4, cut_test_case_get_test_count(test_case));

    g_object_unref(test_case);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
