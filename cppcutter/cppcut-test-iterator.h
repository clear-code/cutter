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

#ifndef __CPPCUT_TEST_ITERATOR_H__
#define __CPPCUT_TEST_ITERATOR_H__

#include <glib-object.h>

#include <cutter/cut-test-iterator.h>

/**
 * SECTION: cppcut-test-iterator
 * @title: TestIterator class for C++.
 * @short_description: Creates CppCutIteratedTest.
 *
 */

G_BEGIN_DECLS

#define CPPCUT_TYPE_TEST_ITERATOR                        \
  (cppcut_test_iterator_get_type())
#define CPPCUT_TEST_ITERATOR(obj)                        \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),                     \
                              CPPCUT_TYPE_TEST_ITERATOR, \
                              CppCutTestIterator))
#define CPPCUT_TEST_ITERATOR_CLASS(klass)                \
  (G_TYPE_CHECK_CLASS_CAST((klass),                      \
                           CPPCUT_TYPE_TEST_ITERATOR,    \
                           CppCutTestIteratorClass))
#define CPPCUT_IS_TEST_ITERATOR(obj)                    \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),                    \
                              CPPCUT_TYPE_TEST_ITERATOR))
#define CPPCUT_IS_TEST_ITERATOR_CLASS(klass)             \
  (G_TYPE_CHECK_CLASS_TYPE((klass),                      \
                           CPPCUT_TYPE_TEST_ITERATOR))
#define CPPCUT_TEST_ITERATOR_GET_CLASS(obj)                     \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               CPPCUT_TYPE_TEST_ITERATOR,       \
                               CppCutTestIteratorClass))

typedef struct _CppCutTestIterator               CppCutTestIterator;
typedef struct _CppCutTestIteratorClass          CppCutTestIteratorClass;

struct _CppCutTestIterator
{
    CutTestIterator object;
};

struct _CppCutTestIteratorClass
{
    CutTestIteratorClass parent_class;
};

GType        cppcut_test_iterator_get_type  (void) G_GNUC_CONST;

CppCutTestIterator *
             cppcut_test_iterator_new       (const gchar    *name,
                                             CutIteratedTestFunction test_function,
                                             CutDataSetupFunction data_setup_function);

G_END_DECLS

#endif /* __CPPCUT_TEST_ITERATOR_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
