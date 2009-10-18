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

#ifndef __CUTPP_ASSERTIONS_H__
#define __CUTPP_ASSERTIONS_H__

#include <cutter++/cut++-assertions-helper.h>

/**
 * SECTION: cut++-assertions
 * @title: Assertions with C++ support
 * @short_description: Checks that your program works as you
 * expect with C++ support.
 *
 */

/**
 * cutpp_assert_equal:
 * @expected: an expected value.
 * @actual: an actual value.
 *
 * Passes if @expected == @actual.
 *
 * Since: 1.0.9
 */
#define cutpp_assert_equal(expected, actual)              \
    cut_trace(cut::assert_equal(expected, actual,         \
                                CUT_STRINGIFY(expected),  \
                                CUT_STRINGIFY(actual)))

#endif /* __CUTPP_ASSERTIONS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
