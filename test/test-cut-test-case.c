#include "cutter.h"
#include "cut-test-case.h"
#include "cut-context.h"

void test_setup(void);
void test_teardown(void);
void test_test_case_count(void);
void test_run(void);
void test_run_with_setup_error(void);
void test_run_this_function(void);
void test_run_tests_with_regex(void);
void test_run_with_name_filter(void);
void test_run_with_regex_filter(void);
void test_run_with_name_and_regex_filter(void);
void test_get_name(void);
void test_start_signal(void);
void test_complete_signal(void);

static CutTestCase *test_object;
static CutContext *test_context;

static gboolean set_error_on_setup = FALSE;

static gint n_setup = 0;
static gint n_teardown = 0;
static gint n_run_dummy_run_test_function = 0;
static gint n_run_dummy_test_function1 = 0;
static gint n_run_dummy_test_function2 = 0;
static gint n_start_signal = 0;
static gint n_complete_signal = 0;

static void
dummy_test_function1 (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    n_run_dummy_test_function1++;
}

static void
dummy_test_function2 (void)
{
    n_run_dummy_test_function2++;
}

static void
dummy_run_test_function (void)
{
    n_run_dummy_run_test_function++;
}

static void
dummy_setup_function (void)
{
    if (set_error_on_setup)
        cut_error("Error in setup");
    n_setup++;
}

static void
dummy_teardown_function (void)
{
    n_teardown++;
}

void
setup (void)
{
    CutTest *test;
    const gchar *test_names[] = {"/.*/", NULL};

    set_error_on_setup = FALSE;

    n_setup = 0;
    n_teardown = 0;
    n_run_dummy_run_test_function = 0;
    n_run_dummy_test_function1 = 0;
    n_run_dummy_test_function2 = 0;
    n_start_signal = 0;
    n_complete_signal = 0;

    test_context = cut_context_new();
    cut_context_set_target_test_names(test_context, test_names);

    test_object = cut_test_case_new("dummy test case",
                                    dummy_setup_function,
                                    dummy_teardown_function,
                                    get_current_test_context,
                                    set_current_test_context);
    test = cut_test_new("dummy_test_1", NULL, dummy_test_function1);
    cut_test_case_add_test(test_object, test);
    test = cut_test_new("dummy_test_2", NULL, dummy_test_function2);
    cut_test_case_add_test(test_object, test);
    test = cut_test_new("run_test_function", NULL, dummy_run_test_function);
    cut_test_case_add_test(test_object, test);
}

void
teardown (void)
{
    g_object_unref(test_object);
    g_object_unref(test_context);
}

static void
cb_start_signal (CutTestCase *test_case, gpointer data)
{
    n_start_signal++;
}

static void
cb_complete_signal (CutTestCase *test_case, gpointer data)
{
    n_complete_signal++;
}

static gboolean
run_the_test (void)
{
    return cut_test_case_run(test_object, test_context);
}

void
test_setup (void)
{
    cut_assert(run_the_test());
    cut_assert(1 < n_setup);
}

void
test_teardown (void)
{
    cut_assert(run_the_test());
    cut_assert(1 < n_teardown);
}

void
test_test_case_count (void)
{
    const gchar *test_names[] = {"/.*/", NULL};
    cut_assert_equal_int(0, cut_test_case_get_n_tests(test_object, NULL));
    cut_assert_equal_int(3, cut_test_case_get_n_tests(test_object, test_names));
}

void
test_run (void)
{
    cut_assert(run_the_test());
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
}

void
test_run_with_setup_error (void)
{
    set_error_on_setup = TRUE;
    cut_assert(!run_the_test());
    cut_assert_equal_int(0, n_run_dummy_test_function1);
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
}

void
test_run_this_function (void)
{
    cut_assert(cut_test_case_run_test(test_object, test_context,
                                      "run_test_function"));

    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(0, n_run_dummy_test_function1);
}

void
test_run_tests_with_regex (void)
{
    cut_assert(cut_test_case_run_test(test_object, test_context, "/dummy/"));
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
}

void
test_run_with_name_filter (void)
{
    const gchar *names[] = {"dummy_test_1", "run_test_function", NULL};

    cut_assert(cut_test_case_run_with_filter(test_object, test_context, names));
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(0, n_run_dummy_test_function2);
}

void
test_run_with_regex_filter (void)
{
    const gchar *regex[] = {"/dummy/", NULL};

    cut_assert(cut_test_case_run_with_filter(test_object, test_context, regex));
    cut_assert_equal_int(0, n_run_dummy_run_test_function);
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
}

void
test_run_with_name_and_regex_filter (void)
{
    const gchar *name_and_regex[] = {"/dummy/", "run_test_function", NULL};

    cut_assert(cut_test_case_run_with_filter(test_object, test_context,
                                             name_and_regex));
    cut_assert_equal_int(1, n_run_dummy_run_test_function);
    cut_assert_equal_int(1, n_run_dummy_test_function1);
    cut_assert_equal_int(1, n_run_dummy_test_function2);
}

void
test_get_name (void)
{
    cut_assert_equal_string("dummy test case",
                            cut_test_get_name(CUT_TEST(test_object)));
}

void
test_start_signal (void)
{
    g_signal_connect(test_object, "start-test", G_CALLBACK(cb_start_signal),
                     NULL);
    cut_assert(cut_test_case_run(test_object, test_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_start_signal),
                                         NULL);
    cut_assert_equal_int(3, n_start_signal);
}

void
test_complete_signal (void)
{
    g_signal_connect(test_object, "complete-test",
                     G_CALLBACK(cb_complete_signal), NULL);
    cut_assert(cut_test_case_run(test_object, test_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_complete_signal),
                                         NULL);
    cut_assert_equal_int(3, n_complete_signal);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
