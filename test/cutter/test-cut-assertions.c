/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>

#include <glib/gstdio.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#else
#  include <io.h>
#endif
#include <errno.h>

#include <cutter.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-utils.h>
#include <cutter/cut-test-runner.h>
#include "../lib/cuttest-assertions.h"

void test_equal_boolean(void);
void test_not_equal_boolean(void);
void test_equal_int(void);
void test_not_equal_int(void);
#ifdef HAVE_STDINT_H
void test_equal_int_least8(void);
void test_not_equal_int_least8(void);
void test_equal_int_least16(void);
void test_not_equal_int_least16(void);
void test_equal_int_least32(void);
void test_not_equal_int_least32(void);
void test_equal_int_least64(void);
void test_not_equal_int_least64(void);
void test_equal_int_fast8(void);
void test_not_equal_int_fast8(void);
void test_equal_int_fast16(void);
void test_not_equal_int_fast16(void);
void test_equal_int_fast32(void);
void test_not_equal_int_fast32(void);
void test_equal_int_fast64(void);
void test_not_equal_int_fast64(void);
void test_equal_intptr(void);
void test_not_equal_intptr(void);
void test_equal_intmax(void);
void test_not_equal_intmax(void);
#endif
void test_equal_uint(void);
void test_not_equal_uint(void);
#ifdef HAVE_STDINT_H
void test_equal_uint_least8(void);
void test_not_equal_uint_least8(void);
void test_equal_uint_least16(void);
void test_not_equal_uint_least16(void);
void test_equal_uint_least32(void);
void test_not_equal_uint_least32(void);
void test_equal_uint_least64(void);
void test_not_equal_uint_least64(void);
void test_equal_uint_fast8(void);
void test_not_equal_uint_fast8(void);
void test_equal_uint_fast16(void);
void test_not_equal_uint_fast16(void);
void test_equal_uint_fast32(void);
void test_not_equal_uint_fast32(void);
void test_equal_uint_fast64(void);
void test_not_equal_uint_fast64(void);
void test_equal_uintptr(void);
void test_not_equal_uintptr(void);
void test_equal_uintmax(void);
void test_not_equal_uintmax(void);
#endif
void test_equal_size(void);
void test_not_equal_size(void);
void test_equal_string(void);
void test_equal_string_with_diff(void);
void test_equal_string_with_folded_diff(void);
void test_not_equal_string(void);
void test_equal_substring(void);
void test_not_equal_substring(void);
void test_equal_double(void);
void test_not_equal_double(void);
void test_operator(void);
void test_operator_int(void);
void test_operator_uint(void);
void test_operator_size(void);
void test_operator_double(void);
void test_equal_memory (void);
void test_not_equal_memory (void);
void test_equal_string_array (void);
void test_null(void);
void test_null_string(void);
void test_equal_string_with_free(void);
void test_error(void);
void test_fail(void);
void test_pending(void);
void test_notification(void);
void test_assert_message(void);
void test_assert_message_with_format_string(void);
void test_error_equal_string_with_null (void);
void test_assert_equal_function (void);
void test_failure_from_nested_function (void);
void test_assert_errno (void);
void test_omit (void);
void test_path_exist (void);
void test_path_not_exist (void);
void test_match (void);
void test_equal_pointer (void);
void test_equal_fixture_data_string (void);
void test_equal_fixture_data_string_without_file (void);
void test_error_errno (void);

static gboolean compare_function_is_called;
static gchar *tmp_file_name;

static CutTest *test;
static CutRunContext *run_context;
static CutTestContext *test_context;
static CutTestResult *test_result;

static gint fail_line;

#define MARK_FAIL(assertion) do                 \
{                                               \
    fail_line = __LINE__;                       \
    assertion;                                  \
} while (0)

#define FAIL_LOCATION (cut_take_printf("%s:%d", __FILE__, fail_line))


static void
error_equal_string_with_null (void)
{
    cut_assert_equal_string("", NULL);
}

static void
cb_collect_result (CutTest *test, CutTestContext *test_context,
                   CutTestResult *result, CutTestResult **output)
{
    *output = result;
    g_object_ref(*output);
}

static gboolean
run (void)
{
    gboolean success;

    cut_assert_not_null(test);

    run_context = cut_test_runner_new();

    test_context = cut_test_context_new(run_context, NULL, NULL, NULL, test);
    cut_test_context_set_fixture_data_dir(test_context,
                                          cuttest_get_base_dir(),
                                          "fixtures", "assertions", NULL);
    cut_test_context_current_push(test_context);
    success = cut_test_runner_run_test(CUT_TEST_RUNNER(run_context),
                                       test, test_context);
    cut_test_context_current_pop();

    return success;
}

void
cut_setup (void)
{
    compare_function_is_called = FALSE;
    tmp_file_name = NULL;
    test = NULL;
    run_context = NULL;
    test_context = NULL;
    test_result = NULL;
    fail_line = 0;
}

void
cut_teardown (void)
{
    if (tmp_file_name) {
        g_remove(tmp_file_name);
        g_free(tmp_file_name);
    }

    if (test)
        g_object_unref(test);
    if (run_context)
        g_object_unref(run_context);
    if (test_context)
        g_object_unref(test_context);
    if (test_result)
        g_object_unref(test_result);
}

static void
stub_equal_boolean (void)
{
    cut_assert_equal_boolean(CUT_TRUE, CUT_TRUE);
    cut_assert_equal_boolean(CUT_TRUE, 100);
    cut_assert_equal_boolean(CUT_FALSE, CUT_FALSE);
    MARK_FAIL(cut_assert_equal_boolean(CUT_TRUE, CUT_FALSE));
}

void
test_equal_boolean (void)
{
    test = cut_test_new("cut_assert_equal_boolean()", stub_equal_boolean);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 3, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_boolean()", NULL,
                           "<CUT_TRUE == CUT_FALSE>",
                           "true", "false",
                           FAIL_LOCATION, "stub_equal_boolean",
                           NULL);
}

static void
stub_not_equal_boolean (void)
{
    cut_assert_not_equal_boolean(CUT_FALSE, CUT_TRUE);
    cut_assert_not_equal_boolean(CUT_TRUE, CUT_FALSE);
    cut_assert_not_equal_boolean(100, CUT_FALSE);
    MARK_FAIL(cut_assert_not_equal_boolean(CUT_TRUE, 100));
}

void
test_not_equal_boolean (void)
{
    test = cut_test_new("cut_assert_not_equal_boolean()",
                        stub_not_equal_boolean);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 3, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_boolean()", NULL,
                           "<CUT_TRUE != 100>",
                           "true", "true",
                           FAIL_LOCATION, "stub_not_equal_boolean",
                           NULL);
}

