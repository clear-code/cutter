/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2019  Sutou Kouhei <kou@clear-code.com>
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

#pragma once

/*
 * CentOS 6: GLib 2.28
 * CentOS 7: GLib 2.50
 * Ubuntu 16.04: GLib 2.48
 * Ubuntu 18.04: GLib 2.54
 */
#ifndef GLIB_VERSION_MIN_REQUIRED
#  define GLIB_VERSION_MIN_REQUIRED GLIB_VERSION_2_28
#endif

#include <cutter.h>
#include <gcutter/gcut-assertions.h>
#include <gcutter/gcut-glib-event-loop.h>

/**
 * SECTION: gcutter
 * @title: GCutter
 * @short_description: Cutter with GLib support.
 * @see_also: <link
 *            linkend="cutter-Assertions-with-GLib-support">
 *            Assertions with GLib support</link>
 *
 * GCutter adds many useful features based on GLib to
 * Cutter. If you want to write tests with more easily,
 * it's good idea that you consider GCutter to be used too.
 *
 * It's easy to use GCutter. You just include &lt;gcutter.h&gt;
 * instead of &lt;cutter.h&gt; and use gcutter.pc instead of
 * cutter.pc:
 *
 * test-xxx.c:
 * |[
 * -#include <cutter.h>
 * +#include <gcutter.h>
 * ]|
 *
 * configure.ac:
 * |[
 * -AC_CHECK_CUTTER
 * +AC_CHECK_GCUTTER
 * ]|
 *
 * Makefile.am:
 * |[
 * -XXX_CFLAGS = $(CUTTER_CFLAGS)
 * -XXX_LIBS = $(CUTTER_LIBS)
 * +XXX_CFLAGS = $(GCUTTER_CFLAGS)
 * +XXX_LIBS = $(GCUTTER_LIBS)
 * ]|
 */

/**
 * GCUTTER_ENABLED
 *
 * Defined when GCutter is enabled.
 *
 * Since: 1.0.6
 */
#define GCUTTER_ENABLED 1

/*
vi:nowrap:ai:expandtab:sw=4
*/
