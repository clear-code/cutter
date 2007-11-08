#include "cutter.h"
#include "cut-context.h"
#include "cut-repository.h"
#include "cut-test-case.h"
#include "cut-test-container.h"

#include "cuttest-utils.h"

void test_create_test_suite (void);

static CutRepository *test_repository;

void
setup (void)
{
    gchar *test_repository_path;

    test_repository_path = g_build_filename(cuttest_get_base_dir(),
                                            "repository_test_dir",
                                            NULL);
    test_repository = cut_repository_new(test_repository_path);
    g_free(test_repository_path);
}

void
teardown (void)
{
    g_object_unref(test_repository);
}

static const gchar *expected_test_case_name[] = {
"repository_test_case1",
"repository_test_case2",
"repository_test_case3"
};

static gint
compare_test_case_name (gconstpointer a, gconstpointer b)
{
    CutTest *ta, *tb;
    ta = CUT_TEST(a);
    tb = CUT_TEST(b);

    return strcmp(cut_test_get_name(ta), cut_test_get_name(tb));
}

void
test_create_test_suite (void)
{
    CutTestSuite *suite;
    GList *test_cases, *list;
    gint i;

    suite = cut_repository_create_test_suite(test_repository);
    cut_assert(suite);

    test_cases = (GList *) cut_test_container_get_children(CUT_TEST_CONTAINER(suite));
    cut_assert(test_cases);

    test_cases = g_list_sort(test_cases, compare_test_case_name);
    for (list = test_cases, i = 0; list; list = g_list_next(list), i++) {
        CutTestCase *test_case;

        cut_assert(CUT_IS_TEST_CASE(list->data));
        test_case = CUT_TEST_CASE(list->data);

        cut_assert_equal_string(expected_test_case_name[i],
                                cut_test_get_name(CUT_TEST(test_case)));
    }

    g_object_unref(suite);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
