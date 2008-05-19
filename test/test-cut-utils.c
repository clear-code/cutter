#include <cutter.h>
#include <cutter/cut-utils.h>

void test_compare_string_array (void);
void test_inspect_string_array (void);
void test_is_interested_diff (void);
void test_strv_concat (void);

void
test_compare_string_array (void)
{
    gchar *strings1[] = {"a", "b", "c", NULL};
    gchar *strings2[] = {"a", "b", "c", NULL};
    gchar *strings3[] = {"a", "b", NULL};
    gchar *strings4[] = {"a", "b", "d", NULL};

    cut_assert(cut_utils_compare_string_array(strings1, strings2));
    cut_assert(!cut_utils_compare_string_array(NULL, strings2));
    cut_assert(cut_utils_compare_string_array(NULL, NULL));
    cut_assert(!cut_utils_compare_string_array(strings1, strings3));
    cut_assert(!cut_utils_compare_string_array(strings1, strings4));
}


void
test_inspect_string_array (void)
{
    gchar *strings[] = {"a", "b", "c", NULL};

    cut_assert_equal_string_with_free("(null)",
                                      cut_utils_inspect_string_array(NULL));
    cut_assert_equal_string_with_free("[\"a\", \"b\", \"c\"]",
                                      cut_utils_inspect_string_array(strings));
}

void
test_is_interested_diff (void)
{
    cut_assert_false(cut_utils_is_interested_diff(NULL));
    cut_assert_false(cut_utils_is_interested_diff(""));
    cut_assert_false(cut_utils_is_interested_diff(" a\n"
                                                  " b\n"
                                                  " c"));
    cut_assert_false(cut_utils_is_interested_diff("- abc\n"
                                                  "+ abc"));
    cut_assert_true(cut_utils_is_interested_diff("- a\n"
                                                 "+ b\n"
                                                 "+ c"));
    cut_assert_true(cut_utils_is_interested_diff("- abc\n"
                                                 "+ abc\n"
                                                 "  xyz"));
    cut_assert_true(cut_utils_is_interested_diff("- abc def ghi xyz\n"
                                                 "?     ^^^\n"
                                                 "+ abc DEF ghi xyz\n"
                                                 "?     ^^^"));
    cut_assert_true(cut_utils_is_interested_diff("  a\n"
                                                 "- abc def ghi xyz\n"
                                                 "?     ^^^\n"
                                                 "+ abc DEF ghi xyz\n"
                                                 "?     ^^^"));
}

void
test_strv_concat (void)
{
    const gchar *strings[] = {"a", "b", "c", NULL};
    gchar *expected[] = {"a", "b", "c", "d", "e", NULL};

    cut_assert_equal_string_array(expected, 
                                  cut_utils_strv_concat(strings, "d", "e", NULL));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
