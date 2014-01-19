/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2014  Kazuhiro Yamato <kz0817@gmail.com>
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <exception>
#include <typeinfo>
#include "cut-helper.h"
#include "cut-test-cpp-helpers.h"

void
cut_test_cpp_safe_invoke (void (*func)(void))
{
    try {
        (*func)();
    } catch (const std::exception &e) {
        cut_test_terminate(ERROR,
                           cut_take_printf("received a C++ exception: <%s> %s",
                                           typeid(e).name(), e.what()));
    } catch (...) {
        cut_test_terminate(ERROR,
                           cut_take_printf("received a C++ exception"));
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
