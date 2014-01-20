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

#include "cppcut-test-iterator.h"
#include "cppcut-iterated-test.h"

G_BEGIN_DECLS

G_DEFINE_TYPE(CppCutTestIterator, cppcut_test_iterator, CUT_TYPE_TEST_ITERATOR)

static CutIteratedTest *
    create_iterated_test (CutTestIterator         *test_iterator,
                          const gchar             *name,
                          CutIteratedTestFunction  iterated_test_function,
                          CutTestData             *test_data);

static void
cppcut_test_iterator_class_init (CppCutTestIteratorClass *klass)
{
    CutTestIteratorClass *cut_test_iterator_class;

    cut_test_iterator_class = CUT_TEST_ITERATOR_CLASS(klass);

    cut_test_iterator_class->create_iterated_test = create_iterated_test;
}

static void
cppcut_test_iterator_init (CppCutTestIterator *test)
{
}

CppCutTestIterator *
cppcut_test_iterator_new (const gchar *name,
                          CutIteratedTestFunction test_function,
                          CutDataSetupFunction data_setup_function)
{
    gpointer object;

    object = g_object_new(CPPCUT_TYPE_TEST_ITERATOR,
                          "element-name", "test-iterator",
                          "name", name,
                          "iterated-test-function", test_function,
                          "data-setup-function", data_setup_function,
                          NULL);
    return CPPCUT_TEST_ITERATOR(object);
}

static CutIteratedTest *
create_iterated_test (CutTestIterator *test_iterator,
                      const gchar     *name,
                      CutIteratedTestFunction iterated_test_function,
                      CutTestData     *test_data)
{
    CppCutIteratedTest *iterated_test;

    iterated_test = cppcut_iterated_test_new(name,
                                             iterated_test_function,
                                             test_data);
    return CUT_ITERATED_TEST(iterated_test);
}

G_END_DECLS

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
