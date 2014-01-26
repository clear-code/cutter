/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2014  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CPPCUT_TEST_METHODS_H__
#define __CPPCUT_TEST_METHODS_H__

#include <cutter/cut-test.h>

namespace cut
{
    namespace test
    {
        void long_jump (CutTestClass  *cut_test_class,
                        CutTest       *test,
                        jmp_buf       *jump_buffer,
                        gint           value);
        void invoke    (CutTestClass  *cut_test_class,
                        CutTest        *test,
                        CutTestContext *test_context,
                        CutRunContext  *run_context);
    }
}

#endif /* __CPPCUT_TEST_METHODS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
