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

#ifndef __CUT_ASSERTION_H__
#define __CUT_ASSERTION_H__

#include <string.h>
#include <glib.h>

G_BEGIN_DECLS

typedef struct _CutContext         CutContext;

typedef enum {
    CUT_TEST_RESULT_SUCCESS,
    CUT_TEST_RESULT_FAILURE,
    CUT_TEST_RESULT_ERROR,
    CUT_TEST_RESULT_PENDING
} CutTestResultStatus;

CutContext *cut_context_get_current (void);
void  cut_context_increment_assertion_count (CutContext *context);
void  cut_context_set_result                (CutContext *context,
                                             CutTestResultStatus status,
                                             const gchar *result_message,
                                             const gchar *function_name,
                                             const gchar *filename,
                                             guint line);

#define cut_error(message) do                   \
{                                               \
    cut_context_set_result(                     \
            cut_context_get_current(),          \
            CUT_TEST_RESULT_ERROR,              \
            message, __PRETTY_FUNCTION__,       \
            __FILE__, __LINE__);                \
    return;                                     \
} while(0)

#define cut_fail(message) do                    \
{                                               \
    cut_context_set_result(                     \
        cut_context_get_current(),              \
        CUT_TEST_RESULT_FAILURE,                \
        message, __PRETTY_FUNCTION__,           \
        __FILE__, __LINE__);                    \
    return;                                     \
} while(0)

#define cut_pending(message) do                 \
{                                               \
    cut_context_set_result(                     \
            cut_context_get_current(),          \
            CUT_TEST_RESULT_PENDING,            \
            message, __PRETTY_FUNCTION__,       \
            __FILE__, __LINE__);                \
    return;                                     \
}

#define cut_assert(expect) do                           \
{                                                       \
    if (!(expect)) {                                    \
        cut_context_set_result(                         \
            cut_context_get_current(),                  \
            CUT_TEST_RESULT_FAILURE,                    \
            "expected: <" #expect "> is not 0/NULL",    \
            __PRETTY_FUNCTION__,                        \
            __FILE__, __LINE__);                        \
        return;                                         \
    } else {                                            \
        cut_context_increment_assertion_count(          \
            cut_context_get_current());                 \
    }                                                   \
} while(0)


#define cut_assert_equal_int(expect, actual) do \
{                                               \
    if (expect != actual) {                     \
        gchar *message;                         \
        message = g_strdup_printf(              \
            "expected: <%d>\n but was: <%d>",   \
            expect, actual);                    \
        cut_context_set_result(                 \
            cut_context_get_current(),          \
            CUT_TEST_RESULT_FAILURE,            \
            message, __PRETTY_FUNCTION__,       \
            __FILE__, __LINE__);                \
        g_free(message);                        \
        return;                                 \
    } else {                                    \
        cut_context_increment_assertion_count(  \
            cut_context_get_current());         \
    }                                           \
} while(0)

#define cut_assert_equal_double(expect, error, actual) do   \
{                                                           \
    double _expect = expect;                                \
    double _actual = actual;                                \
    double _error = error;                                  \
    if (!(_expect - _error <= _actual &&                    \
          _actual <= _expect + _error)) {                   \
        gchar *message;                                     \
        message = g_strdup_printf(                          \
            "expected: <%g +/- %g>\n but was: <%g>",        \
            expect, _error, actual);                        \
        cut_context_set_result(                             \
            cut_context_get_current(),                      \
            CUT_TEST_RESULT_FAILURE,                        \
            message, __PRETTY_FUNCTION__,                   \
            __FILE__, __LINE__);                            \
        g_free(message);                                    \
        return;                                             \
    } else {                                                \
        cut_context_increment_assertion_count(              \
                cut_context_get_current());                 \
    }                                                       \
} while(0)

#define cut_assert_equal_string(expect, actual) do      \
{                                                       \
    if ((!expect && actual) || (expect && !actual) ||   \
        (expect && actual &&strcmp(expect, actual))) {  \
        gchar *message;                                 \
        message = g_strdup_printf(                      \
            "expected: <%s>\n but was: <%s>",           \
            expect, actual);                            \
        cut_context_set_result(                         \
            cut_context_get_current(),                  \
            CUT_TEST_RESULT_FAILURE,                    \
            message, __PRETTY_FUNCTION__,               \
            __FILE__, __LINE__);                        \
        g_free(message);                                \
        return;                                         \
    } else {                                            \
        cut_context_increment_assertion_count(          \
            cut_context_get_current());                 \
    }                                                   \
} while(0)


G_END_DECLS

#endif /* __CUT_ASSERTION_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
