#include "cutter.h"
#include <cutter/cut-verbose-level.h>

void test_parse (void);

void
test_parse (void)
{
    GError *error = NULL;
    cut_assert_equal_int(CUT_VERBOSE_LEVEL_NORMAL,
                         cut_verbose_level_parse(NULL, NULL));
    cut_assert_equal_int(CUT_VERBOSE_LEVEL_NORMAL,
                         cut_verbose_level_parse("n", NULL));
    cut_assert_equal_int(CUT_VERBOSE_LEVEL_NORMAL,
                         cut_verbose_level_parse("normal", NULL));
    cut_assert_equal_int(CUT_VERBOSE_LEVEL_SILENT,
                         cut_verbose_level_parse("s", NULL));
    cut_assert_equal_int(CUT_VERBOSE_LEVEL_SILENT,
                         cut_verbose_level_parse("silent", NULL));
    cut_assert_equal_int(CUT_VERBOSE_LEVEL_VERBOSE,
                         cut_verbose_level_parse("v", NULL));
    cut_assert_equal_int(CUT_VERBOSE_LEVEL_VERBOSE,
                         cut_verbose_level_parse("verbose", NULL));
    cut_assert_equal_int(CUT_VERBOSE_LEVEL_INVALID,
                         cut_verbose_level_parse("XXXXXX", &error));
    cut_assert(error);
    cut_assert_equal_int(CUT_VERBOSE_LEVEL_ERROR_BAD_VALUE, error->code);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
