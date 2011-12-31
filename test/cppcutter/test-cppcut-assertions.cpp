/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2011  Kouhei Sutou <kou@clear-code.com>
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

#define MARK_FAIL(assertion) do                 \
{                                               \
    fail_line = __LINE__;                       \
    assertion;                                  \
} while (0)

#define FAIL_LOCATION (cut_take_printf("%s:%d", __FILE__, fail_line))

namespace cppcut_assertion_equal
{
    CutTest *test;
    CutRunContext *run_context;
    CutTestContext *test_context;
    CutTestResult *test_result;

    gint fail_line;

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
    stub_int (void)
    {
        cppcut_assert_equal(100, 100);
        MARK_FAIL(cppcut_assert_equal(100, 1 - 2));
    }

    void
    test_int (void)
    {
        test = cut_test_new("equal_int test", stub_int);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_int test",
                               NULL,
                               "<100 == 1 - 2>",
                               "100", "-1",
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_int()",
                               NULL);
    }

    static void
    stub_int_reference (void)
    {
        int expected = 100;
        int actual = -100;
        cppcut_assert_equal(expected, expected);
        MARK_FAIL(cppcut_assert_equal(expected, actual));
    }

    void
    test_int_reference (void)
    {
        test = cut_test_new("equal_int_reference test",
                            stub_int_reference);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_int_reference test",
                               NULL,
                               "<expected == actual>",
                               "100", "-100",
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_int_reference()",
                               NULL);
    }

    static void
    stub_unsigned_int (void)
    {
        cppcut_assert_equal(100, 100);
        MARK_FAIL(cppcut_assert_equal(100, 2 - 1));
    }

    void
    test_unsigned_int (void)
    {
        test = cut_test_new("equal_unsigned_int test", stub_unsigned_int);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_unsigned_int test",
                               NULL,
                               "<100 == 2 - 1>",
                               "100", "1",
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_unsigned_int()",
                               NULL);
    }

    static void
    stub_long (void)
    {
#if GLIB_SIZEOF_LONG == 8
        cppcut_assert_equal(G_MAXINT64, G_MAXINT64);
        MARK_FAIL(cppcut_assert_equal(G_MAXINT64, G_MAXINT64 - 1));
#else
        cppcut_assert_equal(G_MAXINT32, G_MAXINT32);
        MARK_FAIL(cppcut_assert_equal(G_MAXINT32, G_MAXINT32 - 1));
#endif
    }

    void
    test_long (void)
    {
        test = cut_test_new("equal_long test", stub_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_long test",
                               NULL,
#if GLIB_SIZEOF_LONG == 8
                               "<G_MAXINT64 == G_MAXINT64 - 1>",
                               cut_take_printf("%" G_GINT64_FORMAT, G_MAXINT64),
                               cut_take_printf("%" G_GINT64_FORMAT, G_MAXINT64 - 1),
#else
                               "<G_MAXINT32 == G_MAXINT32 - 1>",
                               cut_take_printf("%" G_GINT32_FORMAT, G_MAXINT32),
                               cut_take_printf("%" G_GINT32_FORMAT, G_MAXINT32 - 1),
#endif
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_long()",
                               NULL);
    }

    static void
    stub_unsigned_long (void)
    {
#if GLIB_SIZEOF_LONG == 8
        cppcut_assert_equal(G_MAXUINT64, G_MAXUINT64);
        MARK_FAIL(cppcut_assert_equal(G_MAXUINT64, G_MAXUINT64 - 1));
#else
        cppcut_assert_equal(G_MAXUINT32, G_MAXUINT32);
        MARK_FAIL(cppcut_assert_equal(G_MAXUINT32, G_MAXUINT32 - 1));
#endif
    }

    void
    test_unsigned_long (void)
    {
        test = cut_test_new("equal_unsigned_long test",
                            stub_unsigned_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_unsigned_long test",
                               NULL,
#if GLIB_SIZEOF_LONG == 8
                               "<G_MAXUINT64 == G_MAXUINT64 - 1>",
                               cut_take_printf("%" G_GUINT64_FORMAT, G_MAXUINT64),
                               cut_take_printf("%" G_GUINT64_FORMAT,
                                               G_MAXUINT64 - 1),
#else
                               "<G_MAXUINT32 == G_MAXUINT32 - 1>",
                               cut_take_printf("%" G_GUINT32_FORMAT, G_MAXUINT32),
                               cut_take_printf("%" G_GUINT32_FORMAT,
                                               G_MAXUINT32 - 1),
#endif
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_unsigned_long()",
                               NULL);
    }

    static void
    stub_long_long (void)
    {
        cppcut_assert_equal(G_MAXINT64, G_MAXINT64);
        MARK_FAIL(cppcut_assert_equal(G_MAXINT64, G_MAXINT64 - 1));
    }

    void
    test_long_long (void)
    {
        test = cut_test_new("equal_long_long test", stub_long_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_long_long test",
                               NULL,
                               "<G_MAXINT64 == G_MAXINT64 - 1>",
                               cut_take_printf("%" G_GINT64_FORMAT, G_MAXINT64),
                               cut_take_printf("%" G_GINT64_FORMAT, G_MAXINT64 - 1),
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_long_long()",
                               NULL);
    }

    static void
    stub_unsigned_long_long (void)
    {
        cppcut_assert_equal(G_MAXUINT64, G_MAXUINT64);
        MARK_FAIL(cppcut_assert_equal(G_MAXUINT64, G_MAXUINT64 - 1));
    }

    void
    test_unsigned_long_long (void)
    {
        test = cut_test_new("equal_unsigned_long_long test",
                            stub_unsigned_long_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_unsigned_long_long test",
                               NULL,
                               "<G_MAXUINT64 == G_MAXUINT64 - 1>",
                               cut_take_printf("%" G_GUINT64_FORMAT, G_MAXUINT64),
                               cut_take_printf("%" G_GUINT64_FORMAT,
                                               G_MAXUINT64 - 1U),
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_unsigned_long_long()",
                               NULL);
    }

    static void
    stub_c_string_literal (void)
    {
        cppcut_assert_equal("abcde", "abcde");
        MARK_FAIL(cppcut_assert_equal("abcde", "ABcDE"));
    }

    void
    test_c_string_literal (void)
    {
        test = cut_test_new("equal_c_string_literal test",
                            stub_c_string_literal);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_c_string_literal test",
                               NULL,
                               "<\"abcde\" == \"ABcDE\">",
                               "\"abcde\"", "\"ABcDE\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_c_string_literal()",
                               NULL);
    }

    static void
    stub_c_string_buffer (void)
    {
        char expected[] = "abcde";
        char actual_same[] = "abcde";
        char actual_different[] = "ABcDE";
        cppcut_assert_equal(expected, actual_same);
        MARK_FAIL(cppcut_assert_equal(expected, actual_different));
    }

    void
    test_c_string_buffer (void)
    {
        test = cut_test_new("equal_c_string_buffer test",
                            stub_c_string_buffer);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_c_string_buffer test",
                               NULL,
                               "<expected == actual_different>",
                               "\"abcde\"", "\"ABcDE\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_c_string_buffer()",
                               NULL);
    }

    static void
    stub_c_string_reference (void)
    {
        const char *expected = "abcde";
        const char *actual_same = cut_take_strdup(expected);
        const char *actual_different = "ABcDE";
        cppcut_assert_equal(expected, actual_same);
        MARK_FAIL(cppcut_assert_equal(expected, actual_different));
    }

    void
    test_c_string_reference (void)
    {
        test = cut_test_new("equal_c_string_reference test",
                            stub_c_string_reference);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_c_string_reference test",
                               NULL,
                               "<expected == actual_different>",
                               "\"abcde\"", "\"ABcDE\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_c_string_reference()",
                               NULL);
    }

    static void
    stub_c_string_mix (void)
    {
        char expected[] = "abcde";
        const char *actual_same = cut_take_strdup(expected);
        const char *actual_different = "ABcDE";
        cppcut_assert_equal(expected, actual_same);
        MARK_FAIL(cppcut_assert_equal(expected, actual_different));
    }

    void
    test_c_string_mix (void)
    {
        test = cut_test_new("equal_c_string_mix test",
                            stub_c_string_mix);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_c_string_mix test",
                               NULL,
                               "<expected == actual_different>",
                               "\"abcde\"", "\"ABcDE\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_c_string_mix()",
                               NULL);
    }

    static void
    stub_string (void)
    {
        std::string expected("abcde");
        std::string actual_same(cut_take_printf(expected.c_str()));
        std::string actual_different("ABcDE");
        cppcut_assert_equal(expected, actual_same);
        MARK_FAIL(cppcut_assert_equal(expected, actual_different));
    }

    void
    test_string (void)
    {
        test = cut_test_new("equal_string test", stub_string);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_string test",
                               NULL,
                               "<expected == actual_different>",
                               "\"abcde\"", "\"ABcDE\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_equal::stub_string()",
                               NULL);
    }
}

