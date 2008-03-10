#include "cutter.h"
#include "cut-module-factory.h"

#include "cuttest-utils.h"

void test_load_module (void);
void test_collect_names (void);

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

void
test_collect_names (void)
{
    GList *names;

    cut_assert(modules);

    names = cut_module_collect_names(modules);
    cut_assert(names);
    cut_assert_equal_int(2, g_list_length(names));

    cut_assert("test1", names->data);
    cut_assert("test2", g_list_next(names)->data);

    g_list_free(names);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
