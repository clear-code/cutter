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

#include "cut-test-repository.h"
#include "cut-test-loader.h"

#define CUT_TEST_REPOSITORY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_REPOSITORY, CutTestRepositoryPrivate))

typedef struct _CutTestRepositoryPrivate	CutTestRepositoryPrivate;
struct _CutTestRepositoryPrivate
{
    gchar *dirname;
};

enum
{
    PROP_0,
    PROP_DIRNAME
};

G_DEFINE_TYPE (CutTestRepository, cut_test_repository, G_TYPE_OBJECT)

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
cut_test_repository_class_init (CutTestRepositoryClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("dirname",
                               "Directory name",
                               "The directory name in which stores shared object",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_DIRNAME, spec);

    g_type_class_add_private(gobject_class, sizeof(CutTestRepositoryPrivate));
}

static void
cut_test_repository_init (CutTestRepository *repository)
{
    CutTestRepositoryPrivate *priv = CUT_TEST_REPOSITORY_GET_PRIVATE(repository);

    priv->dirname = NULL;
}

static void
dispose (GObject *object)
{
    CutTestRepositoryPrivate *priv = CUT_TEST_REPOSITORY_GET_PRIVATE(object);

    if (priv->dirname) {
        g_free(priv->dirname);
        priv->dirname = NULL;
    }

    G_OBJECT_CLASS(cut_test_repository_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutTestRepositoryPrivate *priv = CUT_TEST_REPOSITORY_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_DIRNAME:
        if (priv->dirname)
            g_free(priv->dirname);
        priv->dirname = g_value_dup_string(value);
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
    CutTestRepositoryPrivate *priv = CUT_TEST_REPOSITORY_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_DIRNAME:
        g_value_set_string(value, priv->dirname);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestRepository *
cut_test_repository_new (const gchar *dirname)
{
    return g_object_new(CUT_TYPE_TEST_REPOSITORY,
                        "dirname", dirname,
                        NULL);
}

CutTestSuite *
cut_test_repository_create_test_suite (CutTestRepository *repository)
{
    CutTestSuite *suite = NULL;
    GDir *dir;
    const gchar *entry;
    CutTestRepositoryPrivate *priv = CUT_TEST_REPOSITORY_GET_PRIVATE(repository);

    if (!priv->dirname)
        return NULL;

    dir = g_dir_open(priv->dirname, 0, NULL);
    if (!dir)
        return NULL;

    while ((entry = g_dir_read_name(dir))) {
        CutTestLoader *loader;
        CutTestCase *test_case;
        gchar *path_name;

        if (!g_str_has_suffix(entry, "."G_MODULE_SUFFIX))
            continue;

        path_name = g_build_filename(priv->dirname, entry, NULL);
        loader = cut_test_loader_new(path_name);
        g_free(path_name);

        test_case = cut_test_loader_load_test_case(loader);
        if (test_case) {
            if (!suite)
                suite = cut_test_suite_new();
            cut_test_container_add_test(CUT_TEST_CONTAINER(suite),
                                        CUT_TEST(test_case));
        }

        g_object_unref(loader);
    }
    g_dir_close(dir);

    return suite;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
