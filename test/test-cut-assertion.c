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
cb_collect_result (CutTest *test, CutTestResult *result, CutTestResult **output)
{
    *output = result;
    g_object_ref(*output);
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
    CutTest *test_object;
    CutContext *test_context;
    CutTestResult *result = NULL;

    test_object = cut_test_new("dummy-error-test", dummy_error_test_function);
    cut_assert(test_object, "Creating a new CutTest object failed");

    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);

    g_signal_connect(test_object, "error",
                     G_CALLBACK(cb_collect_result), &result);
    cut_assert(cut_test_run(test_object, test_context));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_collect_result),
                                         &result);

    cut_assert(result, "Could not get a CutTestResult object since \"error\" signal was not emmitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    g_object_unref(test_object);
    g_object_unref(test_context);
}

void
test_pending (void)
{
    CutTest *test_object;
    CutContext *test_context;
    CutTestResult *result = NULL;

    test_object = cut_test_new("dummy-pending-test", dummy_pending_test_function);
    cut_assert(test_object, "Creating a new CutTest object failed");

    test_context = cut_context_new();
    cut_context_set_verbose_level(test_context, CUT_VERBOSE_LEVEL_SILENT);

    g_signal_connect(test_object, "pending",
                     G_CALLBACK(cb_collect_result), &result);
    cut_assert(!cut_test_run(test_object, test_context), "cut_pending() did not return FALSE!");
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_collect_result),
                                         &result);

    cut_assert(result, "Could not get a CutTestResult object since \"pending\" signal was not emmitted.");
    cut_assert_equal_int(CUT_TEST_RESULT_PENDING,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    g_object_unref(test_object);
    g_object_unref(test_context);
}

void
test_fail (void)
{
    CutTest *test_object;
    CutContext *context;
    CutTestContext *original_test_context, *test_context;
    gboolean success;

    test_object = cut_test_new("dummy-fail-test", dummy_fail_test_function);
    cut_assert(test_object);

    context = cut_context_new();
    cut_context_set_verbose_level(context, CUT_VERBOSE_LEVEL_SILENT);

    test_context = cut_test_context_new(NULL, NULL, test_object);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test_object, context);
    set_current_test_context(original_test_context);

    cut_assert(!success);

    g_object_unref(test_context);
    g_object_unref(context);
    g_object_unref(test_object);
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

