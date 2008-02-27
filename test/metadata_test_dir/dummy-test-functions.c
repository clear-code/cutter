#include <cutter.h>

/* BUG entries */
const char *bug_get_bug_id (void);

/* metadata */
const CutTestMetadata *meta_metadata (void);

/* tests */
void test_get_bug_id (void);
void test_metadata (void);

const char *bug_get_bug_id (void) { return "1234567890"; }
void
test_get_bug_id (void)
{
}


const CutTestMetadata *
meta_metadata (void)
{
    static const CutTestMetadata metadata[] = {
        {"bug", "1234"},
        {"priority", "5678"},
        {"bug", "9"},
        {NULL, NULL}
    };

    return metadata;
}

void
test_metadata (void)
{
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
