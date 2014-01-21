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

#ifndef __CPPCUT_TEST_H__
#define __CPPCUT_TEST_H__

#include <glib-object.h>

#include <cutter/cut-test.h>

/**
 * SECTION: cppcut-test
 * @title: Test class for C++.
 * @short_description: Adds C++ exception support to CutTest.
 *
 */

G_BEGIN_DECLS

#define CPPCUT_TYPE_TEST                        \
  (cppcut_test_get_type())
#define CPPCUT_TEST(obj)                        \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),            \
                              CPPCUT_TYPE_TEST, \
                              CppCutTest))
#define CPPCUT_TEST_CLASS(klass)                \
  (G_TYPE_CHECK_CLASS_CAST((klass),             \
                           CPPCUT_TYPE_TEST,    \
                           CppCutTestClass))
#define CPPCUT_IS_TEST(obj)                             \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),                    \
                              CPPCUT_TYPE_TEST))
#define CPPCUT_IS_TEST_CLASS(klass)             \
  (G_TYPE_CHECK_CLASS_TYPE((klass),             \
                           CPPCUT_TYPE_TEST))
#define CPPCUT_TEST_GET_CLASS(obj)                      \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                   \
                               CPPCUT_TYPE_TEST,        \
                               CppCutTestClass))

typedef struct _CppCutTest               CppCutTest;
typedef struct _CppCutTestClass          CppCutTestClass;

struct _CppCutTest
{
    CutTest object;
};

struct _CppCutTestClass
{
    CutTestClass parent_class;
    void (*parent_longjmp_function) (CutTest *test, jmp_buf *jump_buffer, int val);
};

GType        cppcut_test_get_type  (void) G_GNUC_CONST;

CppCutTest  *cppcut_test_new       (const gchar    *name,
                                    CutTestFunction function);

G_END_DECLS

#endif /* __CPPCUT_TEST_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