static void
stub_equal_int (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(-1, -1);
    MARK_FAIL(cut_assert_equal_int(2 + 3, 3 + 4));
}

void
test_equal_int (void)
{
    test = cut_test_new("cut_assert_equal_int()", stub_equal_int);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_int()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_int",
                           NULL);
}

static void
stub_not_equal_int (void)
{
    cut_assert_not_equal_int(1, 2);
    cut_assert_not_equal_int(1, -1);
    MARK_FAIL(cut_assert_not_equal_int(2 + 3, 3 + 2));
}

void
test_not_equal_int (void)
{
    test = cut_test_new("cut_assert_not_equal_int()", stub_not_equal_int);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_int()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_int",
                           NULL);
}

#ifdef HAVE_STDINT_H
static void
stub_equal_int_least8 (void)
{
    cut_assert_equal_int_least8(1, 1);
    cut_assert_equal_int_least8(-1, -1);
    MARK_FAIL(cut_assert_equal_int_least8(2 + 3, 3 + 4));
}

void
test_equal_int_least8 (void)
{
    test = cut_test_new("cut_assert_equal_int_least8()", stub_equal_int_least8);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_int_least8()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_int_least8",
                           NULL);
}

static void
stub_not_equal_int_least8 (void)
{
    cut_assert_not_equal_int_least8(1, 2);
    cut_assert_not_equal_int_least8(1, -1);
    MARK_FAIL(cut_assert_not_equal_int_least8(2 + 3, 3 + 2));
}

void
test_not_equal_int_least8 (void)
{
    test = cut_test_new("cut_assert_not_equal_int_least8()", stub_not_equal_int_least8);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_int_least8()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_int_least8",
                           NULL);
}

static void
stub_equal_int_least16 (void)
{
    cut_assert_equal_int_least16(1, 1);
    cut_assert_equal_int_least16(-1, -1);
    MARK_FAIL(cut_assert_equal_int_least16(2 + 3, 3 + 4));
}

void
test_equal_int_least16 (void)
{
    test = cut_test_new("cut_assert_equal_int_least16()", stub_equal_int_least16);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_int_least16()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_int_least16",
                           NULL);
}

static void
stub_not_equal_int_least16 (void)
{
    cut_assert_not_equal_int_least16(1, 2);
    cut_assert_not_equal_int_least16(1, -1);
    MARK_FAIL(cut_assert_not_equal_int_least16(2 + 3, 3 + 2));
}

void
test_not_equal_int_least16 (void)
{
    test = cut_test_new("cut_assert_not_equal_int_least16()", stub_not_equal_int_least16);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_int_least16()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_int_least16",
                           NULL);
}

static void
stub_equal_int_least32 (void)
{
    cut_assert_equal_int_least32(1, 1);
    cut_assert_equal_int_least32(-1, -1);
    MARK_FAIL(cut_assert_equal_int_least32(2 + 3, 3 + 4));
}

void
test_equal_int_least32 (void)
{
    test = cut_test_new("cut_assert_equal_int_least32()", stub_equal_int_least32);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_int_least32()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_int_least32",
                           NULL);
}

static void
stub_not_equal_int_least32 (void)
{
    cut_assert_not_equal_int_least32(1, 2);
    cut_assert_not_equal_int_least32(1, -1);
    MARK_FAIL(cut_assert_not_equal_int_least32(2 + 3, 3 + 2));
}

void
test_not_equal_int_least32 (void)
{
    test = cut_test_new("cut_assert_not_equal_int_least32()", stub_not_equal_int_least32);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_int_least32()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_int_least32",
                           NULL);
}

static void
stub_equal_int_least64 (void)
{
    cut_assert_equal_int_least64(1, 1);
    cut_assert_equal_int_least64(-1, -1);
    MARK_FAIL(cut_assert_equal_int_least64(2 + 3, 3 + 4));
}

void
test_equal_int_least64 (void)
{
    test = cut_test_new("cut_assert_equal_int_least64()", stub_equal_int_least64);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_int_least64()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_int_least64",
                           NULL);
}

static void
stub_not_equal_int_least64 (void)
{
    cut_assert_not_equal_int_least64(1, 2);
    cut_assert_not_equal_int_least64(1, -1);
    MARK_FAIL(cut_assert_not_equal_int_least64(2 + 3, 3 + 2));
}

void
test_not_equal_int_least64 (void)
{
    test = cut_test_new("cut_assert_not_equal_int_least64()", stub_not_equal_int_least64);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_int_least64()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_int_least64",
                           NULL);
}

static void
stub_equal_int_fast8 (void)
{
    cut_assert_equal_int_fast8(1, 1);
    cut_assert_equal_int_fast8(-1, -1);
    MARK_FAIL(cut_assert_equal_int_fast8(2 + 3, 3 + 4));
}

void
test_equal_int_fast8 (void)
{
    test = cut_test_new("cut_assert_equal_int_fast8()", stub_equal_int_fast8);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_int_fast8()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_int_fast8",
                           NULL);
}

static void
stub_not_equal_int_fast8 (void)
{
    cut_assert_not_equal_int_fast8(1, 2);
    cut_assert_not_equal_int_fast8(1, -1);
    MARK_FAIL(cut_assert_not_equal_int_fast8(2 + 3, 3 + 2));
}

void
test_not_equal_int_fast8 (void)
{
    test = cut_test_new("cut_assert_not_equal_int_fast8()", stub_not_equal_int_fast8);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_int_fast8()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_int_fast8",
                           NULL);
}

static void
stub_equal_int_fast16 (void)
{
    cut_assert_equal_int_fast16(1, 1);
    cut_assert_equal_int_fast16(-1, -1);
    MARK_FAIL(cut_assert_equal_int_fast16(2 + 3, 3 + 4));
}

void
test_equal_int_fast16 (void)
{
    test = cut_test_new("cut_assert_equal_int_fast16()", stub_equal_int_fast16);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_int_fast16()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_int_fast16",
                           NULL);
}

static void
stub_not_equal_int_fast16 (void)
{
    cut_assert_not_equal_int_fast16(1, 2);
    cut_assert_not_equal_int_fast16(1, -1);
    MARK_FAIL(cut_assert_not_equal_int_fast16(2 + 3, 3 + 2));
}

void
test_not_equal_int_fast16 (void)
{
    test = cut_test_new("cut_assert_not_equal_int_fast16()", stub_not_equal_int_fast16);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_int_fast16()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_int_fast16",
                           NULL);
}

static void
stub_equal_int_fast32 (void)
{
    cut_assert_equal_int_fast32(1, 1);
    cut_assert_equal_int_fast32(-1, -1);
    MARK_FAIL(cut_assert_equal_int_fast32(2 + 3, 3 + 4));
}

