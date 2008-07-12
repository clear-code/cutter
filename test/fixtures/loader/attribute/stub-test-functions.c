#include <cutter.h>

/* BUG entries */
const char *bug_get_bug_id (void);

/* attribute */
const CutTestAttribute *attributes_attribute (void);
const char *description_description (void);

/* tests */
void test_get_bug_id (void);
void test_attribute (void);
void test_description (void);

const char *bug_get_bug_id (void) { return "1234567890"; }
void
test_get_bug_id (void)
{
}


const CutTestAttribute *
attributes_attribute (void)
{
    static const CutTestAttribute attributes[] = {
        {"bug", "1234"},
        {"priority", "5678"},
        {"bug", "9"},
        {NULL, NULL}
    };

    return attributes;
}

void
test_attribute (void)
{
}

const char *
description_description (void)
{
    return "This message is the description of test_description()";
}

void
test_description (void)
{
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
