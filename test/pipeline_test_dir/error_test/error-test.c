#include <cutter.h>

void test_failure (void);
void test_pending (void);
void test_error (void);

void
test_failure (void)
{
    cut_fail("Failed."); 
}

void
test_pending (void)
{
    cut_pend("Pending..."); 
}

void
test_error (void)
{
    cut_error("Error"); 
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
