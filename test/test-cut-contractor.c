#include "cutter.h"
#include "cut-contractor.h"

void test_has_builder (void);

static CutContractor *contractor;

void
setup (void)
{
    contractor = NULL;
}

void
teardown (void)
{
    if (contractor)
        g_object_unref(contractor);
}

void
test_has_builder (void)
{
    contractor = cut_contractor_new();
    cut_assert(contractor);
    cut_assert(cut_contractor_has_builder(contractor, "ui"));
    cut_assert(cut_contractor_has_builder(contractor, "report"));
    cut_assert(!cut_contractor_has_builder(contractor, "XXX"));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
