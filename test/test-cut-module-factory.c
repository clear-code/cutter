#include <cutter.h>
#include <cutter/cut-module-factory.h>
#include <cutter/cut-module-factory-utils.h>

#include "lib/cuttest-utils.h"

void test_module_names (void);
void test_module_arguments (void);
void test_default_module_dir (void);

static CutModuleFactory *factory;
static GOptionContext *option_context;
static GObject *object;

void
startup (void)
{
    gchar *factory_dir;

    cut_module_factory_init();

    factory_dir = g_build_filename(cuttest_get_base_dir(),
                                   "fixtures",
                                   "factory",
                                   NULL);
    cut_module_factory_load_all(factory_dir);
    g_free(factory_dir);
}

void
shutdown (void)
{
    cut_module_factory_unload();
    cut_module_factory_quit();
}

void
setup (void)
{
    factory = NULL;
    option_context = NULL;
    object = NULL;
}

void
teardown (void)
{
    if (factory)
        g_object_unref(factory);
    if (option_context)
        g_option_context_free(option_context);
    if (object)
        g_object_unref(object);
}

void
test_module_names (void)
{
    factory = cut_module_factory_new("type1", "module1", NULL);
    cut_assert(factory);
    g_object_unref(factory);

    factory = cut_module_factory_new("type1", "module2", NULL);
    cut_assert(factory);
    g_object_unref(factory);

    factory = cut_module_factory_new("type1", "module3", NULL);
    cut_assert(factory);
    g_object_unref(factory);

    factory = cut_module_factory_new("type1", "module4", NULL);
    cut_assert_null(factory);
}

void
test_module_arguments (void)
{
    gchar *function_name = NULL;
    gint argc = 3;
    gchar *argv[] = {
        "test program",
        "-f", "stub-function",
        NULL,
    };
    gchar **copy_argv;
    copy_argv = g_strdupv(argv);

    factory = cut_module_factory_new("type1", "module1", NULL);
    cut_assert(factory);

    option_context = g_option_context_new(NULL);
    cut_module_factory_set_option_group(factory, option_context);
    cut_assert(g_option_context_parse(option_context, &argc, &copy_argv, NULL));
    g_strfreev(copy_argv);

    object = cut_module_factory_create(factory);
    cut_assert(object);

    g_object_get(object,
                 "name", &function_name,
                 NULL);
    cut_assert_equal_string_with_free("stub-function", function_name);

    g_object_unref(factory);
    factory = NULL;
}

void
test_default_module_dir (void)
{
    const gchar *orig_dir;
    orig_dir = cut_module_factory_get_default_module_dir();

    cut_module_factory_set_default_module_dir("default_dir");
    cut_assert_equal_string("default_dir",
                            cut_module_factory_get_default_module_dir());
    cut_module_factory_set_default_module_dir(orig_dir);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