namespace cppcut_assertion_not_equal
{
    CutTest *test;
    CutRunContext *run_context;
    CutTestContext *test_context;
    CutTestResult *test_result;

    gint fail_line;

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
    stub_int (void)
    {
        cppcut_assert_not_equal(-100, 1 - 2);
        MARK_FAIL(cppcut_assert_not_equal(-100, -99 - 1));
    }

    void
    test_int (void)
    {
        test = cut_test_new("not_equal_int test", stub_int);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_int test",
                               NULL,
                               "<-100 != -99 - 1>",
                               "-100", "-100",
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_int()",
                               NULL);
    }

    static void
    stub_int_reference (void)
    {
        int expected = -100;
        int actual_same = expected;
        int actual_different = 100;
        cppcut_assert_not_equal(expected, actual_different);
        MARK_FAIL(cppcut_assert_not_equal(expected, actual_same));
    }

    void
    test_int_reference (void)
    {
        test = cut_test_new("not_equal_int_reference test",
                            stub_int_reference);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_int_reference test",
                               NULL,
                               "<expected != actual_same>",
                               "-100", "-100",
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_int_reference()",
                               NULL);
    }

    static void
    stub_unsigned_int (void)
    {
        cppcut_assert_not_equal(100, 2 - 1);
        MARK_FAIL(cppcut_assert_not_equal(100, 99 + 1));
    }

    void
    test_unsigned_int (void)
    {
        test = cut_test_new("not_equal_unsigned_int test", stub_unsigned_int);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_unsigned_int test",
                               NULL,
                               "<100 != 99 + 1>",
                               "100", "100",
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_unsigned_int()",
                               NULL);
    }

    static void
    stub_long (void)
    {
#if GLIB_SIZEOF_LONG == 8
        cppcut_assert_not_equal(G_MININT64, G_MININT64 + 1);
        MARK_FAIL(cppcut_assert_not_equal(G_MININT64, G_MININT64));
#else
        cppcut_assert_not_equal(G_MININT32, G_MININT32 - 1);
        MARK_FAIL(cppcut_assert_not_equal(G_MININT32, G_MININT32));
#endif
    }

    void
    test_long (void)
    {
        test = cut_test_new("not_equal_long test", stub_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_long test",
                               NULL,
#if GLIB_SIZEOF_LONG == 8
                               "<G_MININT64 != G_MININT64>",
                               cut_take_printf("%" G_GINT64_FORMAT, G_MININT64),
                               cut_take_printf("%" G_GINT64_FORMAT, G_MININT64),
#else
                               "<G_MININT32 != G_MININT32>",
                               cut_take_printf("%" G_GINT32_FORMAT, G_MININT32),
                               cut_take_printf("%" G_GINT32_FORMAT, G_MININT32),
#endif
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_long()",
                               NULL);
    }

    static void
    stub_unsigned_long (void)
    {
#if GLIB_SIZEOF_LONG == 8
        cppcut_assert_not_equal(G_MAXUINT64, G_MAXUINT64 - 1);
        MARK_FAIL(cppcut_assert_not_equal(G_MAXUINT64, G_MAXUINT64));
#else
        cppcut_assert_not_equal(G_MAXUINT32, G_MAXUINT32 - 1);
        MARK_FAIL(cppcut_assert_not_equal(G_MAXUINT32, G_MAXUINT32));
#endif
    }

    void
    test_unsigned_long (void)
    {
        test = cut_test_new("not_equal_unsigned_long test",
                            stub_unsigned_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_unsigned_long test",
                               NULL,
#if GLIB_SIZEOF_LONG == 8
                               "<G_MAXUINT64 != G_MAXUINT64>",
                               cut_take_printf("%" G_GUINT64_FORMAT, G_MAXUINT64),
                               cut_take_printf("%" G_GUINT64_FORMAT, G_MAXUINT64),
#else
                               "<G_MAXUINT32 != G_MAXUINT32>",
                               cut_take_printf("%" G_GUINT32_FORMAT, G_MAXUINT32),
                               cut_take_printf("%" G_GUINT32_FORMAT, G_MAXUINT32),
#endif
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_unsigned_long()",
                               NULL);
    }

    static void
    stub_long_long (void)
    {
        cppcut_assert_not_equal(G_MININT64, G_MININT64 + 1);
        MARK_FAIL(cppcut_assert_not_equal(G_MININT64, G_MININT64));
    }

    void
    test_long_long (void)
    {
        test = cut_test_new("not_equal_long_long test", stub_long_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_long_long test",
                               NULL,
                               "<G_MININT64 != G_MININT64>",
                               cut_take_printf("%" G_GINT64_FORMAT, G_MININT64),
                               cut_take_printf("%" G_GINT64_FORMAT, G_MININT64),
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_long_long()",
                               NULL);
    }

    static void
    stub_unsigned_long_long (void)
    {
        cppcut_assert_not_equal(G_MAXUINT64, G_MAXUINT64 - 1);
        MARK_FAIL(cppcut_assert_not_equal(G_MAXUINT64, G_MAXUINT64));
    }

    void
    test_unsigned_long_long (void)
    {
        test = cut_test_new("not_equal_unsigned_long_long test",
                            stub_unsigned_long_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_unsigned_long_long test",
                               NULL,
                               "<G_MAXUINT64 != G_MAXUINT64>",
                               cut_take_printf("%" G_GUINT64_FORMAT, G_MAXUINT64),
                               cut_take_printf("%" G_GUINT64_FORMAT, G_MAXUINT64),
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_unsigned_long_long()",
                               NULL);
    }

    static void
    stub_c_string_literal (void)
    {
        cppcut_assert_not_equal("abcde", "ABcDE");
        MARK_FAIL(cppcut_assert_not_equal("abcde", "ab" "c" "de"));
    }

    void
    test_c_string_literal (void)
    {
        test = cut_test_new("not_equal_c_string_literal test",
                            stub_c_string_literal);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_c_string_literal test",
                               NULL,
                               "<\"abcde\" != \"ab\" \"c\" \"de\">",
                               "\"abcde\"", "\"abcde\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_c_string_literal()",
                               NULL);
    }

    static void
    stub_c_string_buffer (void)
    {
        char expected[] = "abcde";
        char actual_same[] = "abcde";
        char actual_different[] = "ABcDE";
        cppcut_assert_not_equal(expected, actual_different);
        MARK_FAIL(cppcut_assert_not_equal(expected, actual_same));
    }

    void
    test_c_string_buffer (void)
    {
        test = cut_test_new("not_equal_c_string_buffer test",
                            stub_c_string_buffer);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_c_string_buffer test",
                               NULL,
                               "<expected != actual_same>",
                               "\"abcde\"", "\"abcde\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_c_string_buffer()",
                               NULL);
    }

    static void
    stub_c_string_reference (void)
    {
        const char *expected = "abcde";
        const char *actual_same = "abcde";
        const char *actual_different = "ABcDE";
        cppcut_assert_not_equal(expected, actual_different);
        MARK_FAIL(cppcut_assert_not_equal(expected, actual_same));
    }

    void
    test_c_string_reference (void)
    {
        test = cut_test_new("not_equal_c_string_reference test",
                            stub_c_string_reference);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_c_string_reference test",
                               NULL,
                               "<expected != actual_same>",
                               "\"abcde\"", "\"abcde\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_c_string_reference()",
                               NULL);
    }

    static void
    stub_c_string_mix (void)
    {
        char expected[] = "abcde";
        const char *actual_same = "abcde";
        const char *actual_different = "ABcDE";
        cppcut_assert_not_equal(expected, actual_different);
        MARK_FAIL(cppcut_assert_not_equal(expected, actual_same));
    }

    void
    test_c_string_mix (void)
    {
        test = cut_test_new("not_equal_c_string_mix test",
                            stub_c_string_mix);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_c_string_mix test",
                               NULL,
                               "<expected != actual_same>",
                               "\"abcde\"", "\"abcde\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_c_string_mix()",
                               NULL);
    }

    static void
    stub_string (void)
    {
        std::string expected("abcde");
        std::string actual_same("abcde");
        std::string actual_different("ABcDE");
        cppcut_assert_not_equal(expected, actual_different);
        MARK_FAIL(cppcut_assert_not_equal(expected, actual_same));
    }

    void
    test_string (void)
    {
        test = cut_test_new("not_equal_string test", stub_string);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "not_equal_string test",
                               NULL,
                               "<expected != actual_same>",
                               "\"abcde\"", "\"abcde\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_equal::stub_string()",
                               NULL);
    }
}

