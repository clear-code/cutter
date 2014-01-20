/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2014  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CUT_TEST_ITERATOR_H__
#define __CUT_TEST_ITERATOR_H__

#include <glib-object.h>

#include <cutter/cut-test-container.h>
#include <cutter/cut-iterated-test.h>

G_BEGIN_DECLS

#define CUT_TYPE_TEST_ITERATOR            (cut_test_iterator_get_type ())
#define CUT_TEST_ITERATOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_TEST_ITERATOR, CutTestIterator))
#define CUT_TEST_ITERATOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_TEST_ITERATOR, CutTestIteratorClass))
#define CUT_IS_TEST_ITERATOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_TEST_ITERATOR))
#define CUT_IS_TEST_ITERATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_TEST_ITERATOR))
#define CUT_TEST_ITERATOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_TEST_ITERATOR, CutTestIteratorClass))

typedef void    (*CutDataSetupFunction)(void);

typedef struct _CutTestIteratorClass CutTestIteratorClass;

struct _CutTestIterator
{
    CutTestContainer object;
};

struct _CutTestIteratorClass
{
    CutTestContainerClass parent_class;

    CutIteratedTest *
         (*create_iterated_test)
                          (CutTestIterator *test_iterator,
                           const gchar     *name,
                           CutIteratedTestFunction iterated_test_function,
                           CutTestData     *test_data);
    void (*ready)         (CutTestIterator *test_iterator,
                           guint            n_tests);
    void (*start_iterated_test)
                          (CutTestIterator *test_iterator,
                           CutIteratedTest *iterated_test,
                           CutTestContext  *test_context);
    void (*complete_iterated_test)
                          (CutTestIterator *test_iterator,
                           CutIteratedTest *iterated_test,
                           CutTestContext  *test_context,
                           gboolean         success);
};

GType            cut_test_iterator_get_type    (void) G_GNUC_CONST;

CutTestIterator *cut_test_iterator_new         (const gchar     *name,
                                                CutIteratedTestFunction function,
                                                CutDataSetupFunction data_setup_function);
CutTestIterator *cut_test_iterator_new_empty   (void);

CutIteratedTest *cut_test_iterator_create_iterated_test
                                               (CutTestIterator *test_iterator,
                                                const gchar     *name,
                                                CutIteratedTestFunction iterated_test_function,
                                                CutTestData     *test_data);
void             cut_test_iterator_add_test    (CutTestIterator *test_iterator,
                                                CutIteratedTest *test);

G_END_DECLS

#endif /* __CUT_TEST_ITERATOR_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
