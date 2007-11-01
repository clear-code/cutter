#include "cut-test.h"
#include "cut-assertion.h"

static void
test_equal_int (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(-1, -1);
    cut_assert_equal_int(0, 0);
}

CutTestEntry cut_tests[] =
{
    {"euqal int test", test_equal_int},
};
const gint cut_tests_len = G_N_ELEMENTS(cut_tests);

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

