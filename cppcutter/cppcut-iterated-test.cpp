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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "cppcut-iterated-test.h"
#include "cppcut-test-invoke.h"

G_BEGIN_DECLS

G_DEFINE_TYPE(CppCutIteratedTest, cppcut_iterated_test, CUT_TYPE_ITERATED_TEST)

static void         long_jump    (CutTest        *test,
                                  jmp_buf        *jump_buffer,
                                  gint            value);
static void         invoke       (CutTest        *test,
                                  CutTestContext *test_context,
                                  CutRunContext  *run_context);

static void
cppcut_iterated_test_class_init (CppCutIteratedTestClass *klass)
{
    CutTestClass *cut_test_class;

    cut_test_class = CUT_TEST_CLASS(klass);

    cut_test_class->long_jump = long_jump;
    cut_test_class->invoke    = invoke;
}

static void
cppcut_iterated_test_init (CppCutIteratedTest *test)
{
}

CppCutIteratedTest *
cppcut_iterated_test_new (const gchar *name,
                          CutIteratedTestFunction function,
                          CutTestData *data)
{
    gpointer object;

    object = g_object_new(CPPCUT_TYPE_ITERATED_TEST,
                          "element-name", "iterated-test",
                          "name", name,
                          "iterated-test-function", function,
                          "data", data,
                          NULL);
    return CPPCUT_ITERATED_TEST(object);
}

static void
long_jump (CutTest *test, jmp_buf *jump_buffer, gint value)
{
    CutTestClass *cut_test_class;

    cut_test_class = CUT_TEST_CLASS(cppcut_iterated_test_parent_class);
    cut::test::long_jump(cut_test_class, test, jump_buffer, value);
}

static void
invoke (CutTest *test, CutTestContext *test_context, CutRunContext *run_context)
{
    CutTestClass *cut_test_class;

    cut_test_class = CUT_TEST_CLASS(cppcut_iterated_test_parent_class);
    cut::test::invoke(cut_test_class, test, test_context, run_context);
}

G_END_DECLS

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
