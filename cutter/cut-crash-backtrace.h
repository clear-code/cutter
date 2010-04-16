/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_CRASH_BACKTRACE_H__
#define __CUT_CRASH_BACKTRACE_H__

#include <setjmp.h>

#include <glib.h>

#include <cutter/cut-test.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-test-suite.h>

G_BEGIN_DECLS

typedef struct _CutCrashBacktrace CutCrashBacktrace;

void               cut_crash_backtrace_set_show_on_the_moment (gboolean show_on_the_moment);
gboolean           cut_crash_backtrace_get_show_on_the_moment (void);
void               cut_crash_backtrace_reset_signal_received  (void);


CutCrashBacktrace *cut_crash_backtrace_new      (jmp_buf *jump_buffer);
void               cut_crash_backtrace_free     (CutCrashBacktrace *crash_backtrace);

void               cut_crash_backtrace_emit     (CutTestSuite    *test_suite,
                                                 CutTestCase     *test_case,
                                                 CutTest         *test,
                                                 CutTestIterator *test_iterator,
                                                 CutTestData     *test_data,
                                                 CutTestContext  *test_context);


G_END_DECLS

#endif /* __CUT_BACKTRACE_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
