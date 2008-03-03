#include "cutter.h"
#include "cut-module-factory.h"

#include "cuttest-utils.h"

void test_load_module (void);

static GList *modules;
static CutModule *module;

void
initialize (void)
{
    modules = cut_module_load_modules("./module_test_dir/.libs");
}

void
finalize (void)
{
    g_list_free(modules);
    modules = NULL;
}

void
setup (void)
{
    module = NULL;
}

void
teardown (void)
{
    if (module)
        g_object_unref(module);
}

void
test_load_module (void)
{
    cut_assert(modules);

    module = cut_module_find(modules, "test1");
    cut_assert(module);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
