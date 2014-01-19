#include "cutter.h"
#include "cut-contractor.h"

void test_has_listener_builder (void);

static CutContractor *contractor;

void
cut_setup (void)
{
    contractor = NULL;
}

void
cut_teardown (void)
{
    if (contractor)
        g_object_unref(contractor);
}

void
test_has_listener_builder (void)
{
    contractor = cut_contractor_new();
    cut_assert(contractor);
    cut_assert(cut_contractor_has_listener_builder(contractor, "ui"));
    cut_assert(cut_contractor_has_listener_builder(contractor, "report"));
    cut_assert(!cut_contractor_has_listener_builder(contractor, "XXX"));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
