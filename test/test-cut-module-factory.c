#include "cutter.h"
#include "cut-module-factory.h"

#include "cuttest-utils.h"

void test_module_names (void);

static CutModuleFactory *factory;

void
initialize (void)
{
    cut_module_factory_init();
    cut_module_factory_load("./factory_test_dir");
}

void
finalize (void)
{
    cut_module_factory_unload();
    cut_module_factory_quit();
}

void
setup (void)
{
    factory = NULL;
}

void
teardown (void)
{
    if (factory)
        g_object_unref(factory);
}

void
test_module_names (void)
{
    factory = cut_module_factory_new("type1", "factory_module1", NULL);
    cut_assert(factory);
    g_object_unref(factory);

    factory = cut_module_factory_new("type1", "factory_module2", NULL);
    cut_assert(factory);
    g_object_unref(factory);

    factory = cut_module_factory_new("type1", "factory_module2", NULL);
    cut_assert(factory);
    g_object_unref(factory);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
