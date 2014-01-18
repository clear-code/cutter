/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <exception>
#include <typeinfo>
#include <string>
#include <cutter.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-runner.h>

#include "../lib/cuttest-assertions.h"

static CutTest *test;
static CutRunContext *run_context;
static CutTestContext *test_context;

class test_exception : public std::exception {
public:
    virtual const char *
    what(void) const throw() // override
    {
        return message;
    }

    static const char *message;
};
const char *test_exception::message = "Test exception";

extern "C"
void
cut_setup (void)
{
    test = NULL;
    run_context = NULL;
    test_context = NULL;
}

extern "C"
void
cut_teardown (void)
{
    if (test)
        g_object_unref(test);
    if (run_context)
        g_object_unref(run_context);
    if (test_context)
        g_object_unref(test_context);
}

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

static void
stub_throw_std_exception (void)
{
    throw test_exception();
}

static void
stub_throw_non_std_exception (void)
{
    throw 1;
}

static std::string
expected_system_message (const bool &is_std_exception)
{
    test_exception e;
    std::string expected = "received a C++ exception";
    if (is_std_exception) {
        expected += ": <";
        expected += typeid(test_exception).name();
        expected += "> ";
        expected += test_exception::message;
    }
    return expected;
}

extern "C"
void
test_catch_unhandled_std_exception (void)
{
    test = cut_test_new("catch an unhandled C++ standard exception",
                        stub_throw_std_exception);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 0, 1, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_ERROR,
                           "catch an unhandled C++ standard exception",
                           NULL,
                           expected_system_message(true).c_str(),
                           NULL, NULL, NULL,
                           NULL);
}

extern "C"
void
test_catch_unhandled_non_std_exception (void)
{
    test = cut_test_new("catch an unhandled C++ non-standard exception",
                        stub_throw_non_std_exception);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 0, 1, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_ERROR,
                           "catch an unhandled C++ non-standard exception",
                           NULL,
                           expected_system_message(false).c_str(),
                           NULL, NULL, NULL,
                           NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
