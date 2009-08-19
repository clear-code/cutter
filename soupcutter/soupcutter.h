/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Yuto HAYAMIZU <y.hayamizu@gmail.com>
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

#ifndef __SOUPCUTTER_H__
#define __SOUPCUTTER_H__

#include <libsoup/soup.h>
#include <gcutter.h>

/**
 * SECTION: gdkcutter-pixbuf
 * @title: GdkCutter Pixbuf
 * @short_description: Cutter with gdk-pixbuf support.
 * @see_also: <link
 *            linkend="cutter-gdkcut-pixbuf-assertions">Assertions
 *            with gdk-pixbuf support</link>
 *
 * GdkCutter Pixbuf adds many useful features based on
 * gdk-pixbuf to Cutter. If you want to write tests for
 * image, it's good idea that you consider GdkCutter Pixbuf
 * to be used too.
 *
 * It's easy to use GdkCutter Pixbuf. You just include
 * &lt;gdk-cutter-pixbuf.h&gt; instead of &lt;cutter.h&gt;
 * or &lt;gcutter.h&gt; and use gdkcutter-pixbuf.pc instead
 * of cutter.pc or gcutter.pc:
 *
 * test-xxx.c:
 * |[
 * -#include <cutter.h>
 * +#include <gdkcutter-pixbuf.h>
 * ]|
 *
 * configure.ac:
 * |[
 * -AC_CHECK_CUTTER
 * +AC_CHECK_SOUPCUTTER
 * ]|
 *
 * Makefile.am:
 * |[
 * -XXX_CFLAGS = $(CUTTER_CFLAGS)
 * -XXX_LIBS = $(CUTTER_LIBS)
 * +XXX_CFLAGS = $(SOUPCUTTER_CFLAGS)
 * +XXX_LIBS = $(SOUPCUTTER_LIBS)
 * ]|
 */

/**
 * SOUPCUTTER_ENABLED
 *
 * Defined when SoupCutter is enabled.
 *
 * Since: 1.0.8
 */
#define SOUPCUTTER_ENABLED 1

G_BEGIN_DECLS

#define soupcut_assert_equal_content_type(expected, actual) do          \
    {                                                                   \
        cut_trace_with_info_expression(                                 \
            soupcut_assert_equal_content_type_helper(expected, actual,    \
                                        #expected, #actual),                    \
            soupcut_assert_equal_content_type(expected, actual));         \
    } while (0)

void soupcut_assert_equal_content_type_helper (const gchar *expected,
                                               const SoupMessage *actual,
                                               const gchar     *expression_expected,
                                               const gchar     *expression_actual);

G_END_DECLS


#endif /* __SOUPCUTTER_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
