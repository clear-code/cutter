#include <cutter.h>
#include <cutter/cut-runner.h>

void test_sort_test_cases(void);

static CutRunner *runner;
static GList *test_cases;
static gchar **expected_names, **actual_names;

void
setup (void)
{
    runner = cut_runner_new();
    test_cases = NULL;
    expected_names = NULL;
    actual_names = NULL;
}

void
teardown (void)
{
    g_object_unref(runner);

    g_list_foreach(test_cases, (GFunc)g_object_unref, NULL);
    g_list_free(test_cases);

    g_strfreev(expected_names);
    g_strfreev(actual_names);
}

static gchar **
collect_test_case_names (GList *test_cases)
{
    gint i;
    GList *node;
    gchar **names;

    names = g_new0(gchar *, g_list_length(test_cases) + 1);
    for (i = 0, node = test_cases;
         node;
         i++, node = g_list_next(node)) {
        CutTestCase *test_case;

        test_case = CUT_TEST_CASE(node->data);
        names[i] = g_strdup(cut_test_get_name(CUT_TEST(test_case)));
    }
    names[i] = NULL;

    return names;
}

void
test_sort_test_cases (void)
{
    test_cases = g_list_append(test_cases,
                               cut_test_case_new("abc", NULL, NULL,
                                                 NULL, NULL, NULL, NULL));
    test_cases = g_list_append(test_cases,
                               cut_test_case_new("xyz", NULL, NULL,
                                                 NULL, NULL, NULL, NULL));
    test_cases = g_list_append(test_cases,
                               cut_test_case_new("123", NULL, NULL,
                                                 NULL, NULL, NULL, NULL));
    test_cases = g_list_append(test_cases,
                               cut_test_case_new("XYZ", NULL, NULL,
                                                 NULL, NULL, NULL, NULL));

    expected_names = g_strsplit("abc xyz 123 XYZ", " ", -1);
    actual_names = collect_test_case_names(test_cases);
    cut_assert_equal_string_array(expected_names, actual_names);
    g_strfreev(actual_names);

    test_cases = cut_runner_sort_test_cases(runner, test_cases);
    actual_names = collect_test_case_names(test_cases);
    cut_assert_equal_string_array(expected_names, actual_names);
    g_strfreev(expected_names);
    g_strfreev(actual_names);

    cut_runner_set_test_case_order(runner, CUT_ORDER_NAME_ASCENDING);
    test_cases = cut_runner_sort_test_cases(runner, test_cases);
    expected_names = g_strsplit("123 XYZ abc xyz", " ", -1);
    actual_names = collect_test_case_names(test_cases);
    cut_assert_equal_string_array(expected_names, actual_names);
    g_strfreev(expected_names);
    g_strfreev(actual_names);

    cut_runner_set_test_case_order(runner, CUT_ORDER_NAME_DESCENDING);
    test_cases = cut_runner_sort_test_cases(runner, test_cases);
    expected_names = g_strsplit("xyz abc XYZ 123", " ", -1);
    actual_names = collect_test_case_names(test_cases);
    cut_assert_equal_string_array(expected_names, actual_names);
}


/*
vi:nowrap:ai:expandtab:sw=4
*/
