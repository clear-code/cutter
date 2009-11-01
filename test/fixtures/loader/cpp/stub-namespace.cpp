/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#include <cppcutter.h>

namespace calc
{
    void
    setup(void)
    {
    }

    void
    teardown(void)
    {
    }

    void
    test_add(void)
    {
        cppcut_assert_equal(5, 2 + 3);
    }

    void
    test_sub(void)
    {
        cppcut_assert_equal(2, 3 - 1);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
