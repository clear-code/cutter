#include "cutter.h"
#include "cut-factory-builder.h"
#include "cut-ui-factory-builder.h"
#include "cut-report-factory-builder.h"

void test_register_builder (void);

void
setup (void)
{
}

void
teardown (void)
{
}

void
test_register_builder (void)
{
    cut_factory_builder_register_builder();

    cut_assert(g_type_class_peek(CUT_TYPE_UI_FACTORY_BUILDER));
    cut_assert(g_type_class_peek(CUT_TYPE_REPORT_FACTORY_BUILDER));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
