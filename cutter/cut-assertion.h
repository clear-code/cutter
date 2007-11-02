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

#include <glib.h>
#include <string.h>
#include <cut.h>

G_BEGIN_DECLS

#define cut_assert(expect)                                  \
if (!expect) {                                              \
    gchar *message;                                         \
    message = g_strdup_printf(                              \
            "%s:%d: (%s)\nfailed>\n",                       \
            __FILE__, __LINE__, __PRETTY_FUNCTION__);       \
    cut_context_set_error(                                  \
            cut_context_get_current(),                      \
            message, __PRETTY_FUNCTION__,                   \
            __FILE__, __LINE__);                            \
    g_free(message);                                        \
} else {                                                    \
    cut_context_increment_assertion_count(                  \
            cut_context_get_current());                     \
}

#define cut_assert_equal_int(expect, actual)                \
if (expect != actual) {                                     \
    gchar *message;                                         \
    message = g_strdup_printf(                              \
            "%s:%d: (%s)\nexpected: <%d>\n but was: <%d>\n",\
            __FILE__, __LINE__, __PRETTY_FUNCTION__,        \
            expect, actual);                                \
    cut_context_set_error(                                  \
            cut_context_get_current(),                      \
            message, __PRETTY_FUNCTION__,                   \
            __FILE__, __LINE__);                            \
    g_free(message);                                        \
} else {                                                    \
    cut_context_increment_assertion_count(                  \
            cut_context_get_current());                     \
}

#define cut_assert_equal_double(expect, error, actual)  \
do {                                                    \
    double _expect = expect;                            \
    double _actual = actual;                            \
    double _error = error;                              \
    if (!(_expect - _error <= _actual &&                \
          _actual <= _expect + _error)) {               \
        gchar *message;                                 \
        message = g_strdup_printf(                      \
            "%s:%d: (%s)\nexpected: <%g +/- %g>\n but was: <%g>\n",\
            __FILE__, __LINE__, __PRETTY_FUNCTION__,    \
            expect, error, actual);                     \
        cut_context_set_error(                          \
            cut_context_get_current(),                  \
            message, __PRETTY_FUNCTION__,               \
            __FILE__, __LINE__);                        \
        g_free(message);                                \
    } else {                                            \
        cut_context_increment_assertion_count(          \
                cut_context_get_current());             \
    }                                                   \
} while(0)

#define cut_assert_equal_string(expect, actual)         \
if (strcmp(expect, actual))           {                 \
    gchar *message;                                         \
    message = g_strdup_printf(                              \
            "%s:%d: (%s)\nexpected: <%s>\n but was: <%s>\n",\
            __FILE__, __LINE__, __PRETTY_FUNCTION__,        \
            expect, actual);                                \
    cut_context_set_error(                                  \
            cut_context_get_current(),                      \
            message, __PRETTY_FUNCTION__,                   \
            __FILE__, __LINE__);                            \
    g_free(message);                                        \
} else {                                                \
    cut_context_increment_assertion_count(              \
            cut_context_get_current());                 \
}

G_END_DECLS

#endif /* __CUT_ASSERTION_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
