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

#include "cut-test-context.h"

#define CUT_TEST_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_CONTEXT, CutTestContextPrivate))

typedef struct _CutTestContextPrivate	CutTestContextPrivate;
struct _CutTestContextPrivate
{
    guint assertion_count;
};

enum
{
    PROP_0,
    PROP_ASSERTION_COUNT
};

G_DEFINE_TYPE (CutTestContext, cut_test_context, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void
cut_test_context_class_init (CutTestContextClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_uint("assertion-count",
                             "Assertion Count",
                             "The number of assertion.",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ASSERTION_COUNT, spec);

    g_type_class_add_private(gobject_class, sizeof(CutTestContextPrivate));
}

static void
cut_test_context_init (CutTestContext *context)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(context);

    priv->assertion_count = 0;
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(cut_test_context_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_ASSERTION_COUNT:
        priv->assertion_count = g_value_get_uint(value);
        break;
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
    CutTestContextPrivate *priv = CUT_TEST_CONTEXT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_ASSERTION_COUNT:
        g_value_set_uint(value, priv->assertion_count);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestContext *
cut_test_context_new (void)
{
    return g_object_new(CUT_TYPE_TEST_CONTEXT, NULL);
}

guint
cut_test_context_get_assertion_count (CutTestContext *context)
{
    return CUT_TEST_CONTEXT_GET_PRIVATE(context)->assertion_count;
}

void
cut_test_context_reset_assertion_count (CutTestContext *context)
{
    CUT_TEST_CONTEXT_GET_PRIVATE(context)->assertion_count = 0;
}

void
cut_test_context_increment_assertion_count (CutTestContext *context)
{
    CUT_TEST_CONTEXT_GET_PRIVATE(context)->assertion_count++;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
