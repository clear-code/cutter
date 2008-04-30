#include "cutter.h"
#include <cutter/cut-utils.h>

void test_compare_string_array (void);

void
test_compare_string_array (void)
{
    const gchar *strings1[] = {"a", "b", "c", NULL};
    const gchar *strings2[] = {"a", "b", "c", NULL};
    const gchar *strings3[] = {"a", "b", NULL};
    const gchar *strings4[] = {"a", "b", "d", NULL};

    cut_assert(cut_utils_compare_string_array(strings1, strings2));
    cut_assert(!cut_utils_compare_string_array(NULL, strings2));
    cut_assert(cut_utils_compare_string_array(NULL, NULL));
    cut_assert(!cut_utils_compare_string_array(strings1, strings3));
    cut_assert(!cut_utils_compare_string_array(strings1, strings4));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
