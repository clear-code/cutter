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

#ifndef __CUT_ASSERTIONS_H__
#define __CUT_ASSERTIONS_H__

#include <string.h>
#include <glib.h>

G_BEGIN_DECLS

typedef struct _CutTestContext     CutTestContext;

typedef enum {
    CUT_TEST_RESULT_SUCCESS,
    CUT_TEST_RESULT_FAILURE,
    CUT_TEST_RESULT_ERROR,
    CUT_TEST_RESULT_PENDING
} CutTestResultStatus;

void  cut_test_context_pass_assertion       (CutTestContext *context);
void  cut_test_context_register_result      (CutTestContext *context,
                                             CutTestResultStatus status,
                                             const gchar *function_name,
                                             const gchar *filename,
                                             guint line,
                                             const gchar *message,
                                             ...);

#define cut_test_pass() \
    cut_test_context_pass_assertion(get_current_test_context())

#define cut_test_fail(status, message, ...) do          \
{                                                       \
    cut_test_context_register_result(                   \
        get_current_test_context(),                     \
        CUT_TEST_RESULT_ ## status,                     \
        __PRETTY_FUNCTION__, __FILE__, __LINE__,        \
        message, ## __VA_ARGS__);                       \
    return;                                             \
} while (0)


#define cut_error(format, ...) do                       \
{                                                       \
    cut_test_fail(ERROR, NULL, format, ## __VA_ARGS__); \
    return;                                             \
} while(0)

#define cut_fail(format, ...) do                            \
{                                                           \
    cut_test_fail(FAILURE, NULL, format, ## __VA_ARGS__);   \
} while(0)

#define cut_pending(format, ...) do                         \
{                                                           \
    cut_test_fail(PENDING, NULL, format, ## __VA_ARGS__);   \
} while(0)

#define cut_assert(expected, ...) do                        \
{                                                           \
    if (expected) {                                         \
        cut_test_pass();                                    \
    } else {                                                \
        cut_test_fail(                                      \
            FAILURE,                                        \
            "%s",                                           \
            "expected: <" #expected "> is not TRUE/NULL",   \
            ## __VA_ARGS__, NULL);                          \
    }                                                       \
} while(0)

#define cut_assert_null(actual, ...) do                     \
{                                                           \
    if ((actual) == NULL) {                                 \
        cut_test_pass();                                    \
    } else {                                                \
        cut_test_fail(                                      \
            FAILURE,                                        \
            "%s", "expected: <" #actual "> is NULL",        \
            ## __VA_ARGS__, NULL);                          \
    }                                                       \
} while(0)

#define cut_assert_equal_int(expected, actual, ...) do      \
{                                                           \
    if ((expected) == (actual)) {                           \
        cut_test_pass();                                    \
    } else {                                                \
        cut_test_fail(FAILURE,                              \
                      "<%s == %s>\n"                        \
                      "expected: <%ld>\n but was: <%ld>",   \
                      #expected, #actual,                   \
                      (glong)(expected), (glong)(actual),   \
                      ## __VA_ARGS__, NULL);                \
    }                                                       \
} while(0)

#define cut_assert_equal_double(expected, error, actual, ...) do        \
{                                                                       \
    double _expected = (expected);                                      \
    double _actual = (actual);                                          \
    double _error = (error);                                            \
    if (_expected - _error <= _actual &&                                \
        _actual <= _expected + _error) {                                \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      "<%s-%s <= %s <= %s+%s>\n"                        \
                      "expected: <%g +/- %g>\n but was: <%g>",          \
                      #expected, #error, #actual, #expected, #error,    \
                      _expected, _error, _actual,                       \
                      ## __VA_ARGS__, NULL);                            \
    }                                                                   \
} while(0)

#define cut_assert_equal_string(expected, actual, ...) do               \
{                                                                       \
    const gchar *_expected = (expected);                                \
    const gchar *_actual = (actual);                                    \
    if (_expected && _actual && strcmp(_expected, _actual) == 0) {      \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(FAILURE,                                          \
                      "<%s == %s>\n",                                   \
                      "expected: <%s>\n but was: <%s>",                 \
                      #expected, #actual,                               \
                      _expected, _actual,                               \
                      ## __VA_ARGS__, NULL);                            \
    }                                                                   \
} while(0)

#define cut_assert_operator(lhs, operator, rhs, ...) do                 \
{                                                                       \
    if ((lhs) operator (rhs)) {                                         \
        cut_test_pass();                                                \
    } else {                                                            \
        cut_test_fail(                                                  \
            FAILURE,                                                    \
            "expected: <%s %s %s> is TRUE",                             \
            #lhs, #operator, #rhs,                                      \
            ## __VA_ARGS__, NULL);                                      \
    }                                                                   \
} while(0)


G_END_DECLS

#endif /* __CUT_ASSERTIONS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
