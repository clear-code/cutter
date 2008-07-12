#include <cutter.h>
#include <cutter/cut-loader.h>

#include <cuttest-utils.h>

void test_load_valid (void);
void test_load_invalid (void);
void test_load_none (void);

static CutLoader *loader;
static CutTestSuite *test_suite;

void
setup (void)
{
    loader = NULL;
    test_suite = NULL;
}

void
teardown (void)
{
    if (loader)
        g_object_unref(loader);
    if (test_suite)
        g_object_unref(test_suite);
}

static CutLoader *
create_loader (const gchar *so_filename)
{
    CutLoader *new_loader;
    gchar *so_path;

    so_path = g_build_filename(cuttest_get_base_dir(),
                               "fixtures",
                               "loader",
                               "suite",
                               ".libs",
                               so_filename,
                               NULL);
    new_loader = cut_loader_new(so_path);
    g_free(so_path);

    return new_loader;
}

void
test_load_valid (void)
{
    CutWarmupFunction warmup_function = NULL;
    CutCooldownFunction cooldown_function = NULL;

    loader = create_loader("suite_cuttest_loader_valid." G_MODULE_SUFFIX);
    test_suite = cut_loader_load_test_suite(loader);
    cut_assert(test_suite);

    g_object_get(G_OBJECT(test_suite),
                 "warmup-function", &warmup_function,
                 "cooldown-function", &cooldown_function,
                 NULL);
    cut_assert(warmup_function);
    cut_assert(cooldown_function);
}

void
test_load_invalid (void)
{
    CutWarmupFunction warmup_function = NULL;
    CutCooldownFunction cooldown_function = NULL;

    loader = create_loader("suite_cuttest_loader_invalid." G_MODULE_SUFFIX);
    test_suite = cut_loader_load_test_suite(loader);
    cut_assert(test_suite);

    g_object_get(G_OBJECT(test_suite),
                 "warmup-function", &warmup_function,
                 "cooldown-function", &cooldown_function,
                 NULL);
    cut_assert_null(warmup_function);
    cut_assert_null(cooldown_function);
}

void
test_load_none (void)
{
    CutWarmupFunction warmup_function = NULL;
    CutCooldownFunction cooldown_function = NULL;

    loader = create_loader("suite_cuttest_loader_none." G_MODULE_SUFFIX);
    test_suite = cut_loader_load_test_suite(loader);
    cut_assert(test_suite);

    g_object_get(G_OBJECT(test_suite),
                 "warmup-function", &warmup_function,
                 "cooldown-function", &cooldown_function,
                 NULL);
    cut_assert_null(warmup_function);
    cut_assert_null(cooldown_function);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
