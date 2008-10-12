/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string.h>

#include <cutter.h>
#include <cutter/cut-repository.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-test-container.h>

#include "lib/cuttest-utils.h"

void test_create_test_suite (void);

static CutRepository *test_repository;

void
setup (void)
{
    gchar *test_repository_path;

    test_repository_path = g_build_filename(cuttest_get_base_dir(),
                                            "fixtures",
                                            "repository",
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
    "repository-test-case1",
    "repository-test-case2",
    "repository-test-case3"
};
static const gint n_expected_test_case_names = G_N_ELEMENTS(expected_test_case_name);

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
    CutTestContainer *container;
    GList *test_cases, *list;
    gint i;

    suite = cut_repository_create_test_suite(test_repository);
    cut_assert(suite);

    container = CUT_TEST_CONTAINER(suite);
    test_cases = (GList *)cut_test_container_get_children(container);
    cut_assert_not_null(test_cases);

    test_cases = g_list_sort(test_cases, compare_test_case_name);
    for (list = test_cases, i = 0; list; list = g_list_next(list), i++) {
        CutTestCase *test_case;

        cut_assert(i < n_expected_test_case_names);
        cut_assert(CUT_IS_TEST_CASE(list->data));
        test_case = CUT_TEST_CASE(list->data);

        cut_assert_equal_string(expected_test_case_name[i],
                                cut_test_get_name(CUT_TEST(test_case)));
    }

    g_object_unref(suite);
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
