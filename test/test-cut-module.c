#include "cutter.h"
#include "cut-module-factory.h"

#include "cuttest-utils.h"

void test_load_module (void);
void test_collect_names (void);
void test_collect_log_domains (void);
void test_collect_registered_types (void);
void test_load_modules_unique (void);

static GList *modules = NULL;
static gchar *modules_dir = NULL;

void
startup (void)
{
    modules_dir = g_build_filename(cuttest_get_base_dir(),
                                   "module_test_dir",
                                   ".libs",
                                   NULL);
    modules = cut_module_load_modules(modules_dir);
}

void
shutdown (void)
{
    g_list_free(modules);
    modules = NULL;

    g_free(modules_dir);
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

    cut_assert_equal_string("test1", names->data);
    cut_assert_equal_string("test2", g_list_next(names)->data);

    g_list_free(names);
}

void
test_collect_log_domains (void)
{
    GList *log_domains;

    cut_assert(modules);

    log_domains = cut_module_collect_log_domains(modules);
    cut_assert(log_domains);
    cut_assert_equal_int(2, g_list_length(log_domains));

    cut_assert_equal_string("TestDomain1", log_domains->data);
    cut_assert_equal_string("TestDomain2", g_list_next(log_domains)->data);

    g_list_foreach(log_domains, (GFunc)g_free, NULL);
    g_list_free(log_domains);
}

void
test_collect_registered_types (void)
{
    GList *registered_types;

    cut_assert(modules);

    registered_types = cut_module_collect_registered_types(modules);
    cut_assert(registered_types);
    cut_assert_equal_int(2, g_list_length(registered_types));

    cut_assert_equal_string("CutModuleTest1", registered_types->data);
    cut_assert_equal_string("CutModuleTest2", g_list_next(registered_types)->data);

    g_list_free(registered_types);
}

void
test_load_modules_unique (void)
{
    GList *reloaded_modules;

    cut_assert(modules);
    cut_assert_equal_int(2, g_list_length(modules));

    reloaded_modules = cut_module_load_modules_unique(modules_dir, modules);
    cut_assert(reloaded_modules);

    cut_assert_equal_int(2, g_list_length(reloaded_modules));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
