#include <cutter.h>

#include "cuttest-utils.h"

void test_directory (void);

void
test_directory (void)
{
    const char *this_directory;

    this_directory = cut_build_path(cuttest_get_base_dir(),
                                    "fixtures",
                                    "test-directory",
                                    NULL);
    cut_assert_equal_string(this_directory,
                            cut_get_test_directory());
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
