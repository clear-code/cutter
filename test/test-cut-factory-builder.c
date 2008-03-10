#include "cutter.h"
#include "cut-factory-builder.h"
#include "cut-ui-factory-builder.h"
#include "cut-report-factory-builder.h"

void test_has_builder (void);

void
initialize (void)
{
}

void
setup (void)
{
}

void
teardown (void)
{
}

void
test_has_builder (void)
{
    cut_assert(cut_factory_builder_has_builder("ui"));
    cut_assert(cut_factory_builder_has_builder("report"));
    cut_assert(!cut_factory_builder_has_builder("XXXX"));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
