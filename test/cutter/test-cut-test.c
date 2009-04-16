#include <gcutter.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-runner.h>

void test_get_name(void);
void test_get_description(void);
void test_increment_assertion_count(void);
void test_run(void);
void test_start_signal(void);
void test_complete_signal(void);
void test_error_signal(void);
void test_pass_assertion_signal(void);
void test_failure_signal(void);
void test_pending_signal(void);
void test_notification_signal(void);
void test_omission_signal(void);
void test_test_function(void);
void test_set_elapsed(void);
void test_start_time(void);

static CutRunContext *run_context;
static CutTest *test;
static gboolean run_test_flag = FALSE;
static gint n_start_signal = 0;
static gint n_complete_signal = 0;
static gint n_success_signal = 0;
static gint n_failure_signal = 0;
static gint n_error_signal = 0;
static gint n_pending_signal = 0;
static gint n_pass_assertion_signal = 0;
static gint n_notification_signal = 0;
static gint n_omission_signal = 0;

static void
stub_test_function (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    run_test_flag = TRUE;
}

static void
stub_fail_function (void)
{
    cut_fail("This test should fail");
}

static void
stub_pending_function (void)
{
    cut_pend("This test has been pending ever!");
}

static void
stub_notification_function (void)
{
    cut_notify("This test has been notifying ever!");
}

static void
stub_error_function (void)
{
    cut_error("This test should error");
}

static void
stub_omission_function (void)
{
    cut_omit("This test should be omitted");
}

void
cut_setup (void)
{
    run_test_flag = FALSE;
    n_start_signal = 0;
    n_complete_signal = 0;
    n_success_signal = 0;
    n_failure_signal = 0;
    n_error_signal = 0;
    n_pending_signal = 0;
    n_notification_signal = 0;
    n_pass_assertion_signal = 0;
    n_omission_signal = 0;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());

    test = NULL;
}

void
cut_teardown (void)
{
    if (test)
        g_object_unref(test);
    g_object_unref(run_context);
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

static void
cb_failure_signal (CutTest *test, gpointer data)
{
    n_failure_signal++;
}

static void
cb_error_signal (CutTest *test, gpointer data)
{
    n_error_signal++;
}

static void
cb_pending_signal (CutTest *test, gpointer data)
{
    n_pending_signal++;
}

static void
cb_notification_signal (CutTest *test, gpointer data)
{
    n_notification_signal++;
}

static void
cb_pass_assertion_signal (CutTest *test, gpointer data)
{
    n_pass_assertion_signal++;
}

static void
cb_omission_signal (CutTest *test, gpointer data)
{
    n_omission_signal++;
}

static gboolean
run (void)
{
    gboolean success;
    CutTestContext *original_test_context;
    CutTestContext *test_context;

    test_context = cut_test_context_new(run_context, NULL, NULL, NULL, test);
    original_test_context = cut_get_current_test_context();
    cut_set_current_test_context(test_context);
    success = cut_test_run(test, test_context, run_context);
    cut_set_current_test_context(original_test_context);

    g_object_unref(test_context);

    return success;
}

void
test_test_function (void)
{
    CutTestFunction test_function;

    test = cut_test_new("stub-test", stub_test_function);
    g_object_get(test,
                 "test-function", &test_function,
                 NULL);
    cut_assert_equal_pointer(stub_test_function, test_function);
}

void
test_get_name (void)
{
    test = cut_test_new("stub-test", stub_test_function);

    cut_assert_equal_string("stub-test", cut_test_get_name(test));
    cut_test_set_name(test, "new-name");
    cut_assert_equal_string("new-name", cut_test_get_name(test));
}

void
test_get_description (void)
{
    test = cut_test_new("stub-test", stub_test_function);

    cut_assert_equal_string(NULL, cut_test_get_description(test));
    cut_test_set_attribute(test, "description", "Stub Test");
    cut_assert_equal_string("Stub Test", cut_test_get_description(test));
}

void
test_run (void)
{
    test = cut_test_new("stub-test", stub_test_function);

    cut_assert_true(run());
    cut_assert_true(run_test_flag);
}

void
test_start_signal (void)
{
    test = cut_test_new("stub-test", stub_test_function);

    g_signal_connect(test, "start", G_CALLBACK(cb_start_signal), NULL);
    cut_assert_true(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_start_signal),
                                         NULL);
    cut_assert_equal_uint(1, n_start_signal);
}