void
test_equal_int_fast32 (void)
{
    test = cut_test_new("cut_assert_equal_int_fast32()", stub_equal_int_fast32);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_int_fast32()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_int_fast32",
                           NULL);
}

static void
stub_not_equal_int_fast32 (void)
{
    cut_assert_not_equal_int_fast32(1, 2);
    cut_assert_not_equal_int_fast32(1, -1);
    MARK_FAIL(cut_assert_not_equal_int_fast32(2 + 3, 3 + 2));
}

void
test_not_equal_int_fast32 (void)
{
    test = cut_test_new("cut_assert_not_equal_int_fast32()", stub_not_equal_int_fast32);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_int_fast32()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_int_fast32",
                           NULL);
}

static void
stub_equal_int_fast64 (void)
{
    cut_assert_equal_int_fast64(1, 1);
    cut_assert_equal_int_fast64(-1, -1);
    MARK_FAIL(cut_assert_equal_int_fast64(2 + 3, 3 + 4));
}

void
test_equal_int_fast64 (void)
{
    test = cut_test_new("cut_assert_equal_int_fast64()", stub_equal_int_fast64);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_int_fast64()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_int_fast64",
                           NULL);
}

static void
stub_not_equal_int_fast64 (void)
{
    cut_assert_not_equal_int_fast64(1, 2);
    cut_assert_not_equal_int_fast64(1, -1);
    MARK_FAIL(cut_assert_not_equal_int_fast64(2 + 3, 3 + 2));
}

void
test_not_equal_int_fast64 (void)
{
    test = cut_test_new("cut_assert_not_equal_int_fast64()", stub_not_equal_int_fast64);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_int_fast64()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_int_fast64",
                           NULL);
}

static void
stub_equal_intptr (void)
{
    cut_assert_equal_intptr(1, 1);
    cut_assert_equal_intptr(-1, -1);
    MARK_FAIL(cut_assert_equal_intptr(2 + 3, 3 + 4));
}

void
test_equal_intptr (void)
{
    test = cut_test_new("cut_assert_equal_intptr()", stub_equal_intptr);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_intptr()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_intptr",
                           NULL);
}

static void
stub_not_equal_intptr (void)
{
    cut_assert_not_equal_intptr(1, 2);
    cut_assert_not_equal_intptr(1, -1);
    MARK_FAIL(cut_assert_not_equal_intptr(2 + 3, 3 + 2));
}

void
test_not_equal_intptr (void)
{
    test = cut_test_new("cut_assert_not_equal_intptr()", stub_not_equal_intptr);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_intptr()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_intptr",
                           NULL);
}

static void
stub_equal_intmax (void)
{
    cut_assert_equal_intmax(1, 1);
    cut_assert_equal_intmax(-1, -1);
    MARK_FAIL(cut_assert_equal_intmax(2 + 3, 3 + 4));
}

void
test_equal_intmax (void)
{
    test = cut_test_new("cut_assert_equal_intmax()", stub_equal_intmax);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_intmax()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_intmax",
                           NULL);
}

static void
stub_not_equal_intmax (void)
{
    cut_assert_not_equal_intmax(1, 2);
    cut_assert_not_equal_intmax(1, -1);
    MARK_FAIL(cut_assert_not_equal_intmax(2 + 3, 3 + 2));
}

void
test_not_equal_intmax (void)
{
    test = cut_test_new("cut_assert_not_equal_intmax()", stub_not_equal_intmax);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_intmax()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_intmax",
                           NULL);
}
#endif

static void
stub_equal_uint (void)
{
    cut_assert_equal_uint(1, 1);
    MARK_FAIL(cut_assert_equal_uint(2 + 3, 3 + 4));
}

void
test_equal_uint (void)
{
    test = cut_test_new("cut_assert_equal_uint()", stub_equal_uint);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_uint()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_uint",
                           NULL);
}

static void
stub_not_equal_uint (void)
{
    cut_assert_not_equal_uint(1, 2);
    MARK_FAIL(cut_assert_not_equal_uint(2 + 3, 3 + 2));
}

void
test_not_equal_uint (void)
{
    test = cut_test_new("cut_assert_not_equal_uint()", stub_not_equal_uint);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_uint()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_uint",
                           NULL);
}

#ifdef HAVE_STDINT_H
static void
stub_equal_uint_least8 (void)
{
    cut_assert_equal_uint_least8(1, 1);
    cut_assert_equal_uint_least8(-1, -1);
    MARK_FAIL(cut_assert_equal_uint_least8(2 + 3, 3 + 4));
}

void
test_equal_uint_least8 (void)
{
    test = cut_test_new("cut_assert_equal_uint_least8()", stub_equal_uint_least8);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_uint_least8()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_uint_least8",
                           NULL);
}

static void
stub_not_equal_uint_least8 (void)
{
    cut_assert_not_equal_uint_least8(1, 2);
    cut_assert_not_equal_uint_least8(1, -1);
    MARK_FAIL(cut_assert_not_equal_uint_least8(2 + 3, 3 + 2));
}

void
test_not_equal_uint_least8 (void)
{
    test = cut_test_new("cut_assert_not_equal_uint_least8()", stub_not_equal_uint_least8);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_uint_least8()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_uint_least8",
                           NULL);
}

static void
stub_equal_uint_least16 (void)
{
    cut_assert_equal_uint_least16(1, 1);
    cut_assert_equal_uint_least16(-1, -1);
    MARK_FAIL(cut_assert_equal_uint_least16(2 + 3, 3 + 4));
}

void
test_equal_uint_least16 (void)
{
    test = cut_test_new("cut_assert_equal_uint_least16()", stub_equal_uint_least16);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_uint_least16()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_uint_least16",
                           NULL);
}

static void
stub_not_equal_uint_least16 (void)
{
    cut_assert_not_equal_uint_least16(1, 2);
    cut_assert_not_equal_uint_least16(1, -1);
    MARK_FAIL(cut_assert_not_equal_uint_least16(2 + 3, 3 + 2));
}

void
test_not_equal_uint_least16 (void)
{
    test = cut_test_new("cut_assert_not_equal_uint_least16()", stub_not_equal_uint_least16);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_uint_least16()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_uint_least16",
                           NULL);
}

static void
stub_equal_uint_least32 (void)
{
    cut_assert_equal_uint_least32(1, 1);
    cut_assert_equal_uint_least32(-1, -1);
    MARK_FAIL(cut_assert_equal_uint_least32(2 + 3, 3 + 4));
}

void
test_equal_uint_least32 (void)
{
    test = cut_test_new("cut_assert_equal_uint_least32()", stub_equal_uint_least32);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_uint_least32()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_uint_least32",
                           NULL);
}

