#include "cutter.h"
#include "cut-test.h"
#include "cut-test-result.h"
#include "cut-context.h"

void test_equal_int(void);
void test_equal_string(void);
void test_equal_double(void);
void test_error(void);
void test_fail(void);
void test_pending(void);
void test_start_signal(void);
void test_complete_signal(void);

static gboolean need_cleanup;

static CutTest *test_object;
static CutContext *context;
static CutTestContext *test_context;
static CutTestResult *test_result;
static gint n_start_signal = 0;
static gint n_complete_signal = 0;

static void
dummy_error_test_function (void)
{
    cut_error("This test should error");
}

static void
dummy_fail_test_function (void)
{
    cut_fail("This test should fail");
}

static void
dummy_pending_test_function (void)
{
    cut_pending("This test has been pending ever!");
}

static void
dummy_normal_test_function (void)
{
    cut_assert(TRUE);
}

static void
cb_collect_result (CutTest *test, CutTestResult *result, CutTestResult **output)
{
    *output = result;
    g_object_ref(*output);
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
run(CutTest *test)
{
    gboolean success;
    CutTestContext *original_test_context;

    need_cleanup = TRUE;

    test_object = test;

    context = cut_context_new();
    cut_context_set_verbose_level(context, CUT_VERBOSE_LEVEL_SILENT);

    test_context = cut_test_context_new(NULL, NULL, test_object);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test_object, context);
    set_current_test_context(original_test_context);

    return success;
}

void
setup (void)
{
    need_cleanup = FALSE;
    test_object = NULL;
    context = NULL;
    test_context = NULL;
    test_result = NULL;
    n_start_signal = 0;
    n_complete_signal = 0;
}

void
teardown (void)
{
    if (!need_cleanup)
        return;

    if (test_object)
        g_object_unref(test_object);
    if (context)
        g_object_unref(context);
    if (test_context)
        g_object_unref(test_context);
    if (test_result)
        g_object_unref(test_result);
}

void
test_equal_int (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(-1, -1);
    cut_assert_equal_int(0, 0);
}

void
test_equal_string (void)
{
    cut_assert_equal_string("", "");
    cut_assert_equal_string("a", "a");
}

void
test_equal_double (void)
{
    cut_assert_equal_double(1.0, 0.1, 1.0);
    cut_assert_equal_double(1.0, 0.01, 1.01);
}

void
test_error (void)
{
    CutTest *test;

    test = cut_test_new("dummy-error-test", dummy_error_test_function);
    cut_assert(test, "Creating a new CutTest object failed");

    g_signal_connect(test, "error", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert(!run(test));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);

    cut_assert(test_result,
               "Could not get a CutTestResult object "
               "since \"error\" signal was not emmitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR,
                         cut_test_result_get_status(test_result));
}

void
test_pending (void)
{
    CutTest *test;

    test = cut_test_new("dummy-pending-test", dummy_pending_test_function);
    cut_assert(test, "Creating a new CutTest object failed");

    g_signal_connect(test, "pending", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert(!run(test), "cut_pending() did not return FALSE!");
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);

    cut_assert(test_result,
               "Could not get a CutTestResult object "
               "since \"pending\" signal was not emmitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_PENDING,
                         cut_test_result_get_status(test_result));
}

void
test_fail (void)
{
    CutTest *test;

    test = cut_test_new("dummy-fail-test", dummy_fail_test_function);
    cut_assert(test);

    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert(!run(test));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);
    cut_assert(test_result,
               "Could not get a CutTestResult object "
               "since \"failure\" signal was not emmitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(test_result));
}


void
test_start_signal (void)
{
    CutTest *test;

    test = cut_test_new("dummy-normal-test", dummy_normal_test_function);
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

    test = cut_test_new("dummy-normal-test", dummy_normal_test_function);
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

