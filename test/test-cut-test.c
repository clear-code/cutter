#include "cutter.h"
#include "cut-test.h"
#include "cut-context.h"

void test_get_name(void);
void test_get_description(void);
void test_increment_assertion_count(void);
void test_run(void);
void test_start_signal(void);
void test_complete_signal(void);

static CutContext *test_context;
static CutTest *test_object;
static gboolean run_test_flag = FALSE;
static gint n_start_signal = 0;
static gint n_complete_signal = 0;

static void
dummy_test_function (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    run_test_flag = TRUE;
}

void
setup (void)
{
    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);
    test_object = cut_test_new("dummy-test", "Dummy Test", dummy_test_function);
    run_test_flag = FALSE;
    n_start_signal = 0;
    n_complete_signal = 0;
}

void
teardown (void)
{
    g_object_unref(test_object);
    g_object_unref(test_context);
}

static void
cb_start_signal (CutTest *test, gpointer data)
{
    n_start_signal++;
}

static void
cb_complete_signal (CutTest *test, gpointer data)
{
    n_complete_signal++;
}

static gboolean
run (CutTest *test)
{
    return cut_test_run(test, test_context);
}

void
test_get_name (void)
{
    cut_assert_equal_string("dummy-test",
                            cut_test_get_name(test_object));
}

void
test_get_description (void)
{
    cut_assert_equal_string("Dummy Test",
                            cut_test_get_description(test_object));
}

void
test_run (void)
{
    cut_assert(run(test_object));
    cut_assert(run_test_flag);
}

void
test_start_signal (void)
{
    CutTest *test;

    test = cut_test_new("dummy-test", NULL, dummy_test_function);
    cut_assert(test);

    g_signal_connect(test, "start", G_CALLBACK(cb_start_signal), NULL);
    cut_assert(run(test));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_start_signal),
                                         NULL);
    cut_assert_equal_int(1, n_start_signal);
}

void
test_complete_signal (void)
{
    CutTest *test;

    test = cut_test_new("dummy-test", NULL, dummy_test_function);
    cut_assert(test);

    g_signal_connect(test, "complete", G_CALLBACK(cb_complete_signal), NULL);
    cut_assert(run(test));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_complete_signal),
                                         NULL);
    cut_assert_equal_int(1, n_complete_signal);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
