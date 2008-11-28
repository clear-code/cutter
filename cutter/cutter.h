/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2008  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUTTER_H__
#define __CUTTER_H__

#include <cutter/cut-version.h>
#include <cutter/cut-features.h>

#include <cutter/cut-assertions.h>
#include <cutter/cut-multi-process.h>
#include <cutter/cut-helper.h>
#include <cutter/cut-experimental.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SECTION: cutter
 * @title: Cutter
 * @short_description: An Unit Testing Framework for C.
 * @see_also: <link linkend="cutter-cut-assertions">Assertions</link>
 *
 * Cutter is a Unit Testing Framework for C and has the
 * following features:
 * <itemizedlist>
 *   <listitem><para>
 * Easy to use. Cutter doesn't introduce any magic macros
 * like CUTTER_DEFINE_TEST_START and CUTTER_DEFINE_TEST_END
 * into your test program. You can write your test program
 * like normal program. You only use cut_assert_XXX() to
 * check that your program works as you expect.
 * |[
 * #include <cutter.h>
 * #include "my-stack.h"
 *
 * void
 * test_my_stack (void)
 * {
 *     MyStack *stack = my_stack_new();
 *
 *     cut_assert_not_null(stack);
 *     cut_assert(my_stack_is_empty(stack));
 *     cut_assert_equal_int(0, my_stack_get_size(stack));
 *
 *     my_stack_push(stack, 10);
 *     cut_assert(!my_stack_is_empty(stack));
 *     cut_assert_equal_int(1, my_stack_get_size(stack));
 *
 *     my_stack_push(stack, 20);
 *     cut_assert_equal_int(2, my_stack_get_size(stack));
 *
 *     cut_assert_equal(20, my_stack_pop(stack));
 *     cut_assert(!my_stack_is_empty(stack));
 *     cut_assert_equal_int(1, my_stack_get_size(stack));
 *
 *     cut_assert_equal(10, my_stack_pop(stack));
 *     cut_assert(my_stack_is_empty(stack));
 *     cut_assert_equal_int(0, my_stack_get_size(stack));
 * }
 * ]|
 *   </para></listitem>
 *   <listitem><para>
 * Simple but useful output. Cutter works quietly if tests
 * are running without any problems by default. The
 * following is an output of self test:
 * |[
 * ...........................................................
 *
 * Finished in 0.213021 seconds
 *
 * 59 test(s), 246 assertion(s), 0 failure(s), 0 error(s), 0 pending(s), 0 notification(s)
 * ]|
 * Cutter just outputs "." for a passed test and a summary
 * at the end. Cutter doesn't output each test name, how
 * many assertions are used for a test and so because we
 * don't need the information on success.
 *   </para><para>
 * Cutter outputs many information on failure:
 * |[
 * .....................F.....................................
 *
 * 1) Failure: test_error
 * <"Strange" == cut_test_result_get_test_name(result)>
 * expected: &lt;Strange!!!>
 *  but was: &lt;dummy-error-test>
 * test/test-cut-assertions.c:240: cut_assert_test_result()
 *
 * Finished in 0.223657 seconds
 *
 * 59 test(s), 242 assertion(s), 1 failure(s), 0 error(s), 0 pending(s), 0 notification(s)
 * ]|
 * The above result is happened because I add a strange
 * expected staring in Cutter's self test:
 * |[
 * cut_assert_equal_string("Strange!!!", cut_test_result_get_test_name(result));
 * ]|
 * The strange assertion is written in the 240th line in
 * test/test-cut-assertions.c and the line is in the
 * cut_assert_test_result() function. The function is called
 * in test_error test. We expected
 * cut_test_result_get_test_name(result) should return
 * "Strange!!!" but got "dummy-error-name". We can get the
 * above information from Cutter output. This will help your
 * debug.
 *   </para><para>
 * Cutter's output format is pragmatic. ' but was:' is
 * indented and aligned with the above 'expected:'. This
 * helps that you find difference between expected value and
 * actual value by your eyes easily. The problem line is
 * formated as 'FILENAME:LINE: FUNCTION' to integrate
 * Emacs. In Emacs's compilation-mode, if the format is
 * appeared in *compilation* buffer, we can jump to FILENAME
 * at LINE with next-error command. (C-x `) This helps that you
 * find the problem line rapidly.
 *   </para><para>
 * Cutter supports not only easy test writing but also easy
 * debugging.
 *   </para></listitem>
 * </itemizedlist>
  */

/**
 * setup:
 *
 * If you define setup() in your test program, cutter will
 * call your setup() before each your test is run. If you
 * define cut_setup(), setup() is ignored.
 */
void setup(void);

/**
 * cut_setup:
 *
 * If you define cut_setup() in your test program, cutter
 * will call your cut_setup() before each your test is
 * run. cut_setup() has priority over setup().
 *
 * Since: 1.0.6
 */
void cut_setup(void);

/**
 * teardown:
 *
 * If you define teardown() in your test program, cutter will
 * call your teardown() after each your test is run even if
 * a test is failed. If you define cut_teardown(),
 * teardown() is ignored.
 */
void teardown(void);

/**
 * cut_teardown:
 *
 * If you define cut_teardown() in your test program, cutter
 * will call your cut_teardown() after each your test is run
 * even if a test is failed. cut_teardown() has priority
 * over teardown().
 *
 * Since: 1.0.6
 */
void cut_teardown(void);

/**
 * startup:
 *
 * If you define startup() in your test program, cutter will
 * call your startup() before each your test case is run. If
 * you define cut_startup(), startup() is ignored.
 *
 * Since: 0.8
 */
void startup(void);

/**
 * cut_startup:
 *
 * If you define cut_startup() in your test program, cutter
 * will call your cut_startup() before each your test case
 * is run. cut_startup() has priority over startup().
 *
 * Since: 1.0.6
 */
void cut_startup(void);

#ifndef SHUT_RD
/**
 * shutdown:
 *
 * If you define shutdown() in your test program, cutter will
 * call your shutdown() after each your test case is run. If
 * you define cut_shutdown(), shutdown() is ignored.
 *
 * Since: 0.8
 */
void shutdown(void);
#endif

/**
 * cut_shutdown:
 *
 * If you define shutdown() in your test program, cutter
 * will call your shutdown() after each your test case is
 * run.  cut_shutdown() has priority over shutdown().
 *
 * Since: 1.0.6
 */
void cut_shutdown(void);

/**
 * cut_add_data:
 * @first_data_name: The first data name.
 * @...: The data and destroy function of the first data,
 *       followed optionally by more
 *       name/data/destroy_function(#CutDestroyFunction)
 *       triples. The variable arguments should be terminated
 *       by NULL since 1.0.6.
 *
 * Adds data to use data driven test.
 *
 * e.g.:
 * |[
 * #include <cutter.h>
 *
 * void data_translate (void);
 * void test_translate (const void *data);
 *
 * static const char*
 * translate (int input)
 * {
 *    switch(input) {
 *    case 1:
 *        return "first";
 *    case 111:
 *        return "a hundred eleven";
 *    default:
 *        return "unsupported";
 *    }
 * }
 *
 * typedef struct _TranslateTestData
 * {
 *     char *translated;
 *     int input;
 * } TranslateTestData;
 *
 * static TranslateTestData *
 * translate_test_data_new (char *translated, int input)
 * {
 *     TranslateTestData *data;
 *
 *     data = malloc(sizeof(TranslateTestData));
 *     data->translated = strdup(translated);
 *     data->input = input;
 *
 *     return data;
 * }
 *
 * static void
 * translate_test_data_free (TranslateTestData *data)
 * {
 *     free(data->translated);
 *     free(data);
 * }
 *
 * void
 * data_translate(void)
 * {
 *     cut_add_data("simple data",
 *                  translate_test_data_new("first", 1),
 *                  translate_test_data_free,
 *                  "complex data",
 *                  translate_test_data_new("a hundred eleven", 111),
 *                  translate_test_data_free,
 *                  NULL);
 * }
 *
 * void
 * test_translate(const void *data)
 * {
 *      const TranslateTestData *test_data = data;
 *
 *      cut_assert_equal_string(test_data->translated,
 *                              translate(test_data->input));
 * }
 * ]|
 *
 * Since: 1.0.3
 */
#define cut_add_data(first_data_name, ...)                              \
    cut_add_data_backward_compatibility(cut_get_current_test_context(), \
                                        first_data_name, __VA_ARGS__)

#ifdef __GNUC__
#define cut_add_data_backward_compatibility(context, ...)       \
    cut_test_context_add_data(context, ## __VA_ARGS__, NULL)
#else
#define cut_add_data_backward_compatibility(context, ...)       \
    cut_test_context_add_data(context, __VA_ARGS__)
#endif

/**
 * cut_set_attributes:
 * @first_attribute_name: The first attribute name.
 * @...: The value of the first attribute, followed
 *       optionally by more name/value pairs.
 *
 * Sets attributes of the test.
 *
 * e.g.:
 * |[
 * #include <cutter.h>
 *
 * void attributes_repeat (void);
 * void test_repeat (void);
 *
 * void
 * attributes_repeat(void)
 * {
 *     cut_set_attributes("description", "a test for repeat function",
 *                        "bug", "111",
 *                        "priority", "high");
 * }
 *
 * void
 * test_repeat(void)
 * {
 *      cut_assert_equal_string_with_free("XXX", repeat("X", 3));
 * }
 * ]|
 *
 * Since: 1.0.4
 */
#define cut_set_attributes(first_attribute_name, ...)                   \
    cut_test_context_set_attributes(cut_get_current_test_context(),     \
                                    first_attribute_name,               \
                                    ## __VA_ARGS__,                     \
                                    NULL)

/**
 * cut_get_current_test_context:
 *
 * Returns the current test context. The current test
 * context is a thread local object. It means that you don't
 * need to care this if you don't create a new thread in your
 * test. This is only needed to care if you create a new
 * thread. You need to pass the current test context in your
 * test thread to the current test context in a created
 * thread.
 *
 * e.g.:
 * |[
 * int
 * your_thread_function(void *data)
 * {
 *     CutTestContext *test_context = data;
 *     cut_set_current_test_context(test_context);
 *     ...
 * }
 *
 * void
 * run_your_thread(void)
 * {
 *     int result;
 *     pthread_t your_thread;
 *
 *     result = pthread_create(&your_thread, NULL,
 *                             your_thread_function,
 *                             cut_get_current_test_context());
 *     ...
 * }
 * ]|
 *
 * Returns: a #CutTestContext.
 *
 * Since: 1.0.4
 */
#define cut_get_current_test_context()          \
    cut_test_context_current_get()

/**
 * cut_set_current_test_context:
 * @test_context: the #CutTestContext to be the current test
 * context.
 *
 * Set @test_context as the current test context. See
 * cut_get_current_test_context() for more details.
 *
 * Since: 1.0.4
 */
#define cut_set_current_test_context(test_context)      \
    cut_test_context_current_set(test_context)

/**
 * cut_set_message:
 * @format: the message format. See the printf() documentation.
 * @...: the parameters to insert into the format string.
 *
 * Sets a message to be used by the next assertion.
 *
 * Since: 1.0.6
 */
#define cut_set_message(...)                                            \
    cut_test_context_set_user_message(cut_get_current_test_context(),   \
                                      __VA_ARGS__)

/**
 * cut_set_message_va_list:
 * @format: the message format. See the printf() documentation.
 * @args: the parameters to insert into the format string.
 *
 * Sets a message to be used by the next assertion.
 *
 * Since: 1.0.6
 */
#define cut_set_message_va_list(format, args)           \
    cut_test_context_set_user_message_va_list(          \
        cut_get_current_test_context(), format, args)

/**
 * cut_keep_message:
 *
 * Keeps the current message set by cut_set_message() or
 * cut_set_message_va_list() after the next assertion.
 *
 * Since: 1.0.6
 */
#define cut_keep_message(...)                                           \
    cut_test_context_keep_user_message(cut_get_current_test_context())

#ifdef __cplusplus
}
#endif

#endif /* __CUTTER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
