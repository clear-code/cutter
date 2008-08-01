#include <gcutter.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-loader.h>
#include <cutter/cut-utils.h>
#include "lib/cuttest-assertions.h"

void test_set_attribute (void);
void test_bug_id (void);
void test_multi_attributes (void);
void test_description (void);

static GList *tests;
static GList *expected_names;
static CutTest *test;
static CutLoader *test_loader;
static CutTestCase *test_case;

#define cut_omit_if_loader_does_not_support_attribute(loader) do        \
{                                                                       \
    if (!cut_loader_support_attribute(loader))                          \
        cut_omit("test attribute loading isn't "                        \
                 "supported on the environment.");                      \
} while (0)

void
setup (void)
{
    gchar *test_path;

    test_path = g_build_filename(cuttest_get_base_dir(),
                                 "fixtures",
                                 "loader",
                                 "attribute",
                                 ".libs",
                                 "stub_attribute_test." G_MODULE_SUFFIX,
                                 NULL);
    test_loader = cut_loader_new(test_path);
    g_free(test_path);

    expected_names = NULL;
    tests = NULL;

    test_case = NULL;
    test = NULL;
}

void
teardown (void)
{
    if (expected_names)
        gcut_list_string_free(expected_names);
    if (tests)
        g_list_free(tests);

    if (test)
        g_object_unref(test);
    if (test_loader)
        g_object_unref(test_loader);
    if (test_case)
        g_object_unref(test_case);
}

static const GList *
collect_test_names (GList *tests)
{
    GList *node;
    GList *names = NULL;

    for (node = tests; node; node = g_list_next(node)) {
        CutTest *test = node->data;

        names = g_list_append(names, g_strdup(cut_test_get_name(test)));
    }

    return gcut_take_list(names, g_free);
}

static void
stub_fail_test (void)
{
    cut_assert_equal_string("abc", "xyz");
}

void
test_set_attribute (void)
{
    test = cut_test_new("fail-test", stub_fail_test);
    cut_assert(test);
    cut_assert_null(cut_test_get_attribute(test, "category"));
    cut_test_set_attribute(test, "category", "assertion");
    cut_assert_equal_string("assertion",
                            cut_test_get_attribute(test, "category"));
    cut_test_set_attribute(test, "category", "test");
    cut_assert_equal_string("test", cut_test_get_attribute(test, "category"));
}

void
test_bug_id (void)
{
    CutTestContainer *container;
    const gchar *filter[] = {"/test_bug_id/", NULL};

    test_case = cut_loader_load_test_case(test_loader);
    cut_assert(test_case);

    container = CUT_TEST_CONTAINER(test_case);
    tests = cut_test_container_filter_children(container, filter);
    expected_names = gcut_list_string_new("test_bug_id", NULL);
    gcut_assert_equal_list_string(expected_names,
                                  collect_test_names(tests));

    cut_omit_if_loader_does_not_support_attribute(test_loader);

    cut_assert_equal_string("1234567890",
                            cut_test_get_attribute(CUT_TEST(tests->data),
                                                   "bug"));
}

void
test_multi_attributes (void)
{
    CutTestContainer *container;
    CutTest *test;
    GHashTable *expected;
    const gchar *filter[] = {"/test_attribute/", NULL};

    test_case = cut_loader_load_test_case(test_loader);
    cut_assert(test_case);

    container = CUT_TEST_CONTAINER(test_case);
    tests = cut_test_container_filter_children(container, filter);
    expected_names = gcut_list_string_new("test_attribute", NULL);
    gcut_assert_equal_list_string(expected_names,
                                  collect_test_names(tests));

    test = CUT_TEST(tests->data);
    expected = gcut_hash_table_string_string_new("bug", "9",
                                                 "priority", "5678",
                                                 NULL);
    gcut_take_hash_table(expected);
    gcut_assert_equal_hash_table_string_string(expected,
                                               cut_test_get_attributes(test));
}

void
test_description (void)
{
    CutTestContainer *container;
    const gchar *filter[] = {"/test_description/", NULL};

    test_case = cut_loader_load_test_case(test_loader);
    cut_assert(test_case);

    container = CUT_TEST_CONTAINER(test_case);
    tests = cut_test_container_filter_children(container, filter);
    expected_names = gcut_list_string_new("test_description", NULL);
    gcut_assert_equal_list_string(expected_names,
                                  collect_test_names(tests));

    cut_omit_if_loader_does_not_support_attribute(test_loader);

    cut_assert_equal_string("This message is the description of test_description()",
                            cut_test_get_description(CUT_TEST(tests->data)));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
