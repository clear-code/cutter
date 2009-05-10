/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@cozmixng.org>
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
#include <gmodule.h>

#ifdef HAVE_IMAGEHLP_H
#  include <windows.h>
#  include <imagehlp.h>
#endif
#include <glib/gstdio.h>

#include "cut-image-hlp-loader.h"

#define CUT_IMAGE_HLP_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_IMAGE_HLP_LOADER, CutImageHlpLoaderPrivate))

typedef struct _CutImageHlpLoaderPrivate	CutImageHlpLoaderPrivate;
struct _CutImageHlpLoaderPrivate
{
    gchar *so_filename;
#ifdef HAVE_IMAGEHLP_H
    IMAGE_DEBUG_INFORMATION *debug_information;
#endif
};

enum
{
    PROP_0,
    PROP_SO_FILENAME
};

G_DEFINE_TYPE(CutImageHlpLoader, cut_image_hlp_loader, G_TYPE_OBJECT)

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
cut_image_hlp_loader_class_init (CutImageHlpLoaderClass *klass)
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

    g_type_class_add_private(gobject_class, sizeof(CutImageHlpLoaderPrivate));
}

static void
cut_image_hlp_loader_init (CutImageHlpLoader *loader)
{
    CutImageHlpLoaderPrivate *priv = CUT_IMAGE_HLP_LOADER_GET_PRIVATE(loader);

    priv->so_filename = NULL;
#ifdef HAVE_IMAGEHLP_H
    priv->debug_information = NULL;
#endif
}

static void
dispose (GObject *object)
{
    CutImageHlpLoaderPrivate *priv = CUT_IMAGE_HLP_LOADER_GET_PRIVATE(object);

    if (priv->so_filename) {
        g_free(priv->so_filename);
        priv->so_filename = NULL;
    }

#ifdef HAVE_IMAGEHLP_H
    if (priv->debug_information) {
        UnmapDebugInformation(priv->debug_information);
        priv->debug_information = NULL;
    }
#endif

    G_OBJECT_CLASS(cut_image_hlp_loader_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutImageHlpLoaderPrivate *priv = CUT_IMAGE_HLP_LOADER_GET_PRIVATE(object);

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
    CutImageHlpLoaderPrivate *priv = CUT_IMAGE_HLP_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_SO_FILENAME:
        g_value_set_string(value, priv->so_filename);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutImageHlpLoader *
cut_image_hlp_loader_new (const gchar *so_filename)
{
    return g_object_new(CUT_TYPE_IMAGE_HLP_LOADER,
                        "so-filename", so_filename,
                        NULL);
}

gboolean
cut_image_hlp_loader_is_dll (CutImageHlpLoader *loader)
{
#ifdef HAVE_IMAGEHLP_H
    CutImageHlpLoaderPrivate *priv;
    gchar *base_name, *directory_name;

    priv = CUT_IMAGE_HLP_LOADER_GET_PRIVATE(loader);
    if (priv->debug_information)
        return TRUE;

    base_name = g_path_get_basename(priv->so_filename);
    directory_name = g_path_get_dirname(priv->so_filename);
    priv->debug_information = MapDebugInformation(NULL,
                                                  base_name, directory_name,
                                                  0);
    g_free(base_name);
    g_free(directory_name);
    if (!priv->debug_information) {
        gchar *error_message;

        error_message = g_win32_error_message(GetLastError());
        g_warning("failed to load DLL: <%s>: %s",
                  priv->so_filename, error_message);
        g_free(error_message);
    }

    /* should check characterize == IMAGE_FILE_DLL? */
    return priv->debug_information != NULL;
#else
    return FALSE;
#endif
}

gboolean
cut_image_hlp_loader_support_attribute (CutImageHlpLoader *loader)
{
#ifdef HAVE_IMAGEHLP_H
    return TRUE;
#else
    return FALSE;
#endif
}

GList *
cut_image_hlp_loader_collect_symbols (CutImageHlpLoader *loader)
{
#ifdef HAVE_IMAGEHLP_H
    CutImageHlpLoaderPrivate *priv;
    IMAGE_DEBUG_INFORMATION *debug_information;
    GList *symbols = NULL;
    DWORD i, last_index;

    priv = CUT_IMAGE_HLP_LOADER_GET_PRIVATE(loader);
    debug_information = priv->debug_information;
    for (i = 0, last_index = 0; i < debug_information->ExportedNamesSize; i++) {
        if (debug_information->ExportedNames[i] == '\0') {
            PSTR name;

            name = debug_information->ExportedNames + last_index;
            symbols = g_list_prepend(symbols, g_strdup(name));
            last_index = i + 1;
        }
    }

    return symbols;
#else
    return NULL;
#endif
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