static void
stub_not_equal_uint_least32 (void)
{
    cut_assert_not_equal_uint_least32(1, 2);
    cut_assert_not_equal_uint_least32(1, -1);
    MARK_FAIL(cut_assert_not_equal_uint_least32(2 + 3, 3 + 2));
}

void
test_not_equal_uint_least32 (void)
{
    test = cut_test_new("cut_assert_not_equal_uint_least32()", stub_not_equal_uint_least32);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_uint_least32()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_uint_least32",
                           NULL);
}

static void
stub_equal_uint_least64 (void)
{
    cut_assert_equal_uint_least64(1, 1);
    cut_assert_equal_uint_least64(-1, -1);
    MARK_FAIL(cut_assert_equal_uint_least64(2 + 3, 3 + 4));
}

void
test_equal_uint_least64 (void)
{
    test = cut_test_new("cut_assert_equal_uint_least64()", stub_equal_uint_least64);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_uint_least64()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_uint_least64",
                           NULL);
}

static void
stub_not_equal_uint_least64 (void)
{
    cut_assert_not_equal_uint_least64(1, 2);
    cut_assert_not_equal_uint_least64(1, -1);
    MARK_FAIL(cut_assert_not_equal_uint_least64(2 + 3, 3 + 2));
}

void
test_not_equal_uint_least64 (void)
{
    test = cut_test_new("cut_assert_not_equal_uint_least64()", stub_not_equal_uint_least64);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_uint_least64()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_uint_least64",
                           NULL);
}

static void
stub_equal_uint_fast8 (void)
{
    cut_assert_equal_uint_fast8(1, 1);
    cut_assert_equal_uint_fast8(-1, -1);
    MARK_FAIL(cut_assert_equal_uint_fast8(2 + 3, 3 + 4));
}

void
test_equal_uint_fast8 (void)
{
    test = cut_test_new("cut_assert_equal_uint_fast8()", stub_equal_uint_fast8);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_uint_fast8()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_uint_fast8",
                           NULL);
}

static void
stub_not_equal_uint_fast8 (void)
{
    cut_assert_not_equal_uint_fast8(1, 2);
    cut_assert_not_equal_uint_fast8(1, -1);
    MARK_FAIL(cut_assert_not_equal_uint_fast8(2 + 3, 3 + 2));
}

void
test_not_equal_uint_fast8 (void)
{
    test = cut_test_new("cut_assert_not_equal_uint_fast8()", stub_not_equal_uint_fast8);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_uint_fast8()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_uint_fast8",
                           NULL);
}

static void
stub_equal_uint_fast16 (void)
{
    cut_assert_equal_uint_fast16(1, 1);
    cut_assert_equal_uint_fast16(-1, -1);
    MARK_FAIL(cut_assert_equal_uint_fast16(2 + 3, 3 + 4));
}

void
test_equal_uint_fast16 (void)
{
    test = cut_test_new("cut_assert_equal_uint_fast16()", stub_equal_uint_fast16);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_uint_fast16()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_uint_fast16",
                           NULL);
}

static void
stub_not_equal_uint_fast16 (void)
{
    cut_assert_not_equal_uint_fast16(1, 2);
    cut_assert_not_equal_uint_fast16(1, -1);
    MARK_FAIL(cut_assert_not_equal_uint_fast16(2 + 3, 3 + 2));
}

void
test_not_equal_uint_fast16 (void)
{
    test = cut_test_new("cut_assert_not_equal_uint_fast16()", stub_not_equal_uint_fast16);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_uint_fast16()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_uint_fast16",
                           NULL);
}

static void
stub_equal_uint_fast32 (void)
{
    cut_assert_equal_uint_fast32(1, 1);
    cut_assert_equal_uint_fast32(-1, -1);
    MARK_FAIL(cut_assert_equal_uint_fast32(2 + 3, 3 + 4));
}

void
test_equal_uint_fast32 (void)
{
    test = cut_test_new("cut_assert_equal_uint_fast32()", stub_equal_uint_fast32);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_uint_fast32()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_uint_fast32",
                           NULL);
}

static void
stub_not_equal_uint_fast32 (void)
{
    cut_assert_not_equal_uint_fast32(1, 2);
    cut_assert_not_equal_uint_fast32(1, -1);
    MARK_FAIL(cut_assert_not_equal_uint_fast32(2 + 3, 3 + 2));
}

void
test_not_equal_uint_fast32 (void)
{
    test = cut_test_new("cut_assert_not_equal_uint_fast32()", stub_not_equal_uint_fast32);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_uint_fast32()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_uint_fast32",
                           NULL);
}

static void
stub_equal_uint_fast64 (void)
{
    cut_assert_equal_uint_fast64(1, 1);
    cut_assert_equal_uint_fast64(-1, -1);
    MARK_FAIL(cut_assert_equal_uint_fast64(2 + 3, 3 + 4));
}

void
test_equal_uint_fast64 (void)
{
    test = cut_test_new("cut_assert_equal_uint_fast64()", stub_equal_uint_fast64);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_uint_fast64()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_uint_fast64",
                           NULL);
}

static void
stub_not_equal_uint_fast64 (void)
{
    cut_assert_not_equal_uint_fast64(1, 2);
    cut_assert_not_equal_uint_fast64(1, -1);
    MARK_FAIL(cut_assert_not_equal_uint_fast64(2 + 3, 3 + 2));
}

void
test_not_equal_uint_fast64 (void)
{
    test = cut_test_new("cut_assert_not_equal_uint_fast64()", stub_not_equal_uint_fast64);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_uint_fast64()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_uint_fast64",
                           NULL);
}

static void
stub_equal_uintptr (void)
{
    cut_assert_equal_uintptr(1, 1);
    cut_assert_equal_uintptr(-1, -1);
    MARK_FAIL(cut_assert_equal_uintptr(2 + 3, 3 + 4));
}

void
test_equal_uintptr (void)
{
    test = cut_test_new("cut_assert_equal_uintptr()", stub_equal_uintptr);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_uintptr()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_uintptr",
                           NULL);
}

static void
stub_not_equal_uintptr (void)
{
    cut_assert_not_equal_uintptr(1, 2);
    cut_assert_not_equal_uintptr(1, -1);
    MARK_FAIL(cut_assert_not_equal_uintptr(2 + 3, 3 + 2));
}

void
test_not_equal_uintptr (void)
{
    test = cut_test_new("cut_assert_not_equal_uintptr()", stub_not_equal_uintptr);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_uintptr()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_uintptr",
                           NULL);
}

