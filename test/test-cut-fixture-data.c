#include <cutter.h>
#include <cuttest-utils.h>

void test_get_fixture_data_string (void);

void
setup (void)
{
    cut_set_fixture_data_dir(cuttest_get_base_dir(), "fixtures", "data");
}

void
test_get_fixture_data_string (void)
{
    cut_assert_equal_string("file1\n",
                            cut_get_fixture_data_string("1.txt"));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
