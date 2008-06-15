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

#include <glib.h>
#include <glib/gi18n-lib.h>

#include "cut-utils.h"
#include "cut-ui-factory-builder.h"
#include "cut-module-factory.h"
#include "cut-module-factory-utils.h"

static const gchar *ui_name = NULL;
static CutUIFactoryBuilder *the_builder = NULL;
#ifdef G_OS_WIN32
static gchar *win32_ui_factory_module_dir = NULL;
#endif


static GObject *constructor  (GType                  type,
                              guint                  n_props,
                              GObjectConstructParam *props);

static void         set_option_context (CutFactoryBuilder *builder,
                                        GOptionContext    *context);
static GList       *build              (CutFactoryBuilder *builder);
static GList       *build_all          (CutFactoryBuilder *builder);
static const gchar *get_type_name      (CutFactoryBuilder *builder);

G_DEFINE_TYPE(CutUIFactoryBuilder, cut_ui_factory_builder, CUT_TYPE_FACTORY_BUILDER)

static void
cut_ui_factory_builder_class_init (CutUIFactoryBuilderClass *klass)
{
    GObjectClass *gobject_class;
    CutFactoryBuilderClass *builder_class;

    gobject_class = G_OBJECT_CLASS(klass);
    builder_class = CUT_FACTORY_BUILDER_CLASS(klass);

    gobject_class->constructor = constructor;

    builder_class->set_option_context = set_option_context;
    builder_class->build              = build;
    builder_class->build_all          = build_all;
    builder_class->get_type_name      = get_type_name;
}

static GObject *
constructor (GType type, guint n_props, GObjectConstructParam *props)
{
    GObject *object;

    if (!the_builder) {
        GObjectClass *klass = G_OBJECT_CLASS(cut_ui_factory_builder_parent_class);
        object = klass->constructor(type, n_props, props);
        the_builder = CUT_UI_FACTORY_BUILDER(object);
    } else {
        object = g_object_ref(G_OBJECT(the_builder));
    }

    return object;
}

static void
cut_ui_factory_builder_init (CutUIFactoryBuilder *builder)
{
    const gchar *dir;

    dir = g_getenv("CUT_UI_FACTORY_MODULE_DIR");
    if (!dir) {
#ifdef G_OS_WIN32
        if (!win32_ui_factory_module_dir)
            win32_ui_factory_module_dir =
                cut_win32_build_factory_module_dir_name("ui");
        dir = win32_ui_factory_module_dir;
#else
        dir = UI_FACTORY_MODULEDIR;
#endif
    }

    g_object_set(G_OBJECT(builder),
                 "module-dir", dir,
                 NULL);
}

static void
set_option_context (CutFactoryBuilder *builder, GOptionContext *context)
{
    static gchar *arg_description = NULL;
    GOptionGroup *group;
    GOptionEntry entries[] = {
        {"ui", 'u', 0, G_OPTION_ARG_STRING, &ui_name,
         N_("Specify UI"), NULL},
        {NULL}
    };

    if (!arg_description) {
        GString *available_uis;
        GList *names, *node;

        available_uis = g_string_new("[");
        names = cut_module_factory_get_names("ui");
        for (node = names; node; node = g_list_next(node)) {
            const gchar *name = node->data;
            g_string_append(available_uis, name);
            if (g_list_next(node))
                g_string_append(available_uis, "|");
        }
        g_string_append(available_uis, "]");
        arg_description = g_string_free(available_uis, FALSE);

        g_list_foreach(names, (GFunc)g_free, NULL);
        g_list_free(names);
    }
    entries[0].arg_description = arg_description;

    group = g_option_group_new(("ui"),
                               _("UI Options"),
                               _("Show UI options"),
                               builder, NULL);
    g_option_group_add_entries(group, entries);
    g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);
    g_option_context_add_group(context, group);
}

static GList *
build (CutFactoryBuilder *builder)
{
    GList *factories = NULL;
    CutModuleFactory *module_factory;

    if (!ui_name)
        ui_name = "console";

    if (cut_module_factory_exist_module("ui", ui_name)) {
        GOptionContext *option_context;
        module_factory = cut_module_factory_new("ui", ui_name, NULL);
        g_object_get(builder,
                     "option-context", &option_context,
                     NULL);
        cut_module_factory_set_option_group(module_factory,
                                            option_context);
        factories = g_list_prepend(factories, module_factory);
    }

    return g_list_reverse(factories);
}

static GList *
build_all (CutFactoryBuilder *builder)
{
    GList *factories = NULL, *node;
    GList *factory_names;

    factory_names = cut_module_factory_get_names("ui");

    for (node = factory_names; node; node = g_list_next(node)) {
        CutModuleFactory *module_factory;
        GOptionContext *option_context;
        module_factory = cut_module_factory_new("ui", node->data, NULL);
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
    return "ui";
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