static void
stub_equal_uintmax (void)
{
    cut_assert_equal_uintmax(1, 1);
    cut_assert_equal_uintmax(-1, -1);
    MARK_FAIL(cut_assert_equal_uintmax(2 + 3, 3 + 4));
}

void
test_equal_uintmax (void)
{
    test = cut_test_new("cut_assert_equal_uintmax()", stub_equal_uintmax);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_uintmax()", NULL,
                           "<2 + 3 == 3 + 4>",
                           "5", "7",
                           FAIL_LOCATION, "stub_equal_uintmax",
                           NULL);
}

static void
stub_not_equal_uintmax (void)
{
    cut_assert_not_equal_uintmax(1, 2);
    cut_assert_not_equal_uintmax(1, -1);
    MARK_FAIL(cut_assert_not_equal_uintmax(2 + 3, 3 + 2));
}

void
test_not_equal_uintmax (void)
{
    test = cut_test_new("cut_assert_not_equal_uintmax()", stub_not_equal_uintmax);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_uintmax()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_uintmax",
                           NULL);
}
#endif

static void
stub_equal_size (void)
{
    cut_assert_equal_size(1, 1);
    MARK_FAIL(cut_assert_equal_size(1, 10));
}

void
test_equal_size (void)
{
    test = cut_test_new("cut_assert_equal_size()", stub_equal_size);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_size()", NULL,
                           "<1 == 10>",
                           "1", "10",
                           FAIL_LOCATION, "stub_equal_size",
                           NULL);
}

static void
stub_not_equal_size (void)
{
    cut_assert_not_equal_size(1, 10);
    MARK_FAIL(cut_assert_not_equal_size(2 + 3, 3 + 2));
}

void
test_not_equal_size (void)
{
    test = cut_test_new("cut_assert_not_equal_size()", stub_not_equal_size);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_size()", NULL,
                           "<2 + 3 != 3 + 2>",
                           "5", "5",
                           FAIL_LOCATION, "stub_not_equal_size",
                           NULL);
}

static void
stub_equal_string (void)
{
    cut_assert_equal_string("abc", "abc");
    cut_assert_equal_string(NULL, NULL);
    MARK_FAIL(cut_assert_equal_string("abc", "ABC"));
}

void
test_equal_string (void)
{
    test = cut_test_new("assert-equal-string", stub_equal_string);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "assert-equal-string", NULL,
                           "<\"abc\" == \"ABC\">",
                           "abc", "ABC",
                           FAIL_LOCATION, "stub_equal_string",
                           NULL);
}

static void
stub_not_equal_string (void)
{
    cut_assert_not_equal_string("abc", "ABC");
    cut_assert_not_equal_string("abc", NULL);
    cut_assert_not_equal_string(NULL, "abc");
    MARK_FAIL(cut_assert_not_equal_string("abc", "abc"));
}

void
test_not_equal_string (void)
{
    test = cut_test_new("assert-not-equal-string", stub_not_equal_string);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 3, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "assert-not-equal-string", NULL,
                           "<\"abc\" != \"abc\">",
                           "abc", "abc",
                           FAIL_LOCATION, "stub_not_equal_string",
                           NULL);
}

static void
equal_string_with_diff (void)
{
    cut_assert_equal_string("abc def ghi jkl",
                            "abc DEF ghi jkl");
}

void
test_equal_string_with_diff (void)
{
    test = cut_test_new("assert-equal-string-with-diff",
                        equal_string_with_diff);
    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);
    cut_assert_equal_string("<\"abc def ghi jkl\" == \"abc DEF ghi jkl\">",
                            cut_test_result_get_system_message(test_result));
    cut_assert_equal_string("<\"abc def ghi jkl\" == \"abc DEF ghi jkl\">\n"
                            "expected: <abc def ghi jkl>\n"
                            "  actual: <abc DEF ghi jkl>\n"
                            "\n"
                            "diff:\n"
                            "- abc def ghi jkl\n"
                            "?     ^^^\n"
                            "+ abc DEF ghi jkl\n"
                            "?     ^^^",
                            cut_test_result_get_message(test_result));
}

static void
equal_string_with_folded_diff (void)
{
    cut_assert_equal_string("0123456789"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789",

                            "0000000000"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789");
}

void
test_equal_string_with_folded_diff (void)
{
    test = cut_test_new("assert-equal-string-with-folded-diff",
                        equal_string_with_folded_diff);
    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);
    cut_assert_equal_string("<"
                            "\"0123456789\" "
                            "\"1123456789\" "
                            "\"2123456789\" "
                            "\"3123456789\" "
                            "\"4123456789\" "
                            "\"5123456789\" "
                            "\"6123456789\" "
                            "\"7123456789\" "
                            "\"8123456789\""
                            " == "
                            "\"0000000000\" "
                            "\"1123456789\" "
                            "\"2123456789\" "
                            "\"3123456789\" "
                            "\"4123456789\" "
                            "\"5123456789\" "
                            "\"6123456789\" "
                            "\"7123456789\" "
                            "\"8123456789\""
                            ">",
                            cut_test_result_get_system_message(test_result));

    cut_assert_equal_string("<"
                            "\"0123456789\" "
                            "\"1123456789\" "
                            "\"2123456789\" "
                            "\"3123456789\" "
                            "\"4123456789\" "
                            "\"5123456789\" "
                            "\"6123456789\" "
                            "\"7123456789\" "
                            "\"8123456789\""
                            " == "
                            "\"0000000000\" "
                            "\"1123456789\" "
                            "\"2123456789\" "
                            "\"3123456789\" "
                            "\"4123456789\" "
                            "\"5123456789\" "
                            "\"6123456789\" "
                            "\"7123456789\" "
                            "\"8123456789\""
                            ">\n"
                            "expected: <"
                            "0123456789"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789"
                            ">\n"
                            "  actual: <"
                            "0000000000"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789"
                            ">\n"
                            "\n"
                            "diff:\n"
                            "- 0123456789"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789\n"
                            "?  ^^^^^^^^^\n"
                            "+ 0000000000"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "7123456789"
                            "8123456789\n"
                            "?  ^^^^^^^^^\n"
                            "\n"
                            "folded diff:\n"
                            "- 0123456789"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "71234567\n"
                            "?  ^^^^^^^^^\n"
                            "+ 0000000000"
                            "1123456789"
                            "2123456789"
                            "3123456789"
                            "4123456789"
                            "5123456789"
                            "6123456789"
                            "71234567\n"
                            "?  ^^^^^^^^^\n"
                            "  89"
                            "8123456789",
                            cut_test_result_get_message(test_result));
}

