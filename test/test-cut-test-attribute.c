#include "cutter.h"
#include "cut-test.h"
#include "cut-test-result.h"
#include "cut-loader.h"
#include "cut-utils.h"
#include "cuttest-utils.h"
#include "cuttest-assertions.h"

void test_set_attribute (void);
void test_get_bug_id (void);
void test_get_attribute (void);
void test_get_description (void);

static CutTest *test;
static CutLoader *test_loader;
static CutTestCase *test_case;

#define cut_omit_if_loader_does_not_support_attribute() do      \
{                                                               \
    if (!cut_loader_support_attribute(test_loader))             \
        cut_omit("test attribute loading isn't "                \
                 "supported on the environment.");              \
} while (0)

static void
fail_test (void)
{
    cut_assert_equal_string("abc", "xyz");
}

void
setup (void)
{
    gchar *test_path;

    test_path = g_build_filename(cuttest_get_base_dir(),
                                 "attribute_test_dir",
                                 ".libs",
                                 "dummy_attribute_test." G_MODULE_SUFFIX,
                                 NULL);
    test_loader = cut_loader_new(test_path);
    g_free(test_path);

    test_case = NULL;
    test = NULL;
}

void
teardown (void)
{
    if (test)
        g_object_unref(test);
    if (test_loader)
        g_object_unref(test_loader);
    if (test_case)
        g_object_unref(test_case);
}

void
test_set_attribute (void)
{
    test = cut_test_new("fail-test", fail_test);
    cut_assert(test);
    cut_assert_null(cut_test_get_attribute(test, "category"));
    cut_test_set_attribute(test, "category", "assertion");
    cut_assert_equal_string("assertion",
                            cut_test_get_attribute(test, "category"));
    cut_test_set_attribute(test, "category", "test");
    cut_assert_equal_string("test", cut_test_get_attribute(test, "category"));
}

void
test_get_bug_id (void)
{
    CutTestContainer *container;
    GList *tests;
    const gchar *filter[] = {"/test_get_bug_id/", NULL};

    cut_omit_if_loader_does_not_support_attribute();

    test_case = cut_loader_load_test_case(test_loader);
    cut_assert(test_case);

    container = CUT_TEST_CONTAINER(test_case);
    tests = cut_test_container_filter_children(container, filter);
    cut_assert(tests);

    cut_assert(1, g_list_length(tests));
    cut_assert_equal_string("1234567890", cut_test_get_attribute(CUT_TEST(tests->data), "bug"));
}

void
test_get_attribute (void)
{
    CutTestContainer *container;
    GList *tests;
    const gchar *filter[] = {"/test_attribute/", NULL};

    cut_omit_if_loader_does_not_support_attribute();

    test_case = cut_loader_load_test_case(test_loader);
    cut_assert(test_case);

    container = CUT_TEST_CONTAINER(test_case);
    tests = cut_test_container_filter_children(container, filter);
    cut_assert(tests);

    cut_assert(1, g_list_length(tests));
    cut_assert_equal_string("9", cut_test_get_attribute(CUT_TEST(tests->data),
                                                        "bug"));
    cut_assert_equal_string("5678", cut_test_get_attribute(CUT_TEST(tests->data),
                                                           "priority"));
}

void
test_get_description (void)
{
    CutTestContainer *container;
    GList *tests;
    const gchar *filter[] = {"/test_description/", NULL};

    cut_omit_if_loader_does_not_support_attribute();

    test_case = cut_loader_load_test_case(test_loader);
    cut_assert(test_case);

    container = CUT_TEST_CONTAINER(test_case);
    tests = cut_test_container_filter_children(container, filter);
    cut_assert(tests);

    cut_assert(1, g_list_length(tests));
    cut_assert_equal_string("This message is the description of test_description()",
                            cut_test_get_description(CUT_TEST(tests->data)));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
