#include <cutter.h>

/* BUG entries */
const char *bug_get_bug_id (void);

/* tests */
void test_get_bug_id (void);

const char *bug_get_bug_id (void) { return "1234567890"; }
void
test_get_bug_id (void)
{
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
