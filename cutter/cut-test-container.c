/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
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

#define CUT_TEST_CONTAINER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_CONTAINER, CutTestContainerPrivate))

typedef struct _CutTestContainerPrivate	CutTestContainerPrivate;
struct _CutTestContainerPrivate
{
    GList *tests;
};

enum
{
    PROP_0
};

G_DEFINE_ABSTRACT_TYPE (CutTestContainer, cut_test_container, CUT_TYPE_TEST)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static gboolean real_run   (CutTest         *test);

static void
cut_test_container_class_init (CutTestContainerClass *klass)
{
    GObjectClass *gobject_class;
    CutTestClass *test_class;

    gobject_class = G_OBJECT_CLASS(klass);
    test_class = CUT_TEST_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    test_class->run = real_run;

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

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

void
cut_test_container_add_test (CutTestContainer *container, CutTest *test)
{
    CutTestContainerPrivate *priv = CUT_TEST_CONTAINER_GET_PRIVATE(container);

    if (CUT_IS_TEST(test)) {
        priv->tests = g_list_prepend(priv->tests, test);
    }
}

const GList *
cut_test_container_get_children (CutTestContainer *container)
{
    return CUT_TEST_CONTAINER_GET_PRIVATE(container)->tests;
}

static gboolean
real_run (CutTest *test)
{
    GList *list;
    gboolean all_success = TRUE;
    CutTestContainerPrivate *priv;

    g_return_val_if_fail(CUT_IS_TEST_CONTAINER(test), FALSE);

    priv = CUT_TEST_CONTAINER_GET_PRIVATE(test);

    g_signal_emit_by_name(test, "start");
    for (list = priv->tests; list; list = g_list_next(list)) {
        if (!list->data)
            continue;
        if (CUT_IS_TEST(list->data)) {
            CutTest *test = CUT_TEST(list->data);
            if (!cut_test_run(test))
                all_success = FALSE;
        } else {
            g_warning("This object is neither test nor test container!");
        }
    }

    if (all_success) {
        g_signal_emit_by_name(test, "success");
    } else {
        g_signal_emit_by_name(test, "failure");
    }

    g_signal_emit_by_name(test, "complete");

    return all_success;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
