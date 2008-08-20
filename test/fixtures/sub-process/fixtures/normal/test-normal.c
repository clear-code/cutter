/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cutter.h>

void test_success(void);
void test_failure(void);
void test_error(void);
void test_pending(void);
void test_notification(void);
void test_omission(void);

void data_success_iterator(void);
void test_success_iterator(const void *data);
void data_failure_iterator(void);
void test_failure_iterator(const void *data);
void data_error_iterator(void);
void test_error_iterator(const void *data);
void data_pending_iterator(void);
void test_pending_iterator(const void *data);
void data_notification_iterator(void);
void test_notification_iterator(const void *data);
void data_omission_iterator(void);
void test_omission_iterator(const void *data);

void
test_success (void)
{
    cut_assert_equal_string("xxx", "xxx");
}

void
test_failure (void)
{
    cut_fail("Failure!");
}

void
test_error (void)
{
    cut_error("Error!");
}

void
test_pending (void)
{
    cut_pend("Pending!");
}

void
test_notification (void)
{
    cut_notify("Notification!");
}

void
test_omission (void)
{
    cut_omit("Omission!");
}

void
data_success_iterator (void)
{
    cut_add_data("success data1", NULL, NULL,
                 "success data2", NULL, NULL);
}

void
test_success_iterator (const void *data)
{
    cut_assert_equal_string("abc", "abc");
}

void
data_failure_iterator (void)
{
    cut_add_data("failure data1", NULL, NULL,
                 "failure data2", NULL, NULL);
}

void
test_failure_iterator (const void *data)
{
    cut_fail("Failure! - iterator");
}

void
data_error_iterator (void)
{
    cut_add_data("error data1", NULL, NULL,
                 "error data2", NULL, NULL);
}

void
test_error_iterator (const void *data)
{
    cut_error("Error! - iterator");
}

void
data_pending_iterator (void)
{
    cut_add_data("pending data1", NULL, NULL,
                 "pending data2", NULL, NULL);
}

void
test_pending_iterator (const void *data)
{
    cut_pend("Pending! - iterator");
}

void
data_notification_iterator (void)
{
    cut_add_data("notification data1", NULL, NULL,
                 "notification data2", NULL, NULL);
}

void
test_notification_iterator (const void *data)
{
    cut_notify("Notification! - iterator");
}

void
data_omission_iterator (void)
{
    cut_add_data("omission data1", NULL, NULL,
                 "omission data2", NULL, NULL);
}

void
test_omission_iterator (const void *data)
{
    cut_omit("Omission! - iterator");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
