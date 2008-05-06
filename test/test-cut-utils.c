#include <cutter.h>
#include <cutter/cut-utils.h>

void test_compare_string_array (void);
void test_inspect_string_array (void);

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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