namespace cppcut_assertion_null
{
    CutTest *test;
    CutRunContext *run_context;
    CutTestContext *test_context;
    CutTestResult *test_result;

    gint fail_line;

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
    stub_c_string (void)
    {
        const gchar *message = "abcde";
        const gchar *not_null_object = message;
        const gchar *null_object = NULL;
        cppcut_assert_null(null_object);
        MARK_FAIL(cppcut_assert_null(not_null_object));
    }

    void
    test_c_string (void)
    {
        test = cut_test_new("C string test", stub_c_string);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "C string test",
                               NULL,
                               "expected: <abcde>(<not_null_object>) is NULL",
                               NULL,
                               NULL,
                               FAIL_LOCATION,
                               "void cppcut_assertion_null::stub_c_string()",
                               NULL);
    }

    static void
    stub_std_string (void)
    {
        std::string message("abcde");
        std::string *not_null_object = &message;
        std::string *null_object = NULL;
        cppcut_assert_null(null_object);
        MARK_FAIL(cppcut_assert_null(not_null_object));
    }

    void
    test_std_string (void)
    {
        test = cut_test_new("std::string test", stub_std_string);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "std::string test",
                               NULL,
                               "expected: <\"abcde\">(<not_null_object>) is NULL",
                               NULL,
                               NULL,
                               FAIL_LOCATION,
                               "void cppcut_assertion_null::stub_std_string()",
                               NULL);
    }
}