void
test_complete_signal (void)
{
    test = cut_test_new("stub-test", stub_test_function);

    g_signal_connect(test, "complete", G_CALLBACK(cb_complete_signal), NULL);
    cut_assert_true(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_complete_signal),
                                         NULL);
    cut_assert_equal_uint(1, n_complete_signal);
}

void
test_error_signal (void)
{
    test = cut_test_new("stub-error-test", stub_error_function);
    cut_assert_not_null(test);

    g_signal_connect(test, "error", G_CALLBACK(cb_error_signal), NULL);
    cut_assert_false(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_error_signal),
                                         NULL);
    cut_assert_equal_uint(1, n_error_signal);
}

void
test_failure_signal (void)
{
    test = cut_test_new("stub-failure-test", stub_fail_function);
    cut_assert_not_null(test);

    g_signal_connect(test, "failure", G_CALLBACK(cb_failure_signal), NULL);
    cut_assert_false(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_failure_signal),
                                         NULL);
    cut_assert_equal_uint(1, n_failure_signal);
}

void
test_pending_signal (void)
{
    test = cut_test_new("stub-pending-test", stub_pending_function);
    cut_assert_not_null(test);

    g_signal_connect(test, "pending", G_CALLBACK(cb_pending_signal), NULL);
    cut_assert_false(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_pending_signal),
                                         NULL);
    cut_assert_equal_uint(1, n_pending_signal);
}

void
test_notification_signal (void)
{
    test = cut_test_new("stub-notification-test", stub_notification_function);
    cut_assert_not_null(test);

    g_signal_connect(test, "notification", G_CALLBACK(cb_notification_signal),
                     NULL);
    cut_assert_true(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_notification_signal),
                                         NULL);
    cut_assert_equal_uint(1, n_notification_signal);
}

void
test_omission_signal (void)
{
    test = cut_test_new("stub-omission-test", stub_omission_function);
    cut_assert_not_null(test);

    g_signal_connect(test, "omission", G_CALLBACK(cb_omission_signal), NULL);
    cut_assert_true(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_omission_signal),
                                         NULL);
    cut_assert_equal_uint(1, n_omission_signal);
}

void
test_pass_assertion_signal (void)
{
    test = cut_test_new("stub-test", stub_test_function);

    g_signal_connect(test, "pass_assertion",
                     G_CALLBACK(cb_pass_assertion_signal), NULL);
    cut_assert_true(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_pass_assertion_signal),
                                         NULL);
    cut_assert_equal_uint(3, n_pass_assertion_signal);
}

void
test_set_elapsed (void)
{
    test = cut_test_new("stub-test", stub_test_function);

    cut_assert_equal_double(0.0, 0.1, cut_test_get_elapsed(test));

    cut_test_set_elapsed(test, 3.0);
    cut_assert_equal_double(3.0, 0.1, cut_test_get_elapsed(test));

    cut_test_set_elapsed(test, -1.0);
    cut_assert_equal_double(0.0, 0.1, cut_test_get_elapsed(test));
}

void
test_start_time (void)
{
    GTimeVal expected, actual;

    test = cut_test_new("stub-test", stub_test_function);

    expected.tv_sec = 0;
    expected.tv_usec = 0;
    cut_test_get_start_time(test, &actual);
    gcut_assert_equal_time_val(&expected, &actual);

    g_get_current_time(&expected);
    g_signal_emit_by_name(test, "start", NULL);
    cut_test_get_start_time(test, &actual);
    expected.tv_usec = actual.tv_usec; /* don't compare usec */
    gcut_assert_equal_time_val(&expected, &actual);

    expected.tv_sec = 100000;
    expected.tv_usec = 0;
    cut_test_set_start_time(test, &expected);
    cut_test_get_start_time(test, &actual);
    gcut_assert_equal_time_val(&expected, &actual);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
