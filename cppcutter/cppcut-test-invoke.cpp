/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2014  Kouhei Sutou <kou@clear-code.com>
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
#endif

#include <exception>
#include <typeinfo>

#include "cppcut-test-invoke.h"

#include <cutter.h>

void
cut::test::invoke (CutTestClass *cut_test_class,
                   CutTest *test,
                   CutTestContext *test_context,
                   CutRunContext *run_context)
{
  try {
    cut_test_class->invoke(test, test_context, run_context);
  } catch (const std::exception &exception) {
    const gchar *message;
    message = cut_take_printf("Unhandled C++ standard exception is thrown: "
                              "<%s>: %s",
                              typeid(exception).name(),
                              exception.what());
    cut_test_terminate(ERROR, message);
  } catch (...) {
    const gchar *message;
    message = "Unhandled C++ non-standard exception is thrown";
    cut_test_terminate(ERROR, message);
  }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
