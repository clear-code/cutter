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
 * cppcut_assert:
 * @object: the object to be checked.
 * @...: an optional message. Use cppcut_message() for this.
 *
 * This assertion is a generic method based on template. You
 * can pass any object's reference as @object.
 *
 * Passes if @object is not %NULL.
 *
 * e.g.:
 * |[
 * std::string message("hello");
 * cppcut_assert(message);
 * cppcut_assert(message, cppcut_message("easy expression"));
 * cppcut_assert(message, cppcut_message() << "easy expression"));
 * ]|
 *
 * Since: 1.2.0
 */
#define cppcut_assert(object, ...) do                                   \
{                                                                       \
    cut_trace_with_info_expression(                                     \
        cut_test_with_user_message(                                     \
            cut::assert(object, #object),                               \
            __VA_ARGS__),                                               \
        cppcut_assert(object, __VA_ARGS__));                            \
} while (0)

#endif /* __CPPCUT_ASSERTIONS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
