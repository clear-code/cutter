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
#include "cut-module-factory.h"
#include "cut-report.h"
#include "cut-enum-types.h"

static const gchar **filenames;
static CutReportFactory *the_factory = NULL;

static GObject *constructor  (GType                  type,
                              guint                  n_props,
                              GObjectConstructParam *props);

static void set_option_context (CutListenerFactory *factory,
                                GOptionContext   *context);
static void activate           (CutListenerFactory *factory);

G_DEFINE_TYPE(CutReportFactory, cut_report_factory, CUT_TYPE_LISTENER_FACTORY)

static void
cut_report_factory_class_init (CutReportFactoryClass *klass)
{
    GObjectClass *gobject_class;
    CutListenerFactoryClass *factory_class;

    gobject_class = G_OBJECT_CLASS(klass);
    factory_class = CUT_LISTENER_FACTORY_CLASS(klass);

    gobject_class->constructor = constructor;

    factory_class->set_option_context = set_option_context;
    factory_class->activate           = activate;
}

static GObject *
constructor (GType type, guint n_props, GObjectConstructParam *props)
{
    GObject *object;

    if (!the_factory) {
        GObjectClass *klass = G_OBJECT_CLASS(cut_report_factory_parent_class);
        object = klass->constructor(type, n_props, props);
    } else {
        object = g_object_ref(G_OBJECT(the_factory));
    }

    return object;
}

static void
cut_report_factory_init (CutReportFactory *factory)
{
}

static void
set_option_context (CutListenerFactory *factory, GOptionContext *context)
{
    GOptionEntry entries[] = {
        {"output-files", 'o', 0, G_OPTION_ARG_STRING_ARRAY, &filenames,
         N_("Set filenames of the report"), "OUTPUT_FILENAME"},
        {NULL}
    };

    g_option_context_add_main_entries(context, entries, NULL);
}

static void
activate (CutListenerFactory *factory)
{
    if (!filenames || !*filenames)
        return;

    while (*filenames) {
        gchar *basename, *type;
        CutModuleFactory *module_factory;

        basename = g_path_get_basename(*filenames);
        if (!strchr(basename, '.')) {
            g_free(basename);
            continue; /* skip unspecified type */
        }

        type = g_strdup(strrchr(basename, '.'));
        if (cut_module_factory_exist_module("report", type)) {
            GOptionContext *option_context;
            module_factory = cut_module_factory_new("report", type, NULL);
            g_object_get(factory,
                         "option-context", &option_context,
                         NULL);
            cut_module_factory_set_option_group(module_factory,
                                                option_context);
        }

        g_free(type);
        g_free(basename);
        filenames++;
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
