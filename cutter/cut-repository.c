/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#include "cut-repository.h"
#include "cut-loader.h"

#define CUT_REPOSITORY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_REPOSITORY, CutRepositoryPrivate))

typedef struct _CutRepositoryPrivate	CutRepositoryPrivate;
struct _CutRepositoryPrivate
{
    gchar *directory;
    GList *loaders;
};

enum
{
    PROP_0,
    PROP_DIRECTORY
};

G_DEFINE_TYPE (CutRepository, cut_repository, G_TYPE_OBJECT)

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
cut_repository_class_init (CutRepositoryClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("directory",
                               "Directory name",
                               "The directory name in which stores shared object",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_DIRECTORY, spec);

    g_type_class_add_private(gobject_class, sizeof(CutRepositoryPrivate));
}

static void
cut_repository_init (CutRepository *repository)
{
    CutRepositoryPrivate *priv = CUT_REPOSITORY_GET_PRIVATE(repository);

    priv->directory = NULL;
    priv->loaders = NULL;
}

static void
dispose (GObject *object)
{
    CutRepositoryPrivate *priv = CUT_REPOSITORY_GET_PRIVATE(object);

    if (priv->directory) {
        g_free(priv->directory);
        priv->directory = NULL;
    }

    if (priv->loaders) {
        g_list_foreach(priv->loaders, (GFunc)g_object_unref, NULL);
        g_list_free(priv->loaders);
        priv->loaders = NULL;
    }

    G_OBJECT_CLASS(cut_repository_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutRepositoryPrivate *priv = CUT_REPOSITORY_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_DIRECTORY:
        if (priv->directory)
            g_free(priv->directory);
        priv->directory = g_value_dup_string(value);
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
    CutRepositoryPrivate *priv = CUT_REPOSITORY_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_DIRECTORY:
        g_value_set_string(value, priv->directory);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutRepository *
cut_repository_new (const gchar *directory)
{
    return g_object_new(CUT_TYPE_REPOSITORY,
                        "directory", directory,
                        NULL);
}

static void
cut_repository_collect_loader (CutRepository *repository, const gchar *dir_name)
{
    GDir *dir;
    const gchar *entry;
    CutRepositoryPrivate *priv = CUT_REPOSITORY_GET_PRIVATE(repository);

    dir = g_dir_open(dir_name, 0, NULL);
    if (!dir)
        return;

    while ((entry = g_dir_read_name(dir))) {
        CutLoader *loader;
        gchar *path_name;

        path_name = g_build_filename(dir_name, entry, NULL);

        if (g_file_test(path_name, G_FILE_TEST_IS_DIR))
            cut_repository_collect_loader(repository, path_name);

        if (!g_str_has_suffix(entry, "."G_MODULE_SUFFIX)) {
            g_free(path_name);
            continue;
        }

        loader = cut_loader_new(path_name);
        g_free(path_name);

        priv->loaders = g_list_prepend(priv->loaders, loader);
    }
    g_dir_close(dir);
}

CutTestSuite *
cut_repository_create_test_suite (CutRepository *repository)
{
    CutTestSuite *suite = NULL;
    CutRepositoryPrivate *priv = CUT_REPOSITORY_GET_PRIVATE(repository);
    GList *list;

    if (!priv->directory)
        return NULL;

    cut_repository_collect_loader(repository, priv->directory);

    suite = cut_test_suite_new();
    for (list = priv->loaders; list; list = g_list_next(list)) {
        CutLoader *loader = CUT_LOADER(list->data);
        CutTestCase *test_case;

        test_case = cut_loader_load_test_case(loader);
        if (test_case) {
            cut_test_suite_add_test_case(suite, test_case);
            g_object_unref(test_case);
        }
    }
    return suite;
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
