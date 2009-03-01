/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __GDKCUTTER_PIXBUF_H__
#define __GDKCUTTER_PIXBUF_H__

#include <gcutter.h>
#include <gdkcutter-pixbuf/gdkcut-pixbuf-assertions.h>

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
 * +AC_CHECK_GDKCUTTER_PIXBUF
 * ]|
 *
 * Makefile.am:
 * |[
 * -XXX_CFLAGS = $(CUTTER_CFLAGS)
 * -XXX_LIBS = $(CUTTER_LIBS)
 * +XXX_CFLAGS = $(GDKCUTTER_PIXBUF_CFLAGS)
 * +XXX_LIBS = $(GDKCUTTER_PIXBUF_LIBS)
 * ]|
 */

/**
 * GDKCUTTER_PIXBUF_ENABLED
 *
 * Defined when GdkCutter Pixbuf is enabled.
 *
 * Since: 1.0.6
 */
#define GDKCUTTER_PIXBUF_ENABLED 1

#endif /* __GDKCUTTER_PIXBUF_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
