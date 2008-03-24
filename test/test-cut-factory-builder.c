#include "cutter.h"
#include "cut-factory-builder.h"
#include "cut-ui-factory-builder.h"
#include "cut-report-factory-builder.h"

void test_type_name (void);

static GObject *builder;

void
setup (void)
{
    builder = NULL;
}

void
teardown (void)
{
    if (builder)
        g_object_unref(builder);
}

void
test_type_name (void)
{
    builder = g_object_new(CUT_TYPE_UI_FACTORY_BUILDER, NULL);
    cut_assert(builder);
    cut_assert_equal_string("ui",
                            cut_factory_builder_get_type_name(CUT_FACTORY_BUILDER(builder)));
    g_object_unref(builder);
    builder = NULL;

    builder = g_object_new(CUT_TYPE_REPORT_FACTORY_BUILDER, NULL);
    cut_assert(builder);
    cut_assert_equal_string("report",
                            cut_factory_builder_get_type_name(CUT_FACTORY_BUILDER(builder)));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