static void
stub_equal_substring (void)
{
    const gchar actual_string[] =
        "0000"
        "0123456789"
        "999999";
    cut_assert_equal_substring(NULL, NULL, 0);
    cut_assert_equal_substring("0123456789",
                               actual_string + strlen("0000"),
                               strlen("0123456789"));
    MARK_FAIL(cut_assert_equal_substring("0123456789",
                                         actual_string + strlen("0000") + 1,
                                         strlen("0123456789")));
}

void
test_equal_substring (void)
{
    test = cut_test_new("equal-substring", stub_equal_substring);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal-substring", NULL,
                           "<\"0123456789\" == "
                           "(actual_string + strlen(\"0000\") + 1)"
                           "[0..strlen(\"0123456789\")]>",
                           "0123456789",
                           "1234567899",
                           FAIL_LOCATION, "stub_equal_substring",
                           NULL);
}

static void
stub_not_equal_substring (void)
{
    const gchar actual_string[] =
        "0000"
        "0123456789"
        "999999";

    cut_assert_not_equal_substring("abc", NULL, 3);
    cut_assert_not_equal_substring(NULL, "abc", 3);
    cut_assert_not_equal_substring("0123456789",
                               actual_string + strlen("0000") + 1,
                               strlen("0123456789"));
    MARK_FAIL(cut_assert_not_equal_substring("0123456789",
                                             actual_string + strlen("0000"),
                                             strlen("0123456789")));
}

void
test_not_equal_substring (void)
{
    test = cut_test_new("not-equal-substring", stub_not_equal_substring);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 3, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "not-equal-substring", NULL,
                           "<\"0123456789\" != "
                           "(actual_string + strlen(\"0000\"))"
                           "[0..strlen(\"0123456789\")]>",
                           "0123456789",
                           "0123456789",
                           FAIL_LOCATION, "stub_not_equal_substring",
                           NULL);
}

static void
stub_equal_double (void)
{
    cut_assert_equal_double(1.0, 0.1, 1.0);
    cut_assert_equal_double(1.01, 0.01, 1.01);
    MARK_FAIL(cut_assert_equal_double(1.02, 0.01, 1.04));
}

void
test_equal_double (void)
{
    gdouble expected = 1.02;
    gdouble actual = 1.04;
    gdouble error = 0.01;

    test = cut_test_new("cut_assert_equal_double()", stub_equal_double);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(
        run_context, 0, CUT_TEST_RESULT_FAILURE,
        "cut_assert_equal_double()", NULL,
        cut_take_printf("<1.02-0.01 <= 1.04 <= 1.02+0.01>\n"
                        "expected: <%g>\n"
                        "   error: <%g>\n"
                        "     min: <%g>\n"
                        "     max: <%g>\n"
                        "  actual: <%g>\n"
                        "relation: <min < max < actual>",
                        expected,
                        error,
                        expected - error,
                        expected + error,
                        actual),
        NULL, NULL,
        FAIL_LOCATION, "stub_equal_double",
        NULL);
}

static void
stub_not_equal_double (void)
{
    cut_assert_not_equal_double(1.0, 0.1, 1.2);
    MARK_FAIL(cut_assert_not_equal_double(1.01, 0.1, 1.02));
}

void
test_not_equal_double (void)
{
    gdouble expected = 1.01;
    gdouble actual = 1.02;
    gdouble error = 0.1;

    test = cut_test_new("cut_assert_not_equal_double()", stub_not_equal_double);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(
        run_context, 0, CUT_TEST_RESULT_FAILURE,
        "cut_assert_not_equal_double()", NULL,
        cut_take_printf("<(1.02 < 1.01-0.1) && (1.01+0.1 < 1.02)>\n"
                        "expected: <%g>\n"
                        "   error: <%g>\n"
                        "     min: <%g>\n"
                        "     max: <%g>\n"
                        "  actual: <%g>\n"
                        "relation: <min < actual < max>",
                        expected,
                        error,
                        expected - error,
                        expected + error,
                        actual),
        NULL, NULL,
        FAIL_LOCATION, "stub_not_equal_double",
        NULL);
}

void
test_operator (void)
{
    cut_assert_operator(1, <, 2);
    cut_assert_operator(1, <=, 1);
    cut_assert_operator(1, >=, 1);
    cut_assert_operator(2, >, 1);
    cut_assert_operator(NULL, ==, NULL);
    cut_assert_operator(TRUE, ||, FALSE);
    cut_assert_operator(TRUE, &&, TRUE);
}

static void
stub_operator_int (void)
{
    cut_assert_operator_int(1, <, 2 + 3);
    cut_assert_operator_int(2, ==, 1 + 1);
    MARK_FAIL(cut_assert_operator_int(1 + 1, >=, 2 + 4));
}

void
test_operator_int (void)
{
    test = cut_test_new("stub-operator-int", stub_operator_int);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "stub-operator-int", NULL,
                           "expected: <1 + 1> >= <2 + 4>\n"
                           "  actual: <2> >= <6>",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_operator_int",
                           NULL);
}

static void
stub_operator_uint (void)
{
    cut_assert_operator_uint(1, <, 2 + 3);
    cut_assert_operator_uint(2, ==, 1 + 1);
    MARK_FAIL(cut_assert_operator_uint(1 + 1, >=, 2 + 4));
}

void
test_operator_uint (void)
{
    test = cut_test_new("stub-operator-uint", stub_operator_uint);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "stub-operator-uint", NULL,
                           "expected: <1 + 1> >= <2 + 4>\n"
                           "  actual: <2> >= <6>",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_operator_uint",
                           NULL);
}

static void
stub_operator_size (void)
{
    cut_assert_operator_size(1, <, 2 + 3);
    cut_assert_operator_size(2, ==, 1 + 1);
    MARK_FAIL(cut_assert_operator_size(1 + 1, >=, 2 + 4));
}

void
test_operator_size (void)
{
    test = cut_test_new("cut_assert_operator_size()", stub_operator_size);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_operator_size()", NULL,
                           "expected: <1 + 1> >= <2 + 4>\n"
                           "  actual: <2> >= <6>",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_operator_size",
                           NULL);
}

static void
stub_operator_double (void)
{
    cut_assert_operator_double(1.2, <, 2.2 + 3.4);
    MARK_FAIL(cut_assert_operator_double(1.1 + 1.1, >=, 2.2 + 4.4));
}

void
test_operator_double (void)
{
    gdouble lhs, rhs;

    lhs = 1.1 + 1.1;
    rhs = 2.2 + 4.4;

    test = cut_test_new("stub-operator-double", stub_operator_double);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(
        run_context, 0, CUT_TEST_RESULT_FAILURE,
        "stub-operator-double", NULL,
        cut_take_printf("expected: <1.1 + 1.1> >= <2.2 + 4.4>\n"
                        "  actual: <%g> >= <%g>",
                        lhs, rhs),
        NULL, NULL,
        FAIL_LOCATION, "stub_operator_double",
        NULL);
}

