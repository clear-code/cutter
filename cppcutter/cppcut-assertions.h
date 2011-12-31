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

#ifndef __CPPCUT_ASSERTIONS_H__
#define __CPPCUT_ASSERTIONS_H__

#include <cppcutter/cppcut-assertions-helper.h>

/**
 * SECTION: cppcut-assertions
 * @title: Assertions with C++ support
 * @short_description: Checks that your program works as you
 * expect with C++ support.
 *
 */

/**
 * cppcut_assert_equal:
 * @expected: an expected value.
 * @actual: an actual value.
 * @...: an optional message. Use cppcut_message() for this.
 *
 * This assertion is a generic method based on template. You
 * can pass any object's reference as @expected and @actual.
 *
 * Passes if @expected == @actual.
 *
 * e.g.:
 * |[
 * cppcut_assert_equal(3, 1 + 2);
 * cppcut_assert_equal(3, 1 + 2, cppcut_message("easy expression"));
 * cppcut_assert_equal(3, 1 + 2, cppcut_message() << "easy expression"));
 * ]|
 *
 * Since: 1.0.9
 */
#define cppcut_assert_equal(expected, actual, ...) do                   \
{                                                                       \
    cut_trace_with_info_expression(                                     \
        cut_test_with_user_message(                                     \
            cut::assert_equal(expected, actual, #expected, #actual),    \
            __VA_ARGS__),                                               \
        cppcut_assert_equal(expected, actual, __VA_ARGS__));            \
} while (0)

/**
 * cppcut_assert_not_equal:
 * @expected: an expected value.
 * @actual: an actual value.
 * @...: an optional message. Use cppcut_message() for this.
 *
 * This assertion is a generic method based on template. You
 * can pass any object's reference as @expected and @actual.
 *
 * Passes if @expected != @actual.
 *
 * e.g.:
 * |[
 * cppcut_assert_not_equal(3, 3 + 1);
 * cppcut_assert_not_equal(3, 3 + 1, cppcut_message("easy expression"));
 * cppcut_assert_not_equal(3, 3 + 1, cppcut_message() << "easy expression"));
 * ]|
 *
 * Since: 1.2.0
 */
#define cppcut_assert_not_equal(expected, actual, ...) do               \
{                                                                       \
    cut_trace_with_info_expression(                                     \
        cut_test_with_user_message(                                     \
            cut::assert_not_equal(expected, actual,                     \
                                  #expected, #actual),                  \
            __VA_ARGS__),                                               \
        cppcut_assert_not_equal(expected, actual, __VA_ARGS__));        \
} while (0)

/**
 * cppcut_assert_null:
 * @object: the object to be checked.
 * @...: an optional message. Use cppcut_message() for this.
 *
 * This assertion is a generic method based on template. You
 * can pass any object's pointer as @object.
 *
 * Passes if @object is %NULL.
 *
 * e.g.:
 * |[
 * std::string message("hello");
 * std::string *not_null_string = &amp;message;
 * std::string *null_string = NULL;
 * cppcut_assert_null(not_null_string); // fail
 * cppcut_assert_null(null_string);     // pass
 * ]|
 *
 * Since: 1.2.0
 */
#define cppcut_assert_null(object, ...) do                              \
{                                                                       \
    cut_trace_with_info_expression(                                     \
        cut_test_with_user_message(                                     \
            cut::assert_null((object), #object),                        \
            __VA_ARGS__),                                               \
        cppcut_assert_null(object, __VA_ARGS__));                       \
} while (0)

/**
 * cppcut_assert_not_null:
 * @object: the object to be checked.
 * @...: an optional message. Use cppcut_message() for this.
 *
 * This assertion is a generic method based on template. You
 * can pass any object's pointer as @object.
 *
 * Passes if @object is not %NULL.
 *
 * e.g.:
 * |[
 * std::string message("hello");
 * std::string *not_null_string = &amp;message;
 * std::string *null_string = NULL;
 * cppcut_assert_not_null(not_null_string); // pass
 * cppcut_assert_not_null(null_string);     // fail
 * ]|
 *
 * Since: 1.2.0
 */
#define cppcut_assert_not_null(object, ...) do                          \
{                                                                       \
    cut_trace_with_info_expression(                                     \
        cut_test_with_user_message(                                     \
            cut::assert_not_null((object), #object),                    \
            __VA_ARGS__),                                               \
        cppcut_assert_not_null(object, __VA_ARGS__));                   \
} while (0)

/**
 * cppcut_assert_not_null:
 * @lhs: a left hand side value.
 * @operator: a binary operator.
 * @rhs: a right hand side value.
 * @...: an optional message. Use cppcut_message() for this.
 *
 * This assertion is a generic method based on template. You
 * can pass any object as @lhs and @rhs.
 *
 * Passes if (@lhs @operator @rhs) is TRUE.
 *
 * e.g.:
 * |[
 * cppcut_assert_operator(1, <, 2); // pass
 * cppcut_assert_operator(1, >, 2); // fail
 * ]|
 *
 * Since: 1.2.0
 */
#define cppcut_assert_operator(lhs, operator, rhs, ...) do              \
{                                                                       \
    cut_trace_with_info_expression(                                     \
        cut_test_with_user_message(                                     \
            cut::assert_operator((lhs) operator (rhs),                  \
                                 (lhs), (rhs),                          \
                                 #lhs, #operator, #rhs),                \
            __VA_ARGS__),                                               \
        cppcut_assert_operator(lhs, operator, rhs, __VA_ARGS__));       \
} while (0)

#endif /* __CPPCUT_ASSERTIONS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
