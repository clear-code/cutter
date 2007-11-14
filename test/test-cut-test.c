#include "cutter.h"
#include "cut-test.h"
#include "cut-context.h"

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

static CutContext *context;
static CutTest *test_object;
static gboolean run_test_flag = FALSE;
static gint n_start_signal = 0;
static gint n_complete_signal = 0;
static gint n_success_signal = 0;
static gint n_failure_signal = 0;
static gint n_error_signal = 0;
static gint n_pending_signal = 0;
static gint n_pass_assertion_signal = 0;
static gint n_notification_signal = 0;

static void
dummy_test_function (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    run_test_flag = TRUE;
}

static void
dummy_fail_function (void)
{
    cut_fail("This test should fail");
}

static void
dummy_pending_function (void)
{
    cut_pending("This test has been pending ever!");
}

static void
dummy_notification_function (void)
{
    cut_notify("This test has been notification ever!");
}

static void
dummy_error_function (void)
{
    cut_error("This test should error");
}

void
setup (void)
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

    context = cut_context_new();
    cut_context_set_verbose_level(context, CUT_VERBOSE_LEVEL_SILENT);

    test_object = cut_test_new("dummy-test", "Dummy Test", dummy_test_function);
}

void
teardown (void)
{
    g_object_unref(test_object);
    g_object_unref(context);
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

static gboolean
run (CutTest *test)
{
    gboolean success;
    CutTestContext *original_test_context;
    CutTestContext *test_context;

    test_context = cut_test_context_new(NULL, NULL, test);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, context);
    set_current_test_context(original_test_context);

    g_object_unref(test_context);

    return success;
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
    g_signal_connect(test_object, "start", G_CALLBACK(cb_start_signal), NULL);
    cut_assert(run(test_object));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_start_signal),
                                         NULL);
    cut_assert_equal_int(1, n_start_signal);
}

void
test_complete_signal (void)
{
    g_signal_connect(test_object, "complete", G_CALLBACK(cb_complete_signal), NULL);
    cut_assert(run(test_object));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_complete_signal),
                                         NULL);
    cut_assert_equal_int(1, n_complete_signal);
}

void
test_error_signal (void)
{
    CutTest *test;

    test = cut_test_new("dummy-error-test", NULL, dummy_error_function);
    cut_assert(test);

    g_signal_connect(test, "error", G_CALLBACK(cb_error_signal), NULL);
    cut_assert(!run(test));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_error_signal),
                                         NULL);
    cut_assert_equal_int(1, n_error_signal);
    g_object_unref(test);
}

void
test_failure_signal (void)
{
    CutTest *test;

    test = cut_test_new("dummy-failure-test", NULL, dummy_fail_function);
    cut_assert(test);

    g_signal_connect(test, "failure", G_CALLBACK(cb_failure_signal), NULL);
    cut_assert(!run(test));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_failure_signal),
                                         NULL);
    cut_assert_equal_int(1, n_failure_signal);
    g_object_unref(test);
}

void
test_pending_signal (void)
{
    CutTest *test;

    test = cut_test_new("dummy-pending-test", NULL, dummy_pending_function);
    cut_assert(test);

    g_signal_connect(test, "pending", G_CALLBACK(cb_pending_signal), NULL);
    cut_assert(!run(test));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_pending_signal),
                                         NULL);
    cut_assert_equal_int(1, n_pending_signal);
    g_object_unref(test);
}

void
test_notification_signal (void)
{
    CutTest *test;

    test = cut_test_new("dummy-notification-test", NULL, dummy_notification_function);
    cut_assert(test);

    g_signal_connect(test, "notification", G_CALLBACK(cb_notification_signal), NULL);
    cut_assert(run(test));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_notification_signal),
                                         NULL);
    cut_assert_equal_int(1, n_notification_signal);
    g_object_unref(test);
}

void
test_pass_assertion_signal (void)
{
    g_signal_connect(test_object, "pass_assertion", G_CALLBACK(cb_pass_assertion_signal), NULL);
    cut_assert(run(test_object));
    g_signal_handlers_disconnect_by_func(test_object,
                                         G_CALLBACK(cb_pass_assertion_signal),
                                         NULL);
    cut_assert_equal_int(3, n_pass_assertion_signal);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
