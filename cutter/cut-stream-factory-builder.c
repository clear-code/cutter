/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>

#include "cut-utils.h"
#include "cut-stream-factory-builder.h"
#include "cut-module-factory.h"
#include "cut-module-factory-utils.h"

static const gchar *stream_name = NULL;
static int fd = -1;
static const gchar *log_directory = NULL;
static CutStreamFactoryBuilder *the_builder = NULL;
#ifdef G_OS_WIN32
static gchar *win32_stream_factory_module_dir = NULL;
#endif

#define CUT_STREAM_FACTORY_BUILDER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_STREAM_FACTORY_BUILDER, CutStreamFactoryBuilderPrivate))

typedef struct _CutStreamFactoryBuilderPrivate	CutStreamFactoryBuilderPrivate;
struct _CutStreamFactoryBuilderPrivate
{
    GList *names;
};

static GObject *constructor  (GType                  type,
                              guint                  n_props,
                              GObjectConstructParam *props);
static void     dispose      (GObject               *object);

static void         set_option_context (CutFactoryBuilder *builder,
                                        GOptionContext    *context);
static GList       *build              (CutFactoryBuilder *builder);
static GList       *build_all          (CutFactoryBuilder *builder);
static const gchar *get_type_name      (CutFactoryBuilder *builder);

G_DEFINE_TYPE(CutStreamFactoryBuilder, cut_stream_factory_builder, CUT_TYPE_FACTORY_BUILDER)

static void
cut_stream_factory_builder_class_init (CutStreamFactoryBuilderClass *klass)
{
    GObjectClass *gobject_class;
    CutFactoryBuilderClass *builder_class;

    gobject_class = G_OBJECT_CLASS(klass);
    builder_class = CUT_FACTORY_BUILDER_CLASS(klass);

    gobject_class->constructor = constructor;
    gobject_class->dispose     = dispose;

    builder_class->set_option_context = set_option_context;
    builder_class->build              = build;
    builder_class->build_all          = build_all;
    builder_class->get_type_name      = get_type_name;

    g_type_class_add_private(gobject_class,
                             sizeof(CutStreamFactoryBuilderPrivate));
}

static GObject *
constructor (GType type, guint n_props, GObjectConstructParam *props)
{
    GObject *object;

    if (!the_builder) {
        GObjectClass *klass;
        klass = G_OBJECT_CLASS(cut_stream_factory_builder_parent_class);
        object = klass->constructor(type, n_props, props);
        the_builder = CUT_STREAM_FACTORY_BUILDER(object);
    } else {
        object = g_object_ref(G_OBJECT(the_builder));
    }

    return object;
}

static void
cut_stream_factory_builder_init (CutStreamFactoryBuilder *builder)
{
    CutStreamFactoryBuilderPrivate *priv;
    const gchar *dir;

    priv = CUT_STREAM_FACTORY_BUILDER_GET_PRIVATE(builder);
    priv->names = NULL;

    dir = g_getenv("CUT_STREAM_FACTORY_MODULE_DIR");
    if (!dir) {
#ifdef G_OS_WIN32
        if (!win32_stream_factory_module_dir)
            win32_stream_factory_module_dir =
                cut_win32_build_factory_module_dir_name("stream");
        dir = win32_stream_factory_module_dir;
#else
        dir = STREAM_FACTORY_MODULEDIR;
#endif
    }

    g_object_set(G_OBJECT(builder),
                 "module-dir", dir,
                 NULL);
    cut_module_factory_load(dir, "stream");
}

static void
dispose (GObject *object)
{
    CutStreamFactoryBuilderPrivate *priv;

    priv = CUT_STREAM_FACTORY_BUILDER_GET_PRIVATE(object);

    if (priv->names) {
        g_list_foreach(priv->names, (GFunc)g_free, NULL);
        g_list_free(priv->names);
        priv->names = NULL;
    }


    G_OBJECT_CLASS(cut_stream_factory_builder_parent_class)->dispose(object);
}

static void
set_option_context (CutFactoryBuilder *builder, GOptionContext *context)
{
    static gchar *arg_description = NULL;
    GOptionGroup *group;
    GOptionEntry entries[] = {
        {"stream", 0, 0, G_OPTION_ARG_STRING, &stream_name,
         N_("Specify stream"), NULL},
        {"stream-fd", 0, 0, G_OPTION_ARG_INT, &fd,
         N_("Stream to FILE_DESCRIPTOR (default: stdout)"), "FILE_DESCRIPTOR"},
        {"stream-log-directory", 0, 0, G_OPTION_ARG_STRING, &log_directory,
         N_("Stream to a file under DIRECTORY (default: none)"), "DIRECTORY"},
        {NULL}
    };

    if (!arg_description) {
        GString *available_streams;
        GList *names, *node;

        available_streams = g_string_new("[");
        names = cut_module_factory_get_names("stream");
        for (node = names; node; node = g_list_next(node)) {
            const gchar *name = node->data;
            g_string_append(available_streams, name);
            if (g_list_next(node))
                g_string_append(available_streams, "|");
        }
        g_string_append(available_streams, "]");
        arg_description = g_string_free(available_streams, FALSE);

        g_list_foreach(names, (GFunc)g_free, NULL);
        g_list_free(names);
    }
    entries[0].arg_description = arg_description;

    group = g_option_group_new(("stream"),
                               _("Stream Options"),
                               _("Show stream options"),
                               builder, NULL);
    g_option_group_add_entries(group, entries);
    g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);
    g_option_context_add_group(context, group);
}

static CutModuleFactory *
build_factory (CutFactoryBuilder *builder, const gchar *stream_type,
               const gchar *first_property, ...)
{
    CutModuleFactory *factory = NULL;

    if (cut_module_factory_exist_module("stream", stream_type)) {
        GOptionContext *option_context;
        va_list var_args;
        va_start(var_args, first_property);
        factory = cut_module_factory_new_valist("stream", stream_type,
                                                first_property, var_args);
        va_end(var_args);

        g_object_get(builder, "option-context", &option_context, NULL);
        cut_module_factory_set_option_group(factory, option_context);
    }

    return factory;
}

static GList *
build (CutFactoryBuilder *builder)
{
    GList *factories = NULL;
    CutStreamFactoryBuilderPrivate *priv;
    CutModuleFactory *factory;

    if (!stream_name)
        return NULL;

    priv = CUT_STREAM_FACTORY_BUILDER_GET_PRIVATE(builder);
    factory = build_factory(builder, stream_name,
                            "fd", fd,
                            "log-directory", log_directory,
                            NULL);

    if (factory)
        factories = g_list_prepend(factories, factory);

    return g_list_reverse(factories);
}

static GList *
build_all (CutFactoryBuilder *builder)
{
    GList *factories = NULL, *node;
    GList *factory_names;

    factory_names = cut_module_factory_get_names("stream");

    for (node = factory_names; node; node = g_list_next(node)) {
        CutModuleFactory *module_factory;
        GOptionContext *option_context;
        module_factory = cut_module_factory_new("stream", node->data, NULL);
        g_object_get(builder,
                     "option-context", &option_context,
                     NULL);
        cut_module_factory_set_option_group(module_factory,
                                            option_context);
        factories = g_list_prepend(factories, module_factory);
    }

    return g_list_reverse(factories);
}

static const gchar *
get_type_name (CutFactoryBuilder *builder)
{
    return "stream";
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
