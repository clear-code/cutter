#include "cut-test.h"

static void
test_function1 (void)
{
    g_warning("test_function1 is called");
}

static void
test_function2 (void)
{
    g_warning("test_function2 is called");
}

CutTestStruct cut_tests[] =
{
    {"test1", test_function1},
    {"test2", test_function2}
};
const gint cut_tests_len = G_N_ELEMENTS(cut_tests);

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
