/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>
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

#include "cut-report.h"
#include "cut-runner.h"
#include "cut-test.h"
#include "cut-test-case.h"
#include "cut-module.h"

static GList *modules = NULL;
static gchar *module_dir = NULL;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

void cut_report_init (void)
{
}

void cut_report_quit (void)
{
    cut_report_unload();
    cut_report_set_default_module_dir(NULL);
}

const gchar *
cut_report_get_default_module_dir (void)
{
    return module_dir;
}

void
cut_report_set_default_module_dir (const gchar *dir)
{
    if (module_dir)
        g_free(module_dir);
    module_dir = NULL;

    if (dir)
        module_dir = g_strdup(dir);
}

static const gchar *
_cut_report_module_dir (void)
{
    const gchar *dir;

    if (module_dir)
        return module_dir;

    dir = g_getenv("CUT_REPORT_MODULE_DIR");
    if (dir)
        return dir;

    return REPORT_MODULEDIR;
}

void
cut_report_load (const gchar *base_dir)
{
    if (!base_dir)
        base_dir = _cut_report_module_dir();

    modules = g_list_concat(cut_module_load_modules(base_dir), modules);
}

static CutModule *
cut_report_load_module (const gchar *name)
{
    CutModule *module;

    module = cut_module_find(modules, name);
    if (module)
        return module;

    module = cut_module_load_module(_cut_report_module_dir(), name);
    if (module) {
        if (g_type_module_use(G_TYPE_MODULE(module))) {
            modules = g_list_prepend(modules, module);
            g_type_module_unuse(G_TYPE_MODULE(module));
        }
    }

    return module;
}

void
cut_report_unload (void)
{
    g_list_foreach(modules, (GFunc)cut_module_unload, NULL);
    g_list_free(modules);
    modules = NULL;
}

GList *
cut_report_get_registered_types (void)
{
    return cut_module_collect_registered_types(modules);
}

GList *
cut_report_get_log_domains (void)
{
    return cut_module_collect_log_domains(modules);
}

#define cut_report_init init
G_DEFINE_ABSTRACT_TYPE (CutReport, cut_report, G_TYPE_OBJECT)
#undef cut_report_init

static void
cut_report_class_init (CutReportClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;
}

static void
init (CutReport *report)
{
}

static void
dispose (GObject *object)
{
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    switch (prop_id) {
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
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutReport *
cut_report_new (const gchar *name, const gchar *first_property, ...)
{
    CutModule *module;
    GObject *report;
    va_list var_args;

    module = cut_report_load_module(name);
    g_return_val_if_fail(module != NULL, NULL);

    va_start(var_args, first_property);
    report = cut_module_instantiate(module, first_property, var_args);
    va_end(var_args);

    return CUT_REPORT(report);
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
