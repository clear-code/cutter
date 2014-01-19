/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2014  Kouhei Sutou <kou@clear-code.com>
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
#include <glib-compatible/glib-compatible.h>

#include "cut-repository.h"
#include "cut-loader.h"
#include "cut-utils.h"

#define CUT_REPOSITORY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_REPOSITORY, CutRepositoryPrivate))

typedef struct _CutRepositoryPrivate	CutRepositoryPrivate;
struct _CutRepositoryPrivate
{
    gchar *directory;
    GList *exclude_files_regexs;
    GList *exclude_dirs_regexs;
    GList *loader_customizers;
    GList *loaders;

    gint deep;
    CutLoader *test_suite_loader;

    gboolean keep_opening_modules;
    gboolean enable_convenience_attribute_definition;
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
    priv->exclude_files_regexs = NULL;
    priv->exclude_dirs_regexs = NULL;
    priv->loader_customizers = NULL;
    priv->deep = 0;
    priv->test_suite_loader = NULL;
    priv->keep_opening_modules = FALSE;
    priv->enable_convenience_attribute_definition = FALSE;
}

static void
free_regexs (GList *regexs)
{
    g_list_foreach(regexs, (GFunc)g_regex_unref, NULL);
    g_list_free(regexs);
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

    if (priv->exclude_files_regexs) {
        free_regexs(priv->exclude_files_regexs);
        priv->exclude_files_regexs = NULL;
    }

    if (priv->exclude_dirs_regexs) {
        free_regexs(priv->exclude_dirs_regexs);
        priv->exclude_dirs_regexs = NULL;
    }

    if (priv->loader_customizers) {
        g_list_foreach(priv->loader_customizers, (GFunc)g_object_unref, NULL);
        g_list_free(priv->loader_customizers);
        priv->loader_customizers = NULL;
    }

    if (priv->test_suite_loader) {
        g_object_unref(priv->test_suite_loader);
        priv->test_suite_loader = NULL;
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

gboolean
cut_repository_get_keep_opening_modules (CutRepository *repository)
{
    return CUT_REPOSITORY_GET_PRIVATE(repository)->keep_opening_modules;
}

void
cut_repository_set_keep_opening_modules (CutRepository *repository,
                                         gboolean keep_opening)
{
    CUT_REPOSITORY_GET_PRIVATE(repository)->keep_opening_modules = keep_opening;
}

gboolean
cut_repository_get_enable_convenience_attribute_definition (CutRepository *repository)
{
    return CUT_REPOSITORY_GET_PRIVATE(repository)->enable_convenience_attribute_definition;
}

void
cut_repository_set_enable_convenience_attribute_definition (CutRepository *repository,
                                                            gboolean enable_convenience_attribute_definition)
{
    CUT_REPOSITORY_GET_PRIVATE(repository)->enable_convenience_attribute_definition = enable_convenience_attribute_definition;
}

static gboolean
is_test_suite_so_path_name (const gchar *path_name)
{
    gboolean is_test_suite_so = FALSE;
    gchar *base_name;

    base_name = g_path_get_basename(path_name);
    if (g_str_has_prefix(base_name, "suite_") ||
        g_str_has_prefix(base_name, "suite-"))
        is_test_suite_so = TRUE;
    g_free(base_name);
    return is_test_suite_so;
}

static void
update_test_suite_loader (CutRepositoryPrivate *priv, CutLoader *loader,
                          gint deep)
{
    if (!priv->test_suite_loader) {
        priv->deep = deep;
        priv->test_suite_loader = g_object_ref(loader);
    }

    if (priv->deep > deep) {
        priv->deep = deep;
        g_object_unref(priv->test_suite_loader);
        priv->test_suite_loader = g_object_ref(loader);
    }
}

static inline gboolean
is_ignore_directory (const gchar *dir_name)
{
    return g_str_equal(dir_name, ".svn") ||
        g_str_equal(dir_name, ".git") ||
        g_str_equal(dir_name, "CVS") ||
        g_str_has_suffix(dir_name, ".dSYM");
}

static inline gchar *
compute_relative_path (GArray *paths)
{
    gchar *last_component = NULL;
    gchar *relative_path;
    gboolean is_ignore_library_directory = FALSE;

    if (paths->len > 0) {
        last_component = g_array_index(paths, gchar *, paths->len - 1);
        if (g_str_equal(last_component, ".libs") ||
            g_str_equal(last_component, "_libs"))
            is_ignore_library_directory = TRUE;
    }

    if (is_ignore_library_directory)
        g_array_index(paths, gchar *, paths->len - 1) = NULL;
    relative_path = g_build_filenamev((gchar **)(paths->data));
    if (is_ignore_library_directory)
        g_array_index(paths, gchar *, paths->len - 1) = last_component;

    return relative_path;
}

static void
cut_repository_collect_loader (CutRepository *repository, const gchar *dir_name,
                               GArray *paths)
{
    GDir *dir;
    const gchar *entry;
    CutRepositoryPrivate *priv = CUT_REPOSITORY_GET_PRIVATE(repository);

    if (is_ignore_directory(dir_name))
        return;

    dir = g_dir_open(dir_name, 0, NULL);
    if (!dir)
        return;

    while ((entry = g_dir_read_name(dir))) {
        gchar *path_name;

        path_name = g_build_filename(dir_name, entry, NULL);
        if (g_file_test(path_name, G_FILE_TEST_IS_DIR)) {
            if (cut_utils_filter_match(priv->exclude_dirs_regexs, entry)) {
                g_free(path_name);
                continue;
            }
            g_array_append_val(paths, entry);
            cut_repository_collect_loader(repository, path_name, paths);
            g_array_remove_index(paths, paths->len - 1);
        } else {
            CutLoader *loader;
            gchar *relative_path;
            GList *node;

            if (cut_utils_filter_match(priv->exclude_files_regexs, entry) ||
                !g_str_has_suffix(entry, "."G_MODULE_SUFFIX)) {
                g_free(path_name);
                continue;
            }

            loader = cut_loader_new(path_name);
            relative_path = compute_relative_path(paths);
            cut_loader_set_base_directory(loader, relative_path);
            cut_loader_set_keep_opening(loader, priv->keep_opening_modules);
            cut_loader_set_enable_convenience_attribute_definition(
                loader, priv->enable_convenience_attribute_definition);
            for (node = priv->loader_customizers; node; node = g_list_next(node)) {
                CutLoaderCustomizer *customizer = node->data;
                cut_loader_customizer_customize(customizer, loader);
            }
            if (is_test_suite_so_path_name(path_name)) {
                update_test_suite_loader(priv, loader, paths->len);
            } else {
                priv->loaders = g_list_prepend(priv->loaders, loader);
            }
            g_free(relative_path);
        }
        g_free(path_name);
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

    if (!priv->loaders) {
        GArray *paths;

        priv->deep = 0;
        paths = g_array_new(TRUE, TRUE, sizeof(gchar *));
        cut_repository_collect_loader(repository, priv->directory, paths);
        g_array_free(paths, TRUE);
    }

    if (priv->test_suite_loader)
        suite = cut_loader_load_test_suite(priv->test_suite_loader);
    if (!suite)
        suite = cut_test_suite_new_empty();

    for (list = priv->loaders; list; list = g_list_next(list)) {
        CutLoader *loader = CUT_LOADER(list->data);
        GList *test_cases, *node;

        test_cases = cut_loader_load_test_cases(loader);
        for (node = test_cases; node; node = g_list_next(node)) {
            CutTestCase *test_case = node->data;

            cut_test_suite_add_test_case(suite, test_case);
            g_object_unref(test_case);
        }
        g_list_free(test_cases);
    }
    return suite;
}

void
cut_repository_set_exclude_files (CutRepository *repository, const gchar **files)
{
    CutRepositoryPrivate *priv = CUT_REPOSITORY_GET_PRIVATE(repository);

    if (priv->exclude_files_regexs)
        free_regexs(priv->exclude_files_regexs);

    if (files)
        priv->exclude_files_regexs = cut_utils_filter_to_regexs(files);
}

void
cut_repository_set_exclude_directories (CutRepository *repository, const gchar **dirs)
{
    CutRepositoryPrivate *priv = CUT_REPOSITORY_GET_PRIVATE(repository);

    if (priv->exclude_dirs_regexs)
        free_regexs(priv->exclude_dirs_regexs);

    if (dirs)
        priv->exclude_dirs_regexs = cut_utils_filter_to_regexs(dirs);
}

void
cut_repository_add_loader_customizer (CutRepository *repository,
                                      CutLoaderCustomizer *customizer)
{
    CutRepositoryPrivate *priv = CUT_REPOSITORY_GET_PRIVATE(repository);

    g_object_ref(customizer);
    priv->loader_customizers =
        g_list_append(priv->loader_customizers, customizer);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
