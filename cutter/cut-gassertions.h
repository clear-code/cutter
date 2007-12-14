/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifndef __CUT_GASSERTIONS_H__
#define __CUT_GASSERTIONS_H__

#include <glib.h>

#include <cutter/cut-value-equal.h>
#include <cutter/cut-assertions.h>

G_BEGIN_DECLS

/**
 * SECTION: cut-gassertions
 * @title: Assertions
 * @short_description: Checks that your program works as you
 * expect with GLib support.
 *
 */

/**
 * cut_assert_equal_g_type:
 * @expected: an expected GType.
 * @actual: an actual GType.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 */
#define cut_assert_equal_g_type(expected, actual, ...) do       \
{                                                               \
    GType _expected = (expected);                               \
    GType _actual = (actual);                                   \
    if (_expected == _actual) {                                 \
        cut_test_pass();                                        \
    } else {                                                    \
        cut_test_fail(FAILURE,                                  \
                      "<%s == %s>\n"                            \
                      "expected: <%s>\n"                        \
                      " but was: <%s>",                         \
                      #expected, #actual,                       \
                      g_type_name(_expected),                   \
                      g_type_name(_actual),                     \
                      NULL, ## __VA_ARGS__);                    \
    }                                                           \
} while(0)

/**
 * cut_assert_equal_g_value:
 * @expected: an expected GValue *.
 * @actual: an actual GValue *.
 * @...: optional format string, followed by parameters to insert
 * into the format string (as with printf())
 *
 * Passes if @expected == @actual.
 */
#define cut_assert_equal_g_value(expected, actual, ...) do              \
{                                                                       \
    GValue *_expected = (expected);                                     \
    GValue *_actual = (actual);                                         \
    if (cut_value_equal(expected, actual)) {                            \
        cut_test_pass();                                                \
    } else {                                                            \
        const gchar *inspected_expected, *inspected_actual;             \
        inspected_expected =                                            \
            cut_take_string(g_strdup_value_contents(_expected));        \
        inspected_actual =                                              \
            cut_take_string(g_strdup_value_contents(_actual));          \
        cut_test_fail(FAILURE,                                          \
                      "<%s == %s>\n"                                    \
                      "expected: <%s> (%s)\n"                           \
                      " but was: <%s> (%s)",                            \
                      #expected, #actual,                               \
                      inspected_expected,                               \
                      g_type_name(G_VALUE_TYPE(_expected)),             \
                      inspected_actual,                                 \
                      g_type_name(G_VALUE_TYPE(_actual)),               \
                      NULL, ## __VA_ARGS__);                            \
    }                                                                   \
} while(0)

G_END_DECLS

#endif /* __CUT_GASSERTIONS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
