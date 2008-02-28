/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifndef __CUTTER_H__
#define __CUTTER_H__

#include <cutter/cut-assertions.h>
#include <cutter/cut-hidden-definitions.h>
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
 * Cutter is an Unit Testing Framework for C and has the
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
 * call your setup() before each your test is run.
 */
void setup(void);

/**
 * teardown:
 *
 * If you define teardown() in your test program, cutter will
 * call your teardown() after each your test is run even if
 * a test is failed.
 */
void teardown(void);

/**
 * initialize:
 *
 * If you define initialize() in your test program, cutter will
 * call your initialize() before each your test case is run.
 */
void initialize(void);

/**
 * finalize:
 *
 * If you define finalize() in your test program, cutter will
 * call your finalize() after each your test case is run.
 */
void finalize(void);


#ifdef __cplusplus
}
#endif

#endif /* __CUTTER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
