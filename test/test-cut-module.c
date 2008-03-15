#include "cutter.h"
#include "cut-module-factory.h"

#include "cuttest-utils.h"

void test_load_module (void);
void test_collect_names (void);

static GList *modules = NULL;

void
initialize (void)
{
    gchar *modules_dir;

    modules_dir = g_build_filename(cuttest_get_base_dir(),
                                   "module_test_dir",
                                   ".libs",
                                   NULL);
    modules = cut_module_load_modules(modules_dir);
    g_free(modules_dir);
}

void
finalize (void)
{
    g_list_foreach(modules, (GFunc)g_object_unref, NULL);
    g_list_free(modules);
    modules = NULL;
}

void
test_load_module (void)
{
    cut_assert(modules);

    cut_assert(cut_module_find(modules, "test1"));
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
