#include "cuttest-utils.h"

const gchar *
cuttest_get_base_dir(void)
{
    const gchar *dir;

    dir = g_getenv("BASE_DIR");
    return dir ? dir : ".";
}

void
cuttest_add_test (CutTestCase *test_case, const gchar *test_name,
                  CutTestFunction test_function)
{
    CutTest *test;

    test = cut_test_new(test_name, test_function);
    cut_test_case_add_test(test_case, test);
    g_object_unref(test);
}
