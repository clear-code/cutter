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

#include <cppcutter.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-utils.h>
#include <cutter/cut-test-runner.h>
#include <cutter/cut-repository.h>
#include <cutter/cut-enum-types.h>
#include <cutter/cut-loader.h>
#include "../lib/cuttest-assertions.h"

CPPCUT_BEGIN_TEST_DECLS
void test_equal_int(void);
void test_equal_int_reference(void);
void test_equal_unsigned_int(void);
void test_equal_long(void);
void test_equal_unsigned_long(void);
void test_equal_float(void);
void test_equal_double(void);
void test_equal_c_string(void);
void test_equal_c_string_reference(void);
void test_equal_string(void);
CPPCUT_END_TEST_DECLS

static CutTest *test;
static CutRunContext *run_context;
static CutTestContext *test_context;
static CutTestResult *test_result;

static GError *error;
static GError *error1, *error2;
static gboolean need_to_free_error;

static GType flags_type = 0;

static gint fail_line;

#define MARK_FAIL(assertion) do                 \
{                                               \
    fail_line = __LINE__;                       \
    assertion;                                  \
} while (0)

#define FAIL_LOCATION (cut_take_printf("%s:%d", __FILE__, fail_line))

static gboolean
run (void)
{
    gboolean success;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());

    test_context = cut_test_context_new(run_context, NULL, NULL, NULL, test);
    cut_test_context_current_push(test_context);
    success = cut_test_runner_run_test(CUT_TEST_RUNNER(run_context),
                                       test, test_context);
    cut_test_context_current_pop();

    return success;
}

void
cut_setup (void)
{
    test = NULL;
    run_context = NULL;
    test_context = NULL;
    test_result = NULL;

    fail_line = 0;
}

void
cut_teardown (void)
{
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
stub_equal_int (void)
{
    cppcut_assert_equal(100, 100);
    MARK_FAIL(cppcut_assert_equal(100, 1 - 2));
}

void
test_equal_int (void)
{
    test = cut_test_new("equal_int test", stub_equal_int);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_int test",
                           NULL,
                           "<100 == 1 - 2>",
                           "100", "-1",
                           FAIL_LOCATION, "void stub_equal_int()",
                           NULL);
}

static void
stub_equal_int_reference (void)
{
    int expected = 100;
    int actual = -100;
    cppcut_assert_equal(expected, expected);
    MARK_FAIL(cppcut_assert_equal(expected, actual));
}

void
test_equal_int_reference (void)
{
    test = cut_test_new("equal_int_reference test", stub_equal_int_reference);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_int_reference test",
                           NULL,
                           "<expected == actual>",
                           "100", "-100",
                           FAIL_LOCATION, "void stub_equal_int_reference()",
                           NULL);
}

static void
stub_equal_unsigned_int (void)
{
    cppcut_assert_equal(100, 100);
    MARK_FAIL(cppcut_assert_equal(100, 2 - 1));
}

void
test_equal_unsigned_int (void)
{
    test = cut_test_new("equal_unsigned_int test", stub_equal_unsigned_int);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_unsigned_int test",
                           NULL,
                           "<100 == 2 - 1>",
                           "100", "1",
                           FAIL_LOCATION, "void stub_equal_unsigned_int()",
                           NULL);
}

static void
stub_equal_long (void)
{
    cppcut_assert_equal(2147483648, 2147483648);
    MARK_FAIL(cppcut_assert_equal(2147483648, 2147483648 + 1));
}

void
test_equal_long (void)
{
    test = cut_test_new("equal_long test", stub_equal_long);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_long test",
                           NULL,
                           "<2147483648 == 2147483648 + 1>",
                           "2147483648", "2147483649",
                           FAIL_LOCATION, "void stub_equal_long()",
                           NULL);
}

static void
stub_equal_unsigned_long (void)
{
    cppcut_assert_equal(4294967296, 4294967296);
    MARK_FAIL(cppcut_assert_equal(4294967296, 4294967296 + 1));
}

void
test_equal_unsigned_long (void)
{
    test = cut_test_new("equal_unsigned_long test", stub_equal_unsigned_long);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_unsigned_long test",
                           NULL,
                           "<4294967296 == 4294967296 + 1>",
                           "4294967296", "4294967297",
                           FAIL_LOCATION, "void stub_equal_unsigned_long()",
                           NULL);
}

static void
stub_equal_c_string (void)
{
    cppcut_assert_equal("abcde", "abcde");
    MARK_FAIL(cppcut_assert_equal("abcde", "ABcDE"));
}

void
test_equal_c_string (void)
{
    test = cut_test_new("equal_c_string test", stub_equal_c_string);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_c_string test",
                           NULL,
                           "<\"abcde\" == \"ABcDE\">",
                           "abcde", "ABcDE",
                           FAIL_LOCATION, "void stub_equal_c_string()",
                           NULL);
}

static void
stub_equal_c_string_reference (void)
{
    char expected[] = "abcde";
    char actual[] = "ABcDE";
    cppcut_assert_equal(expected, expected);
    MARK_FAIL(cppcut_assert_equal(expected, actual));
}

void
test_equal_c_string_reference (void)
{
    test = cut_test_new("equal_c_string_reference test",
                        stub_equal_c_string_reference);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_c_string_reference test",
                           NULL,
                           "<expected == actual>",
                           "abcde", "ABcDE",
                           FAIL_LOCATION, "void stub_equal_c_string_reference()",
                           NULL);
}

static void
stub_equal_string (void)
{
    std::string expected("abcde");
    std::string actual("ABcDE");
    cppcut_assert_equal(expected, expected);
    MARK_FAIL(cppcut_assert_equal(expected, actual));
}

void
test_equal_string (void)
{
    test = cut_test_new("equal_string test", stub_equal_string);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_string test",
                           NULL,
                           "<expected == actual>",
                           "abcde", "ABcDE",
                           FAIL_LOCATION, "void stub_equal_string()",
                           NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4:ts=4
*/
