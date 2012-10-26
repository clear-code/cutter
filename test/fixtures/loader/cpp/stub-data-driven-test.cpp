/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2012  Kouhei Sutou <kou@clear-code.com>
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
#include <cppcutter.h>

namespace data_driven_test
{
    CUT_EXPORT void
    data_string(void)
    {
        gcut_add_datum("ASCII",
                       "string", G_TYPE_STRING, "ASCII",
                       NULL);
    }

    CUT_EXPORT void
    test_string(const gpointer data)
    {
        const gchar *string = gcut_data_get_string(data, "string");
        cppcut_assert_equal("ASCII", string);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