static void
stub_equal_memory (void)
{
    gchar expected[] = {0x00, 0x01, 0x02, 0x03, 0x04};
    gchar actual[] = {0x00, 0x01, 0x02, 0x03, 0x04,
                      0x12, 0x10, 0x0e, 0x0c, 0x0a};

    cut_assert_equal_memory(expected, sizeof(expected),
                            expected, sizeof(expected));
    cut_assert_equal_memory(actual, sizeof(actual),
                            actual, sizeof(actual));
    cut_assert_equal_memory(expected, sizeof(expected),
                            actual, sizeof(expected));
    MARK_FAIL(cut_assert_equal_memory(expected, sizeof(expected),
                                      actual, sizeof(actual)));
}

void
test_equal_memory (void)
{
    test = cut_test_new("stub-equal-memory", stub_equal_memory);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 3, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "stub-equal-memory", NULL,
                           "<expected(size: sizeof(expected)) == "
                           "actual(size: sizeof(actual))>",
                           "0x00 0x01 0x02 0x03 0x04 (size: 5)",
                           "0x00 0x01 0x02 0x03 0x04 "
                           "0x12 0x10 0x0e 0x0c 0x0a (size: 10)",
                           FAIL_LOCATION, "stub_equal_memory",
                           NULL);
}

static void
stub_not_equal_memory (void)
{
    gchar expected[] = {0x00, 0x01, 0x02, 0x03, 0x04};
    gchar actual[] = {0x00, 0x01, 0x02, 0x03, 0x04,
                      0x12, 0x10, 0x0e, 0x0c, 0x0a};

    cut_assert_not_equal_memory(expected, sizeof(expected),
                                actual, sizeof(actual));
    cut_assert_not_equal_memory(NULL, 0,
                                actual, sizeof(actual));
    cut_assert_not_equal_memory(expected, sizeof(expected),
                                NULL, 0);
    MARK_FAIL(cut_assert_not_equal_memory(expected, sizeof(expected),
                                          actual, sizeof(expected)));
}

void
test_not_equal_memory (void)
{
    test = cut_test_new("stub-not-equal-memory", stub_not_equal_memory);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 3, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "stub-not-equal-memory", NULL,
                           "<expected(size: sizeof(expected)) != "
                           "actual(size: sizeof(expected))>",
                           "0x00 0x01 0x02 0x03 0x04 (size: 5)",
                           "0x00 0x01 0x02 0x03 0x04 (size: 5)",
                           FAIL_LOCATION, "stub_not_equal_memory",
                           NULL);
}

static void
stub_equal_string_array (void)
{
    gchar *strings1[] = {"a", "b", "c", NULL};
    gchar *strings2[] = {"a", "b", "c", "d", NULL};

    cut_assert_equal_string_array(strings1, strings1);
    cut_assert_equal_string_array(strings2, strings2);
    MARK_FAIL(cut_assert_equal_string_array(strings1, strings2));
}

void
test_equal_string_array (void)
{
    test = cut_test_new("stub-equal-string-array", stub_equal_string_array);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "stub-equal-string-array", NULL,
                           "<strings1 == strings2>",
                           "[\"a\", \"b\", \"c\"]",
                           "[\"a\", \"b\", \"c\", \"d\"]",
                           FAIL_LOCATION, "stub_equal_string_array",
                           NULL);
}

void
test_null (void)
{
    cut_assert_null(NULL);
}

void
test_error_equal_string_with_null (void)
{
    test = cut_test_new("error-equal-string-with-null",
                        error_equal_string_with_null);
    cut_assert_not_null(test);
    cut_assert_false(run());
}

static void
stub_error (void)
{
    MARK_FAIL(cut_error("This test should error"));
}

void
test_error (void)
{
    test = cut_test_new("stub-error-test", stub_error);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 0, 1, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_ERROR,
                           "stub-error-test",
                           "This test should error", NULL,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_error",
                           NULL);
}

static void
stub_pend (void)
{
    cut_pend("This test has been pending ever!");
}

void
test_pending (void)
{
    /* FIXME: use cut_assert_test_result_summary() */
    test = cut_test_new("stub-pend-test", stub_pend);
    cut_assert_not_null(test);

    g_signal_connect(test, "pending", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_false(run(), cut_message("cut_pend() did not return FALSE!"));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);

    cut_assert_not_null(test_result,
                        cut_message("Could not get a CutTestResult object "
                                    "since \"pending\" signal "
                                    "was not emitted."));
    cut_assert_equal_int(CUT_TEST_RESULT_PENDING,
                         cut_test_result_get_status(test_result));
}

static void
stub_notify (void)
{
    cut_notify("This test has been notifying ever!");
}

void
test_notification (void)
{
    /* FIXME: use cut_assert_test_result_summary() */
    test = cut_test_new("stub-notify-test", stub_notify);
    cut_assert_not_null(test);

    g_signal_connect(test, "notification", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_true(run(), cut_message("cut_notify() did return TRUE!"));
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);

    cut_assert_not_null(test_result,
                        cut_message("Could not get a CutTestResult object "
                                    "since \"notification\" signal "
                                    "was not emitted."));
    cut_assert_equal_int(CUT_TEST_RESULT_NOTIFICATION,
                         cut_test_result_get_status(test_result));
}

static void
stub_fail (void)
{
    cut_fail("This test should fail. %s", "Exactly!");
}

void
test_fail (void)
{
    /* FIXME: use cut_assert_test_result_summary() */
    test = cut_test_new("stub-fail-test", stub_fail);
    cut_assert_not_null(test);

    g_signal_connect(test, "failure", G_CALLBACK(cb_collect_result),
                     &test_result);
    cut_assert_false(run());
    g_signal_handlers_disconnect_by_func(test,
                                         G_CALLBACK(cb_collect_result),
                                         &test_result);
    cut_assert_not_null(test_result,
                        cut_message("Could not get a CutTestResult object "
                                    "since \"failure\" signal "
                                    "was not emitted."));
    cut_assert_equal_int(CUT_TEST_RESULT_FAILURE,
                         cut_test_result_get_status(test_result));

    cut_assert_equal_string("This test should fail. Exactly!",
                            cut_test_result_get_message(test_result));
}

static void
stub_assert_message (void)
{
    MARK_FAIL(cut_assert(FALSE, cut_message("The message of %s", "assertion")));
}

void
test_assert_message (void)
{
    test = cut_test_new("stub-assertion-message-test",
                        stub_assert_message);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "stub-assertion-message-test",
                           "The message of assertion",
                           "expected: <FALSE> is neither FALSE nor NULL",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_assert_message",
                           NULL);
}

