/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-test-container.h"

#include "cut-test.h"
#include "cut-utils.h"

#define CUT_TEST_CONTAINER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_CONTAINER, CutTestContainerPrivate))

typedef struct _CutTestContainerPrivate	CutTestContainerPrivate;
struct _CutTestContainerPrivate
{
    GList *tests;
};

G_DEFINE_ABSTRACT_TYPE (CutTestContainer, cut_test_container, CUT_TYPE_TEST)

static void dispose        (GObject         *object);

static gdouble  real_get_elapsed  (CutTest  *test);

static void
cut_test_container_class_init (CutTestContainerClass *klass)
{
    GObjectClass *gobject_class;
    CutTestClass *test_class;

    gobject_class = G_OBJECT_CLASS(klass);
    test_class = CUT_TEST_CLASS(klass);

    gobject_class->dispose      = dispose;

    test_class->get_elapsed = real_get_elapsed;

    g_type_class_add_private(gobject_class, sizeof(CutTestContainerPrivate));
}

static void
cut_test_container_init (CutTestContainer *container)
{
    CutTestContainerPrivate *priv = CUT_TEST_CONTAINER_GET_PRIVATE(container);

    priv->tests = NULL;
}

static void
dispose (GObject *object)
{
    CutTestContainerPrivate *priv = CUT_TEST_CONTAINER_GET_PRIVATE(object);

    if (priv->tests) {
        g_list_foreach(priv->tests, (GFunc)g_object_unref, NULL);
        g_list_free(priv->tests);
        priv->tests = NULL;
    }

    G_OBJECT_CLASS(cut_test_container_parent_class)->dispose(object);
}

void
cut_test_container_add_test (CutTestContainer *container, CutTest *test)
{
    CutTestContainerPrivate *priv = CUT_TEST_CONTAINER_GET_PRIVATE(container);

    if (CUT_IS_TEST(test)) {
        g_object_ref(test);
        priv->tests = g_list_prepend(priv->tests, test);
    }
}

GList *
cut_test_container_get_children (CutTestContainer *container)
{
    return CUT_TEST_CONTAINER_GET_PRIVATE(container)->tests;
}

static gdouble
real_get_elapsed (CutTest *test)
{
    gdouble result = 0.0;
    GList *child;
    CutTestContainerPrivate *priv;

    g_return_val_if_fail(CUT_IS_TEST_CONTAINER(test), FALSE);

    priv = CUT_TEST_CONTAINER_GET_PRIVATE(test);
    for (child = priv->tests; child; child = g_list_next(child)) {
        CutTest *test = child->data;

        result += cut_test_get_elapsed(test);
    }

    return result;
}

static inline gboolean
match (GList *regexs, const gchar *name)
{
    GList *node;

    for (node = regexs; node; node = g_list_next(node)) {
        GRegex *regex = node->data;

        if (g_regex_match(regex, name, 0, NULL))
            return TRUE;
    }

    return FALSE;
}

GList *
cut_test_container_filter_children (CutTestContainer *container,
                                    gchar **filter)
{
    GList *original, *node, *regexs;
    GList *matched_tests = NULL;

    g_return_val_if_fail(CUT_IS_TEST_CONTAINER(container), NULL);

    original = (GList *)cut_test_container_get_children(container);
    if (!filter)
        return g_list_copy(original);

    regexs = cut_utils_filter_to_regexs(filter);
    if (!regexs)
        return g_list_copy(original);

    for (node = original; node; node = g_list_next(node)) {
        CutTest *test = node->data;

        if (match(regexs, cut_test_get_name(test)))
            matched_tests = g_list_prepend(matched_tests, test);
    }

    g_list_foreach(regexs, (GFunc)g_regex_unref, NULL);
    g_list_free(regexs);

    return matched_tests;
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
