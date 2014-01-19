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

#include "cppcut-test.h"

#include <cutter.h>

G_BEGIN_DECLS

G_DEFINE_TYPE(CppCutTest, cppcut_test, CUT_TYPE_TEST)

static void         invoke       (CutTest        *test,
                                  CutTestContext *test_context,
                                  CutRunContext  *run_context);

static void
cppcut_test_class_init (CppCutTestClass *klass)
{
    CutTestClass *cut_test_class;

    cut_test_class = CUT_TEST_CLASS(klass);

    cut_test_class->invoke = invoke;
}

static void
cppcut_test_init (CppCutTest *test)
{
}

CppCutTest *
cppcut_test_new (const gchar *name, CutTestFunction function)
{
    gpointer object;

    object = g_object_new(CPPCUT_TYPE_TEST,
                          "element-name", "test",
                          "name", name,
                          "test-function", function,
                          NULL);
    return CPPCUT_TEST(object);
}

static void
invoke (CutTest *test, CutTestContext *test_context, CutRunContext *run_context)
{
    CutTestClass *cut_test_class = CUT_TEST_CLASS(cppcut_test_parent_class);

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

G_END_DECLS

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