static void
stub_assert_message_with_format_string (void)
{
    MARK_FAIL(cut_assert_equal_string("%s", "%d",
                                      cut_message("%s and %s have format string",
                                                  "expected", "actual")));
}

void
test_assert_message_with_format_string (void)
{
    test = cut_test_new("stub-assert-message-with-string",
                        stub_assert_message_with_format_string);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "stub-assert-message-with-string",
                           "expected and actual have format string",
                           "<\"%s\" == \"%d\">",
                           "%s", "%d",
                           FAIL_LOCATION,
                           "stub_assert_message_with_format_string",
                           NULL);
}

static gboolean
compare_function (gpointer a, gpointer b)
{
    compare_function_is_called = TRUE;
    return TRUE;
}

void
test_assert_equal_function (void)
{
    cut_assert_false(compare_function_is_called);
    cut_assert_equal(compare_function, "o", "o");
    cut_assert_true(compare_function_is_called);
}

static void
fail_in_nested_function (void)
{
    cut_fail("Fail from nested function");
}

static void
just_call_fail_in_nested_function (void)
{
    fail_in_nested_function();
    cut_fail("Never here");
}

void
test_failure_from_nested_function (void)
{
    /* FIXME: use cut_assert_test_result_summary() */
    CutTestResult *result;

    test = cut_test_new("fail from nested function",
                        just_call_fail_in_nested_function);
    cut_assert_not_null(test);

    cut_assert_false(run());

    cut_assert_not_null(run_context);
    cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);

    result = cut_run_context_get_results(run_context)->data;
    cut_assert_equal_string("Fail from nested function",
                            cut_test_result_get_user_message(result));
}

static void
stub_null_string (void)
{
    cut_assert_null_string(NULL);
    MARK_FAIL(cut_assert_null_string(""));
}

void
test_null_string (void)
{
    test = cut_test_new("assert-null-string", stub_null_string);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "assert-null-string", NULL,
                           "<\"\"> is NULL",
                           "<NULL>", "",
                           FAIL_LOCATION, "stub_null_string",
                           NULL);
}

static void
equal_string_with_free_assertions (void)
{
    cut_assert_equal_string_with_free(NULL, NULL);
    cut_assert_equal_string_with_free("", g_strdup(""));
    cut_assert_equal_string_with_free("xxx", g_strdup("xxx"));
    cut_assert_equal_string_with_free("xxx", g_strdup("zzz"));
}

void
test_equal_string_with_free (void)
{
    test = cut_test_new("assert-string-equal-string-with-free",
                        equal_string_with_free_assertions);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 3, 0, 1, 0, 0, 0, 0);
}

static void
assert_errno_for_eacces (void)
{
    errno = 0;
    cut_assert_errno(cut_message("Passed"));

    errno = EACCES;
    cut_assert_errno(cut_message("Failed"));
}

void
test_assert_errno (void)
{
    test = cut_test_new("assert-errno-for-eacces", assert_errno_for_eacces);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
}

static void
omit_test (void)
{
    cut_assert_equal_int(1, 1);
    cut_omit("Omit the following tests");
    cut_assert_equal_int(2, 2);
}

void
test_omit (void)
{
    test = cut_test_new("omit-test", omit_test);
    cut_assert_true(run());
    cut_assert_test_result_summary(run_context, 1, 1, 1, 0, 0, 0, 0, 1);
}

static void
path_exist_test (void)
{
    gint fd;
    GError *error = NULL;

    fd = g_file_open_tmp(NULL, &tmp_file_name, &error);
    if (fd == -1) {
        const gchar *message;
        message = cut_take_string(gcut_error_inspect(error));
        g_error_free(error);
        cut_error("can't create temporary file: %s", message);
    }
    close(fd);

    cut_assert_path_exist(tmp_file_name);
    g_remove(tmp_file_name);
    cut_assert_path_exist(tmp_file_name);
}

void
test_path_exist (void)
{
    test = cut_test_new("path-exist-test", path_exist_test);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
}

static void
path_not_exist_test (void)
{
    gint fd;
    GError *error = NULL;

    fd = g_file_open_tmp(NULL, &tmp_file_name, &error);
    gcut_assert_error(error);
    close(fd);

    g_remove(tmp_file_name);
    cut_assert_path_not_exist(tmp_file_name);

    g_file_set_contents(tmp_file_name, "XXX", -1, &error);
    gcut_assert_error(error);
    cut_assert_path_not_exist(tmp_file_name);
}

void
test_path_not_exist (void)
{
    test = cut_test_new("path-not-exist-test", path_not_exist_test);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 3, 0, 1, 0, 0, 0, 0);
}

static void
match_test (void)
{
    cut_assert_match("^abc", "abc");
    cut_assert_match("^abc", " abc", cut_message("FAILED"));
}

void
test_match (void)
{
    test = cut_test_new("match-test", match_test);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
}

static void
equal_pointer_test (void)
{
    int int1, int2;

    cut_assert_equal_pointer(&int1, &int1);
    cut_assert_equal_pointer(&int1, &int2);
}

void
test_equal_pointer (void)
{
    test = cut_test_new("equal-pointer-test", equal_pointer_test);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
}

static void
equal_fixture_data_string_test (void)
{
    cut_assert_equal_fixture_data_string("top level data\n",
                                         "data.txt", NULL);
    cut_assert_equal_fixture_data_string("sub level data\n",
                                         "sub", "data.txt", NULL);
    cut_assert_equal_fixture_data_string("wrong data\n",
                                         "sub", "data.txt", NULL);
}

void
test_equal_fixture_data_string (void)
{
    test = cut_test_new("equal-fixture-data-sting-test",
                        equal_fixture_data_string_test);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 5, 0, 1, 0, 0, 0, 0);
}

static void
equal_fixture_data_string_test_without_file (void)
{
    cut_assert_equal_fixture_data_string("NONE!", "nonexistent.txt", NULL);
}

void
test_equal_fixture_data_string_without_file (void)
{
    test = cut_test_new("equal-fixture-data-sting-test-without-file",
                        equal_fixture_data_string_test_without_file);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 0, 1, 0, 0, 0);
}

static void
error_errno (void)
{
    errno = 0;
    cut_error_errno(cut_message("Not error"));

    errno = EACCES;
    cut_error_errno(cut_message("Should error"));
}

void
test_error_errno (void)
{
    /* FIXME: use cut_assert_test_result_summary() */
    CutTestResult *result;

    test = cut_test_new("error-errno", error_errno);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 0, 1, 0, 0, 0);

    result = cut_run_context_get_results(run_context)->data;
    cut_assert_equal_string("Should error",
                            cut_test_result_get_user_message(result));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