namespace cppcut_assertion_not_null
{
    CutTest *test;
    CutRunContext *run_context;
    CutTestContext *test_context;
    CutTestResult *test_result;

    gint fail_line;

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
    stub_std_string (void)
    {
        std::string message("abcde");
        std::string *not_null_object = &message;
        std::string *null_object = NULL;
        cppcut_assert_not_null(not_null_object);
        MARK_FAIL(cppcut_assert_not_null(null_object));
    }

    void
    test_std_string (void)
    {
        test = cut_test_new("std::string test", stub_std_string);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "std::string test",
                               NULL,
                               "expected: <null_object> is not NULL",
                               NULL,
                               NULL,
                               FAIL_LOCATION,
                               "void cppcut_assertion_not_null::stub_std_string()",
                               NULL);
    }
}

namespace cppcut_assertion_operator
{
    CutTest *test;
    CutRunContext *run_context;
    CutTestContext *test_context;
    CutTestResult *test_result;

    gint fail_line;

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
    stub_int (void)
    {
        int lhs = 3;
        int rhs = 5;
        cppcut_assert_operator(lhs, <, rhs);
        MARK_FAIL(cppcut_assert_operator(lhs, >, rhs));
    }

    void
    test_int (void)
    {
        test = cut_test_new("int test", stub_int);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "int test",
                               NULL,
                               "expected: <lhs> > <rhs>\n"
                               "  actual: <3> > <5>",
                               NULL,
                               NULL,
                               FAIL_LOCATION,
                               "void cppcut_assertion_operator::stub_int()",
                               NULL);
    }

    static void
    stub_unsigned_int (void)
    {
        unsigned int lhs = 3;
        unsigned int rhs = 5;
        cppcut_assert_operator(lhs, <, rhs);
        MARK_FAIL(cppcut_assert_operator(lhs, >, rhs));
    }

    void
    test_unsigned_int (void)
    {
        test = cut_test_new("unsigned int test", stub_unsigned_int);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "unsigned int test",
                               NULL,
                               "expected: <lhs> > <rhs>\n"
                               "  actual: <3> > <5>",
                               NULL,
                               NULL,
                               FAIL_LOCATION,
                               "void cppcut_assertion_operator::stub_unsigned_int()",
                               NULL);
    }

    static void
    stub_long (void)
    {
        long lhs = 3;
        long rhs = 5;
        cppcut_assert_operator(lhs, <, rhs);
        MARK_FAIL(cppcut_assert_operator(lhs, >, rhs));
    }

    void
    test_long (void)
    {
        test = cut_test_new("long test", stub_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "long test",
                               NULL,
                               "expected: <lhs> > <rhs>\n"
                               "  actual: <3> > <5>",
                               NULL,
                               NULL,
                               FAIL_LOCATION,
                               "void cppcut_assertion_operator::stub_long()",
                               NULL);
    }

    static void
    stub_unsigned_long (void)
    {
        unsigned long lhs = 3;
        unsigned long rhs = 5;
        cppcut_assert_operator(lhs, <, rhs);
        MARK_FAIL(cppcut_assert_operator(lhs, >, rhs));
    }

    void
    test_unsigned_long (void)
    {
        test = cut_test_new("unsigned long test", stub_unsigned_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "unsigned long test",
                               NULL,
                               "expected: <lhs> > <rhs>\n"
                               "  actual: <3> > <5>",
                               NULL,
                               NULL,
                               FAIL_LOCATION,
                               "void cppcut_assertion_operator::stub_unsigned_long()",
                               NULL);
    }

    static void
    stub_long_long (void)
    {
        long long lhs = 3;
        long long rhs = 5;
        cppcut_assert_operator(lhs, <, rhs);
        MARK_FAIL(cppcut_assert_operator(lhs, >, rhs));
    }

    void
    test_long_long (void)
    {
        test = cut_test_new("long long test", stub_long_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "long long test",
                               NULL,
                               "expected: <lhs> > <rhs>\n"
                               "  actual: <3> > <5>",
                               NULL,
                               NULL,
                               FAIL_LOCATION,
                               "void cppcut_assertion_operator::stub_long_long()",
                               NULL);
    }

    static void
    stub_unsigned_long_long (void)
    {
        unsigned long long lhs = 3;
        unsigned long long rhs = 5;
        cppcut_assert_operator(lhs, <, rhs);
        MARK_FAIL(cppcut_assert_operator(lhs, >, rhs));
    }

    void
    test_unsigned_long_long (void)
    {
        test = cut_test_new("unsigned long long test", stub_unsigned_long_long);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "unsigned long long test",
                               NULL,
                               "expected: <lhs> > <rhs>\n"
                               "  actual: <3> > <5>",
                               NULL,
                               NULL,
                               FAIL_LOCATION,
                               "void cppcut_assertion_operator::stub_unsigned_long_long()",
                               NULL);
    }

}

namespace cppcut_assertion_message
{
    CutTest *test;
    CutRunContext *run_context;
    CutTestContext *test_context;
    CutTestResult *test_result;

    gint fail_line;

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
    stub_format (void)
    {
        MARK_FAIL(cppcut_assert_equal(
                      "abcde", "ABCDE",
                      cppcut_message("The message of %s", "assertion")));
    }

    void
    test_format (void)
    {
        test = cut_test_new("optional message test", stub_format);
        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "optional message test",
                               "The message of assertion",
                               "<\"abcde\" == \"ABCDE\">",
                               "\"abcde\"", "\"ABCDE\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_message::stub_format()",
                               NULL);
    }

    static void
    stub_shift (void)
    {
        MARK_FAIL(cppcut_assert_equal(
                      "abcde", "ABCDE",
                      cppcut_message() << "The message of " << "assertion"));
    }

    void
    test_shift (void)
    {
        test = cut_test_new("optional message test", stub_shift);
        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "optional message test",
                               "The message of assertion",
                               "<\"abcde\" == \"ABCDE\">",
                               "\"abcde\"", "\"ABCDE\"",
                               FAIL_LOCATION,
                               "void cppcut_assertion_message::stub_shift()",
                               NULL);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4:ts=4
*/
