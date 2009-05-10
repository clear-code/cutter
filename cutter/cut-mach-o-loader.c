/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
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
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#ifdef HAVE_MACH_O_LOADER_H
#  include <mach-o/loader.h>
#endif
#include <glib/gstdio.h>

#include "cut-mach-o-loader.h"

#define CUT_MACH_O_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_MACH_O_LOADER, CutMachOLoaderPrivate))

typedef struct _CutMachOLoaderPrivate	CutMachOLoaderPrivate;
struct _CutMachOLoaderPrivate
{
    gchar *so_filename;
};

enum
{
    PROP_0,
    PROP_SO_FILENAME
};

G_DEFINE_TYPE(CutMachOLoader, cut_mach_o_loader, G_TYPE_OBJECT)

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
cut_mach_o_loader_class_init (CutMachOLoaderClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("so-filename",
                               ".so filename",
                               "The filename of shared object",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_SO_FILENAME, spec);

    g_type_class_add_private(gobject_class, sizeof(CutMachOLoaderPrivate));
}

static void
cut_mach_o_loader_init (CutMachOLoader *loader)
{
    CutMachOLoaderPrivate *priv = CUT_MACH_O_LOADER_GET_PRIVATE(loader);

    priv->so_filename = NULL;
}

static void
dispose (GObject *object)
{
    CutMachOLoaderPrivate *priv = CUT_MACH_O_LOADER_GET_PRIVATE(object);

    if (priv->so_filename) {
        g_free(priv->so_filename);
        priv->so_filename = NULL;
    }

    G_OBJECT_CLASS(cut_mach_o_loader_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutMachOLoaderPrivate *priv = CUT_MACH_O_LOADER_GET_PRIVATE(object);

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
    CutMachOLoaderPrivate *priv = CUT_MACH_O_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_SO_FILENAME:
        g_value_set_string(value, priv->so_filename);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutMachOLoader *
cut_mach_o_loader_new (const gchar *so_filename)
{
    return g_object_new(CUT_TYPE_MACH_O_LOADER,
                        "so-filename", so_filename,
                        NULL);
}

gboolean
cut_mach_o_loader_is_mach_o (CutMachOLoader *loader)
{
#ifdef HAVE_MACH_O_LOADER_H
    CutLoaderPrivate *priv;
    FILE *input;
    struct mach_header header;
    gboolean is_mach_o = FALSE;

    priv = CUT_MACH_O_LOADER_GET_PRIVATE(loader);
    input = g_fopen(priv->so_filename);
    if (!input)
        return FALSE;

    if (fread(&header, sizeof(header), 1, input) == 1) {
        is_mach_o = header.magic == MH_MAGIC;
    }

    fclose(input);

    return is_mach_o;
#else
    return FALSE;
#endif
}

gboolean
cut_mach_o_loader_support_attribute (CutMachOLoader *loader)
{
#ifdef HAVE_MACH_O_LOADER_H
    return TRUE;
#else
    return FALSE;
#endif
}

GList *
cut_mach_o_loader_collect_symbols (CutMachOLoader *loader)
{
#ifdef HAVE_MACH_O_LOADER_H
    return NULL;
#else
    return NULL;
#endif
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
