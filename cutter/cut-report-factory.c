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
#include <glib/gstdio.h>
#include <glib/gi18n-lib.h>

#include "cut-module.h"
#include "cut-report-factory.h"
#include "cut-enum-types.h"

#define CUT_REPORT_FACTORY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_REPORT_FACTORY, CutReportFactoryPrivate))

typedef struct _CutReportFactoryPrivate	CutReportFactoryPrivate;
struct _CutReportFactoryPrivate
{
    const gchar *filename;
};

static void real_set_option_group (CutModuleFactory *factory,
                                   GOptionContext   *context);

G_DEFINE_ABSTRACT_TYPE(CutReportFactory, cut_report_factory, CUT_TYPE_MODULE_FACTORY)

static void
cut_report_factory_class_init (CutReportFactoryClass *klass)
{
    GObjectClass *gobject_class;
    CutModuleFactoryClass *factory_class;

    gobject_class = G_OBJECT_CLASS(klass);
    factory_class = CUT_MODULE_FACTORY_CLASS(klass);

    factory_class->set_option_group = real_set_option_group;

    g_type_class_add_private(gobject_class, sizeof(CutReportFactoryPrivate));
}

static void
cut_report_factory_init (CutReportFactory *factory)
{
    CutReportFactoryPrivate *priv = CUT_REPORT_FACTORY_GET_PRIVATE(factory);

    priv->filename = NULL;
}

CutModuleFactory *
cut_report_factory_new (const gchar *name, const gchar *first_property, ...)
{
    CutModuleFactory *factory;
    va_list var_args;

    va_start(var_args, first_property);
    factory = cut_module_factory_newv("report", name, first_property, var_args);
    va_end(var_args);

    return factory;
}

static void
real_set_option_group (CutModuleFactory *factory, GOptionContext *context)
{
    CutReportFactoryPrivate *priv = CUT_REPORT_FACTORY_GET_PRIVATE(factory);
    GOptionGroup *group;
    GOptionEntry entries[] = {
        {"output-file", 'o', 0, G_OPTION_ARG_STRING, &priv->filename,
         N_("Set filename of the report"), "OUTPUT_FILENAME"},
        {NULL}
    };

    group = g_option_group_new(("report-common"),
                               _("Common Report Options"),
                               _("Show Common Report Options"),
                               factory, NULL);
    g_option_group_add_entries(group, entries);
    g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);
    g_option_context_add_group(context, group);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
