/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2011-2013  Kouhei Sutou <kou@clear-code.com>
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

#include <gcutter.h>

#if !GLIB_CHECK_VERSION(2, 32, 0)
#  define g_thread_try_new(name, func, data, error) \
    g_thread_create(func, data, TRUE, error)
#endif

void test_failure (void);

static gpointer
fail (gpointer data)
{
    CutTestContext *context = data;
    gint i;

    cut_set_current_test_context(context);

    for (i = 0; i < 10; i++) {
        cut_fail("failed in thread");
    }

    return NULL;
}

void
test_failure (void)
{
    GThread *threads[64];
    gsize i;

    for (i = 0; i < sizeof(threads) / sizeof(*threads); i++) {
        GError *error = NULL;
        threads[i] = g_thread_try_new(NULL,
                                      fail,
                                      cut_get_current_test_context(),
                                      &error);
        gcut_assert_error(error);
    }

    for (i = 0; i < sizeof(threads) / sizeof(*threads); i++) {
        g_thread_join(threads[i]);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
