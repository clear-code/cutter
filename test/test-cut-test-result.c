#include "cutter.h"
#include "cut-test-result.h"

void test_get_status(void);
void test_get_message_full(void);
void test_get_message_only_user(void);
void test_get_message_only_system(void);
void test_get_message_none(void);
void test_set_messages(void);
void test_get_function_name(void);
void test_get_filename(void);
void test_get_line(void);
void test_status_to_signal_name(void);
void test_status_is_critical(void);

void
test_get_status (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_SUCCESS,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_ERROR,
                                 NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_ERROR,
                         cut_test_result_get_status(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_PENDING,
                                 NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(CUT_TEST_RESULT_PENDING,
                         cut_test_result_get_status(result));
    g_object_unref(result);
}

void
test_get_message_full (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("user-message\nsystem-message",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("user-message",
                            cut_test_result_get_user_message(result));
    cut_assert_equal_string("system-message",
                            cut_test_result_get_system_message(result));
    g_object_unref(result);
}

void
test_get_message_only_user (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL,
                                 "user-message",
                                 NULL,
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("user-message",
                            cut_test_result_get_message(result));
    cut_assert_equal_string("user-message",
                            cut_test_result_get_user_message(result));
    cut_assert(cut_test_result_get_system_message(result) == NULL);
    g_object_unref(result);
}

void
test_get_message_only_system (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL,
                                 NULL,
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("system-message",
                            cut_test_result_get_message(result));
    cut_assert(cut_test_result_get_user_message(result) == NULL);
    cut_assert_equal_string("system-message",
                            cut_test_result_get_system_message(result));
    g_object_unref(result);
}

void
test_get_message_none (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL,
                                 NULL,
                                 NULL,
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert(cut_test_result_get_message(result) == NULL,
               "cut_test_result_get_message() should return NULL!");
    cut_test_result_set_message(result, "message");
    cut_assert("message", cut_test_result_get_message(result));
    cut_assert(cut_test_result_get_user_message(result) == NULL,
               "cut_test_result_get_user_message() should return NULL!");
    cut_test_result_set_user_message(result, "user message");
    cut_assert("user message", cut_test_result_get_user_message(result));
    cut_assert(cut_test_result_get_system_message(result) == NULL,
               "cut_test_result_get_system_message() should return NULL!");
    cut_test_result_set_system_message(result, "system message");
    cut_assert("system message", cut_test_result_get_system_message(result));
    g_object_unref(result);
}

void
test_set_messages (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert("user-message\nsystem-message", cut_test_result_get_message(result));
    cut_test_result_set_message(result, "message");
    cut_assert("message", cut_test_result_get_message(result));
    cut_assert("user-message", cut_test_result_get_user_message(result));
    cut_test_result_set_user_message(result, "user message");
    cut_assert("user message", cut_test_result_get_user_message(result));
    cut_assert("system-message", cut_test_result_get_system_message(result));
    cut_test_result_set_system_message(result, "system message");
    cut_assert("system message", cut_test_result_get_system_message(result));
    g_object_unref(result);
}

void
test_get_function_name (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("function-name",
                            cut_test_result_get_function_name(result));
    cut_test_result_set_function_name(result, "another-function-name");
    cut_assert_equal_string("another-function-name",
                            cut_test_result_get_function_name(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 "filename",
                                 999);
    cut_assert(cut_test_result_get_function_name(result) == NULL,
               "cut_test_result_get_function_name() should return NULL!");
    g_object_unref(result);
}

void
test_get_filename (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_string("filename", cut_test_result_get_filename(result));
    cut_test_result_set_filename(result, "another-filename");
    cut_assert_equal_string("another-filename", cut_test_result_get_filename(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 999);
    cut_assert(cut_test_result_get_filename(result) == NULL,
               "cut_test_result_get_filename() should return NULL!");
    g_object_unref(result);
}

void
test_get_line (void)
{
    CutTestResult *result;

    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 NULL, NULL, NULL,
                                 "user-message",
                                 "system-message",
                                 "function-name",
                                 "filename",
                                 999);
    cut_assert_equal_int(999, cut_test_result_get_line(result));
    cut_test_result_set_line(result, 9999);
    cut_assert_equal_int(9999, cut_test_result_get_line(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 0);
    cut_assert_equal_int(0, cut_test_result_get_line(result));
    g_object_unref(result);

    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 NULL, NULL, NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 G_MAXUINT);
    cut_assert_equal_int(G_MAXUINT, cut_test_result_get_line(result));
    g_object_unref(result);
}

void
test_status_to_signal_name (void)
{
#define STATUS_TO_SIGNAL_NAME(status) \
    cut_test_result_status_to_signal_name(CUT_TEST_RESULT_ ## status)

    cut_assert_equal_string("success", STATUS_TO_SIGNAL_NAME(SUCCESS));
    cut_assert_equal_string("notification", STATUS_TO_SIGNAL_NAME(NOTIFICATION));
    cut_assert_equal_string("omission", STATUS_TO_SIGNAL_NAME(OMISSION));
    cut_assert_equal_string("pending", STATUS_TO_SIGNAL_NAME(PENDING));
    cut_assert_equal_string("failure", STATUS_TO_SIGNAL_NAME(FAILURE));
    cut_assert_equal_string("error", STATUS_TO_SIGNAL_NAME(ERROR));

#undef STATUS_IS_CRITICAL
}

void
test_status_is_critical (void)
{
#define STATUS_IS_CRITICAL(status) \
    cut_test_result_status_is_critical(CUT_TEST_RESULT_ ## status)

    cut_assert(!STATUS_IS_CRITICAL(SUCCESS));
    cut_assert(!STATUS_IS_CRITICAL(NOTIFICATION));
    cut_assert(!STATUS_IS_CRITICAL(OMISSION));
    cut_assert(STATUS_IS_CRITICAL(PENDING));
    cut_assert(STATUS_IS_CRITICAL(FAILURE));
    cut_assert(STATUS_IS_CRITICAL(ERROR));

#undef STATUS_IS_CRITICAL
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
