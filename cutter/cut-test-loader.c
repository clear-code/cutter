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
#include <gmodule.h>

#include "cut-test-loader.h"

#define CUT_TEST_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_LOADER, CutTestLoaderPrivate))

typedef struct _CutTestLoaderPrivate	CutTestLoaderPrivate;
struct _CutTestLoaderPrivate
{
    gchar *so_filename;
    GModule *module;
    CutTestRegisterFunction register_function;
};

enum
{
    PROP_0,
    PROP_SO_FILENAME
};

G_DEFINE_ABSTRACT_TYPE (CutTestLoader, cut_test_loader, G_TYPE_OBJECT)

static GObject *constructor (GType                  type,
                             guint                  n_props,
                             GObjectConstructParam *props);
static void dispose         (GObject               *object);
static void set_property    (GObject               *object,
                             guint                  prop_id,
                             const GValue          *value,
                             GParamSpec            *pspec);
static void get_property    (GObject               *object,
                             guint                  prop_id,
                             GValue                *value,
                             GParamSpec            *pspec);

static void
cut_test_loader_class_init (CutTestLoaderClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->constructor  = constructor;
    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("so-filename",
                               ".so filename",
                               "The filename of shared object",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_SO_FILENAME, spec);

    g_type_class_add_private(gobject_class, sizeof(CutTestLoaderPrivate));
}

static void
cut_test_loader_load (CutTestLoader *loader)
{
    CutTestLoaderPrivate *priv = CUT_TEST_LOADER_GET_PRIVATE(loader);

    if (!priv->so_filename)
        return;

    priv->module = g_module_open(priv->so_filename, G_MODULE_BIND_LAZY);
    if (priv->module) {
        g_module_symbol(priv->module,
                        "cut_test_register",
                        (gpointer)&priv->register_function);
    }
}

static GObject *
constructor (GType type, guint n_props, GObjectConstructParam *props)
{
    GObject *object;
    GObjectClass *klass = G_OBJECT_CLASS(cut_test_loader_parent_class);

    object = klass->constructor(type, n_props, props);

    /* load so file */
    cut_test_loader_load(CUT_TEST_LOADER(object));

    return object;
}

static void
cut_test_loader_init (CutTestLoader *loader)
{
    CutTestLoaderPrivate *priv = CUT_TEST_LOADER_GET_PRIVATE(loader);

    priv->so_filename = NULL;
    priv->register_function = NULL;
}

static void
dispose (GObject *object)
{
    CutTestLoaderPrivate *priv = CUT_TEST_LOADER_GET_PRIVATE(object);

    if (priv->so_filename) {
        g_free(priv->so_filename);
        priv->so_filename = NULL;
    }

    if (priv->module) {
        g_module_close(priv->module);
        priv->module = NULL;
    }

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
      case PROP_SO_FILENAME:
        if (priv->so_filename)
            g_free(priv->so_filename);
        priv->so_filename = g_value_dup_string(value);
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
    CutTestLoaderPrivate *priv = CUT_TEST_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_SO_FILENAME:
        g_value_set_string(value, priv->so_filename);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestLoader *
cut_test_loader_new (const gchar *soname)
{
    return g_object_new(CUT_TYPE_TEST_LOADER,
                        "so-filename", soname,
                        NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
