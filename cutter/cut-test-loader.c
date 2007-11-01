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

#include "cut-test-loader.h"

#define CUT_TEST_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_LOADER, CutTestLoaderPrivate))

typedef struct _CutTestLoaderPrivate	CutTestLoaderPrivate;
struct _CutTestLoaderPrivate
{
};

enum
{
    PROP_0
};

G_DEFINE_ABSTRACT_TYPE (CutTestLoader, cut_test_loader, G_TYPE_OBJECT)

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
cut_test_loader_class_init (CutTestLoaderClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    g_type_class_add_private(gobject_class, sizeof(CutTestLoaderPrivate));
}

static void
cut_test_loader_init (CutTestLoader *container)
{
    CutTestLoaderPrivate *priv = CUT_TEST_LOADER_GET_PRIVATE(container);
}

static void
dispose (GObject *object)
{
    CutTestLoaderPrivate *priv = CUT_TEST_LOADER_GET_PRIVATE(object);

    G_OBJECT_CLASS(cut_test_loader_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestLoaderPrivate *priv = CUT_TEST_LOADER_GET_PRIVATE(object);

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
    CutTestLoaderPrivate *priv = CUT_TEST_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestLoader *
cut_test_loader_new (const gchar *soname)
{
    return g_object_new(CUT_TYPE_TEST_LOADER,
                        NULL);
}
/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
