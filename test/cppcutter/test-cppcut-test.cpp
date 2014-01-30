/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2014  Kouhei Sutou <kou@clear-code.com>
 *  Copyright (C) 2014  Kazuhiro Yamato <kz0817@gmail.com>
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

#include <typeinfo>

#include <cppcutter.h>

#include <cppcutter/cppcut-test.h>
#include <cutter/cut-test-runner.h>

#include "../lib/cuttest-assertions.h"

#define MARK_FAIL(assertion) do                 \
{                                               \
    fail_line = __LINE__;                       \
    assertion;                                  \
} while (0)

#define FAIL_LOCATION (cut_take_printf("%s:%d", __FILE__, fail_line))

namespace cppcut_test
{
    CppCutTest *test;
    CutRunContext *run_context;
    CutTestContext *test_context;
    CutTestResult *test_result;
    gint n_destructor_calls;
    gint fail_line;

    static gboolean
    run (void)
    {
        gboolean success;

        run_context = CUT_RUN_CONTEXT(cut_test_runner_new());

        test_context = cut_test_context_new(run_context, NULL, NULL, NULL,
                                            CUT_TEST(test));
        cut_test_context_current_push(test_context);
        success = cut_test_runner_run_test(CUT_TEST_RUNNER(run_context),
                                           CUT_TEST(test), test_context);
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

        n_destructor_calls = 0;
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

    class StubException : public std::exception
    {
    public:
        StubException(const char *message) : message_(message)
        {
        }

        virtual const char *
        what(void) const throw()
        {
            return message_;
        }

    private:
        const char *message_;
    };

    static void
    stub_std_exception (void)
    {
        throw StubException("std::exception family exception");
    }

    void
    test_std_exception (void)
    {
        test = cppcut_test_new("std::exception test", stub_std_exception);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 0, 0, 0, 1, 0, 0, 0);
        const gchar *system_message =
            cut_take_printf("Unhandled C++ standard exception is thrown: "
                            "<%s>: %s",
                            typeid(StubException).name(),
                            "std::exception family exception");
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_ERROR,
                               "std::exception test",
                               NULL,
                               system_message,
                               NULL, NULL,
                               NULL,
                               "void cppcut_test::stub_standard_exception()",
                               NULL);
    }

    static void
    stub_not_std_exception (void)
    {
        throw "not std::exception";
    }

    void
    test_not_std_exception (void)
    {
        test = cppcut_test_new("not std::exception test",
                               stub_not_std_exception);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 0, 0, 0, 1, 0, 0, 0);
        const gchar *system_message =
            "Unhandled C++ non-standard exception is thrown";
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_ERROR,
                               "not std::exception test",
                               NULL,
                               system_message,
                               NULL, NULL,
                               NULL,
                               "void cppcut_test::stub_not_standard_exception()",
                               NULL);
    }

    static void
    stub_exception_on_stacked_jump_buffer (void)
    {
        struct {
            bool operator ()(void) {
                throw "exception on stacked jump buffer";
            }
        } stub_throw;
        MARK_FAIL(cppcut_assert_equal(true, stub_throw()));
    }

    void
    test_exception_on_stacked_jump_buffer (void)
    {
        test = cppcut_test_new("exception on stacked jump buffer test",
                               stub_exception_on_stacked_jump_buffer);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 0, 0, 0, 1, 0, 0, 0);
        const gchar *system_message =
            "Unhandled C++ non-standard exception is thrown";
        cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_ERROR,
                               "exception on stacked jump buffer test",
                               NULL,
                               system_message,
                               NULL, NULL,
                               FAIL_LOCATION,
                               "void cppcut_test::stub_exception_on_stacked_jump_buffer()",
                               NULL);
    }

    class DestructorCallCounter
    {
    public:
        ~DestructorCallCounter()
        {
            n_destructor_calls++;
        }
    };

    static void
    stub_unwind_stack (void)
    {
        DestructorCallCounter counter;

        cut_fail("return from the test");
    }

    void
    test_unwind_stack (void)
    {
        test = cppcut_test_new("unwind stack test", stub_unwind_stack);
        cut_assert_not_null(test);

        cut_assert_false(run());
        cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);
        cut_assert_equal_int(1, n_destructor_calls);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4:ts=4
*/
