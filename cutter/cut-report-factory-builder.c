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

#include "cut-report-factory-builder.h"
#include "cut-module-factory.h"
#include "cut-module-factory-utils.h"

static CutReportFactoryBuilder *the_builder = NULL;

#define CUT_REPORT_FACTORY_BUILDER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_REPORT_FACTORY_BUILDER, CutReportFactoryBuilderPrivate))

typedef struct _CutReportFactoryBuilderPrivate	CutReportFactoryBuilderPrivate;
struct _CutReportFactoryBuilderPrivate
{
    GList *names;
    gchar **filenames;
    GOptionEntry *option_entries;
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

G_DEFINE_TYPE(CutReportFactoryBuilder, cut_report_factory_builder, CUT_TYPE_FACTORY_BUILDER)

static void
cut_report_factory_builder_class_init (CutReportFactoryBuilderClass *klass)
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
                             sizeof(CutReportFactoryBuilderPrivate));
}

static GObject *
constructor (GType type, guint n_props, GObjectConstructParam *props)
{
    GObject *object;

    if (!the_builder) {
        GObjectClass *klass;
        klass = G_OBJECT_CLASS(cut_report_factory_builder_parent_class);
        object = klass->constructor(type, n_props, props);
        the_builder = CUT_REPORT_FACTORY_BUILDER(object);
    } else {
        object = g_object_ref(G_OBJECT(the_builder));
    }

    return object;
}

static void
cut_report_factory_builder_init (CutReportFactoryBuilder *builder)
{
    CutReportFactoryBuilderPrivate *priv;
    const gchar *dir;

    priv = CUT_REPORT_FACTORY_BUILDER_GET_PRIVATE(builder);
    priv->names = NULL;
    priv->filenames = NULL;
    priv->option_entries = NULL;

    dir = g_getenv("CUT_REPORT_FACTORY_MODULE_DIR");
    if (!dir)
        dir = REPORT_FACTORY_MODULEDIR;

    g_object_set(G_OBJECT(builder),
                 "module-dir", dir,
                 NULL);
    cut_module_factory_load(dir, "report");
}

static void
free_option_entries (CutReportFactoryBuilderPrivate *priv, GOptionEntry *entries)
{
    GList *node;
    guint n_reports, i;

    n_reports = g_list_length(priv->names);

    for (i = 0, node = priv->names;
         i < n_reports && node;
         i++, node = g_list_next(node)) {
        g_free((gchar *)entries[i].long_name);
        g_free((gchar *)entries[i].description);
    }
    g_free(entries);
}

static void
dispose (GObject *object)
{
    CutReportFactoryBuilderPrivate *priv;

    priv = CUT_REPORT_FACTORY_BUILDER_GET_PRIVATE(object);

    if (priv->option_entries) {
        free_option_entries(priv, priv->option_entries);
        priv->option_entries = NULL;
    }

    if (priv->names) {
        g_list_foreach(priv->names, (GFunc)g_free, NULL);
        g_list_free(priv->names);
        priv->names = NULL;
    }

    if (priv->filenames) {
        g_strfreev(priv->filenames);
        priv->filenames = NULL;
    }

    G_OBJECT_CLASS(cut_report_factory_builder_parent_class)->dispose(object);
}

static GOptionEntry *
create_option_entries (CutFactoryBuilder *builder)
{
    GList *node;
    guint n_reports, i;
    GOptionEntry *entries;
    CutReportFactoryBuilderPrivate *priv;

    priv = CUT_REPORT_FACTORY_BUILDER_GET_PRIVATE(builder);
    if (!priv->names)
        priv->names = cut_module_factory_get_names("report");
    if (!priv->names)
        return NULL;

    n_reports = g_list_length(priv->names);
    entries = g_new0(GOptionEntry, n_reports + 1);
    priv->filenames = g_new0(gchar*, n_reports + 1);
    priv->filenames[n_reports] = NULL;

    for (i = 0, node = priv->names;
         i < n_reports && node;
         i++, node = g_list_next(node)) {
        const gchar *name = node->data;
        entries[i].long_name = g_strconcat(name, "-report", NULL);
        entries[i].arg = G_OPTION_ARG_STRING;
        entries[i].arg_data = &priv->filenames[i];
        entries[i].description = g_strdup_printf("Set filename of %s report",
                                                 name);
        entries[i].arg_description = "FILE";
    }

    return entries;
}

static void
set_option_context (CutFactoryBuilder *builder, GOptionContext *context)
{
    GOptionGroup *group;
    GOptionEntry *entries;

    group = g_option_group_new(("report"),
                               _("Report Options"),
                               _("Show report options"),
                               builder, NULL);

    entries = create_option_entries(builder);
    if (!entries)
        return;

    g_option_group_add_entries(group, entries);
    g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);
    g_option_context_add_group(context, group);
}

static CutModuleFactory *
build_factory (CutFactoryBuilder *builder, const gchar *report_type,
               const gchar *first_property, ...)
{
    CutModuleFactory *factory = NULL;

    if (cut_module_factory_exist_module("report", report_type)) {
        GOptionContext *option_context;
        va_list var_args;
        va_start(var_args, first_property);
        factory = cut_module_factory_new_valist("report", report_type,
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
    CutReportFactoryBuilderPrivate *priv;
    const gchar **filename;
    GList *node;

    priv = CUT_REPORT_FACTORY_BUILDER_GET_PRIVATE(builder);
    for (filename = (const gchar **)priv->filenames, node = priv->names;
         node;
         filename++, node = g_list_next(node)) {
        CutModuleFactory *factory;
        const gchar *report_type;

        if (!*filename)
            continue;
        report_type = node->data;
        factory = build_factory(builder, report_type,
                                "filename", *filename,
                                NULL);
        if (factory)
            factories = g_list_prepend(factories, factory);
    }

    return g_list_reverse(factories);
}

static GList *
build_all (CutFactoryBuilder *builder)
{
    GList *factories = NULL, *node;
    GList *factory_names;

    factory_names = cut_module_factory_get_names("report");

    for (node = factory_names; node; node = g_list_next(node)) {
        CutModuleFactory *module_factory;
        GOptionContext *option_context;
        module_factory = cut_module_factory_new("report", node->data, NULL);
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
    return "report";
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
