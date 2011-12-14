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

namespace cppcut_assertions
{
    CutTest *test;
    CutRunContext *run_context;
    CutTestContext *test_context;
    CutTestResult *test_result;

    GError *error;
    GError *error1, *error2;
    gboolean need_to_free_error;

    GType flags_type = 0;

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
                               FAIL_LOCATION,
                               "void cppcut_assertions::stub_equal_int()",
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
        test = cut_test_new("equal_int_reference test",
                            stub_equal_int_reference);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_int_reference test",
                               NULL,
                               "<expected == actual>",
                               "100", "-100",
                               FAIL_LOCATION,
                               "void cppcut_assertions::stub_equal_int_reference()",
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
                               FAIL_LOCATION,
                               "void cppcut_assertions::stub_equal_unsigned_int()",
                               NULL);
    }

    static void
    stub_equal_long (void)
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
    test_equal_long (void)
    {
        test = cut_test_new("equal_long test", stub_equal_long);
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
                               "void cppcut_assertions::stub_equal_long()",
                               NULL);
    }

    static void
    stub_equal_unsigned_long (void)
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
    test_equal_unsigned_long (void)
    {
        test = cut_test_new("equal_unsigned_long test",
                            stub_equal_unsigned_long);
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
                               "void cppcut_assertions::stub_equal_unsigned_long()",
                               NULL);
    }

    static void
    stub_equal_long_long (void)
    {
        cppcut_assert_equal(G_MAXINT64, G_MAXINT64);
        MARK_FAIL(cppcut_assert_equal(G_MAXINT64, G_MAXINT64 - 1));
    }

    void
    test_equal_long_long (void)
    {
        test = cut_test_new("equal_long_long test", stub_equal_long_long);
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
                               "void cppcut_assertions::stub_equal_long_long()",
                               NULL);
    }

    static void
    stub_equal_unsigned_long_long (void)
    {
        cppcut_assert_equal(G_MAXUINT64, G_MAXUINT64);
        MARK_FAIL(cppcut_assert_equal(G_MAXUINT64, G_MAXUINT64 - 1));
    }

    void
    test_equal_unsigned_long_long (void)
    {
        test = cut_test_new("equal_unsigned_long_long test",
                            stub_equal_unsigned_long_long);
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
                               "void cppcut_assertions::stub_equal_unsigned_long_long()",
                               NULL);
    }

    static void
    stub_equal_c_string_literal (void)
    {
        cppcut_assert_equal("abcde", "abcde");
        MARK_FAIL(cppcut_assert_equal("abcde", "ABcDE"));
    }

    void
    test_equal_c_string_literal (void)
    {
        test = cut_test_new("equal_c_string_literal test",
                            stub_equal_c_string_literal);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_c_string_literal test",
                               NULL,
                               "<\"abcde\" == \"ABcDE\">",
                               "abcde", "ABcDE",
                               FAIL_LOCATION,
                               "void cppcut_assertions::stub_equal_c_string_literal()",
                               NULL);
    }

    static void
    stub_equal_c_string_buffer (void)
    {
        char expected[] = "abcde";
        char actual[] = "ABcDE";
        cppcut_assert_equal(expected, expected);
        MARK_FAIL(cppcut_assert_equal(expected, actual));
    }

    void
    test_equal_c_string_buffer (void)
    {
        test = cut_test_new("equal_c_string_buffer test",
                            stub_equal_c_string_buffer);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_c_string_buffer test",
                               NULL,
                               "<expected == actual>",
                               "abcde", "ABcDE",
                               FAIL_LOCATION,
                               "void cppcut_assertions::stub_equal_c_string_buffer()",
                               NULL);
    }

    static void
    stub_equal_c_string_reference (void)
    {
        const char *expected = "abcde";
        const char *actual = "ABcDE";
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
                               FAIL_LOCATION,
                               "void cppcut_assertions::stub_equal_c_string_reference()",
                               NULL);
    }

    static void
    stub_equal_c_string_mix (void)
    {
        char expected[] = "abcde";
        const char *actual = "ABcDE";
        cppcut_assert_equal(expected, expected);
        MARK_FAIL(cppcut_assert_equal(expected, actual));
    }

    void
    test_equal_c_string_mix (void)
    {
        test = cut_test_new("equal_c_string_mix test",
                            stub_equal_c_string_mix);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "equal_c_string_mix test",
                               NULL,
                               "<expected == actual>",
                               "abcde", "ABcDE",
                               FAIL_LOCATION,
                               "void cppcut_assertions::stub_equal_c_string_mix()",
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
                               FAIL_LOCATION,
                               "void cppcut_assertions::stub_equal_string()",
                               NULL);
    }

    static void
    stub_assert_string (void)
    {
        std::string object("abcde");
        std::string *null_object = NULL;
        cppcut_assert(&object);
        MARK_FAIL(cppcut_assert(null_object));
    }

    void
    test_assert_string (void)
    {
        test = cut_test_new("assert_string test", stub_assert_string);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "assert_string test",
                               NULL,
                               "expected: <0> is neither false nor NULL",
                               NULL,
                               NULL,
                               FAIL_LOCATION,
                               "void cppcut_assertions::stub_assert_string()",
                               NULL);
    }

    static void
    stub_message (void)
    {
        MARK_FAIL(cppcut_assert_equal(
                      "abcde", "ABCDE",
                      cppcut_message("The message of %s", "assertion")));
    }

    void
    test_message (void)
    {
        test = cut_test_new("optional message test", stub_message);
        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "optional message test",
                               "The message of assertion",
                               "<\"abcde\" == \"ABCDE\">",
                               "abcde", "ABCDE",
                               FAIL_LOCATION,
                               "void cppcut_assertions::stub_message()",
                               NULL);
    }

    static void
    stub_message_shift (void)
    {
        MARK_FAIL(cppcut_assert_equal(
                      "abcde", "ABCDE",
                      cppcut_message() << "The message of " << "assertion"));
    }

    void
    test_message_shift (void)
    {
        test = cut_test_new("optional message test", stub_message_shift);
        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                               "optional message test",
                               "The message of assertion",
                               "<\"abcde\" == \"ABCDE\">",
                               "abcde", "ABCDE",
                               FAIL_LOCATION,
                               "void cppcut_assertions::stub_message_shift()",
                               NULL);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4:ts=4
*/
