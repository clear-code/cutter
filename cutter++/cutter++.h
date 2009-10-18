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

#ifndef __CUTTERPP_H__
#define __CUTTERPP_H__

#include <cutter.h>
#include <cutter++/cut++-assertions.h>

/**
 * SECTION: cutter++
 * @title: Cutter++
 * @short_description: Cutter with C++ support.
 * @see_also: <link
 *            linkend="cutter++-cut++-assertions">Assertions
 *            with C++ support</link>
 *
 * Cutter++ provides C++ friendly interface of Cutter. If
 * you want to write tests with C++, it's good idea that you
 * consider Cutter++ to be used too.
 *
 * It's easy to use Cutter++. You just include &lt;cutter++.h&gt;
 * instead of &lt;cutter.h&gt; and use cutter++.pc instead of
 * cutter.pc:
 *
 * test-xxx.cpp:
 * |[
 * -#include <cutter.h>
 * +#include <cutter++.h>
 * ]|
 *
 * configure.ac:
 * |[
 * -AC_CHECK_CUTTER
 * +AC_CHECK_CUTTERPP
 * ]|
 *
 * Makefile.am:
 * |[
 * -XXX_CFLAGS = $(CUTTER_CFLAGS)
 * -XXX_LIBS = $(CUTTER_LIBS)
 * +XXX_CFLAGS = $(CUTTERPP_CFLAGS)
 * +XXX_LIBS = $(CUTTERPP_LIBS)
 * ]|
 *
 * NOTE: You need to extern test functions in C scope. See
 * %CUTPP_BEGIN_TEST_DECLS.
 */

/**
 * CUTTERPP_ENABLED
 *
 * Defined when Cutter++ is enabled.
 *
 * Since: 1.0.9
 */
#define CUTTERPP_ENABLED 1

/**
 * CUTPP_BEGIN_TEST_DECLS
 *
 * Use %CUTPP_BEGIN_TEST_DECLS and %CUTPP_END_TEST_DECLS
 * pair for prototype declarations for test functions:
 *
 * |[
 * CUTPP_BEGIN_TEST_DECLS
 * void test_add ();
 * void test_remove ();
 * CUTPP_END_TEST_DECLS
 * ]|
 *
 * Those macros just surround prototype declarations for
 * test functions with 'extern "C" {...}'.
 *
 * Since: 1.0.9
 */
#define CUTPP_BEGIN_TEST_DECLS extern "C" {

/**
 * CUTPP_END_TEST_DECLS
 *
 * See %CUTPP_BEGIN_TEST_DECLS.
 *
 * Since: 1.0.9
 */
#define CUTPP_END_TEST_DECLS }

#endif /* __CUTTERPP_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
