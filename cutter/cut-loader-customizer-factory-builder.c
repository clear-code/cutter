/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2014-2025  Sutou Kouhei <kou@clear-code.com>
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

#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>

#include "cut-utils.h"
#include "cut-loader-customizer-factory-builder.h"
#include "cut-module-factory.h"
#include "cut-module-factory-utils.h"

static CutLoaderCustomizerFactoryBuilder *the_builder = NULL;
#ifdef G_OS_WIN32
static gchar *win32_loader_customizer_factory_module_dir = NULL;
#endif

static GObject *constructor  (GType                  type,
                              guint                  n_props,
                              GObjectConstructParam *props);

static GList       *build              (CutFactoryBuilder *builder);
static GList       *build_all          (CutFactoryBuilder *builder);
static const gchar *get_type_name      (CutFactoryBuilder *builder);

G_DEFINE_TYPE(CutLoaderCustomizerFactoryBuilder,
              cut_loader_customizer_factory_builder,
              CUT_TYPE_FACTORY_BUILDER)

static void
cut_loader_customizer_factory_builder_class_init (CutLoaderCustomizerFactoryBuilderClass *klass)
{
    GObjectClass *gobject_class;
    CutFactoryBuilderClass *builder_class;

    gobject_class = G_OBJECT_CLASS(klass);
    builder_class = CUT_FACTORY_BUILDER_CLASS(klass);

    gobject_class->constructor = constructor;

    builder_class->build              = build;
    builder_class->build_all          = build_all;
    builder_class->get_type_name      = get_type_name;
}

static GObject *
constructor (GType type, guint n_props, GObjectConstructParam *props)
{
    GObject *object;

    if (!the_builder) {
        GObjectClass *klass;
        klass = G_OBJECT_CLASS(cut_loader_customizer_factory_builder_parent_class);
        object = klass->constructor(type, n_props, props);
        the_builder = CUT_LOADER_CUSTOMIZER_FACTORY_BUILDER(object);
    } else {
        object = g_object_ref(G_OBJECT(the_builder));
    }

    return object;
}

static void
cut_loader_customizer_factory_builder_init (CutLoaderCustomizerFactoryBuilder *builder)
{
    const gchar *dir;

    dir = g_getenv("CUT_LOADER_CUSTOMIZER_FACTORY_MODULE_DIR");
    if (!dir) {
#ifdef G_OS_WIN32
        if (!win32_loader_customizer_factory_module_dir)
            win32_loader_customizer_factory_module_dir =
                cut_win32_build_factory_module_dir_name("loader-cutomizer");
        dir = win32_loader_customizer_factory_module_dir;
#else
        dir = LOADER_CUSTOMIZER_FACTORY_MODULE_DIR;
#endif
    }

    g_object_set(G_OBJECT(builder),
                 "module-dir", dir,
                 NULL);
    cut_module_factory_load(dir, "loader-customizer");
}

static GList *
build (CutFactoryBuilder *builder)
{
    return build_all(builder);
}

static GList *
build_all (CutFactoryBuilder *builder)
{
    GList *factories = NULL, *node;
    GList *factory_names;

    factory_names = cut_module_factory_get_names("loader-customizer");

    for (node = factory_names; node; node = g_list_next(node)) {
        CutModuleFactory *module_factory;
        module_factory = cut_module_factory_new("loader-customizer",
                                                node->data, NULL);
        factories = g_list_prepend(factories, module_factory);
    }

    g_list_foreach(factory_names, (GFunc)g_free, NULL);
    g_list_free(factory_names);

    return g_list_reverse(factories);
}

static const gchar *
get_type_name (CutFactoryBuilder *builder)
{
    return "loader-cutomizer";
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
