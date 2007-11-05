#include "cutter.h"
#include "cut-test-case.h"
#include "cut-test-suite.h"
#include "cut-loader.h"

void test_run_test_case (void);

static CutTestSuite *test_object;
static CutLoader *loader1, *loader2;

void
setup (void)
{
    CutTestCase *test_case;
    test_object = cut_test_suite_new();

    loader1 = cut_loader_new("loader_test_dir/.libs/libdummy_loader_test.so");
    loader2 = cut_loader_new("loader_test_dir/.libs/libdummy_loader_test2.so");
    test_case = cut_loader_load_test_case(loader1);
    cut_test_container_add_test(CUT_TEST_CONTAINER(test_object), CUT_TEST(test_case));
    test_case = cut_loader_load_test_case(loader2);
    cut_test_container_add_test(CUT_TEST_CONTAINER(test_object), CUT_TEST(test_case));
}

void
teardown (void)
{
    g_object_unref(loader1);
    g_object_unref(loader2);
    g_object_unref(test_object);
}

void
test_run_test_case (void)
{
    cut_test_suite_run_test_case (test_object, "dummy_loader_test2");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

