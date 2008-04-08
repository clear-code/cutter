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
#  include <cutter/config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-report.h>
#include <cutter/cut-module-factory.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_REPORT_FACTORY_PDF            cut_type_report_factory_pdf
#define CUT_REPORT_FACTORY_PDF(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_REPORT_FACTORY_PDF, CutReportFactoryPDF))
#define CUT_REPORT_FACTORY_PDF_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_REPORT_FACTORY_PDF, CutReportFactoryPDFClass))
#define CUT_IS_REPORT_FACTORY_PDF(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_REPORT_FACTORY_PDF))
#define CUT_IS_REPORT_FACTORY_PDF_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_REPORT_FACTORY_PDF))
#define CUT_REPORT_FACTORY_PDF_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_REPORT_FACTORY_PDF, CutReportFactoryPDFClass))

typedef struct _CutReportFactoryPDF CutReportFactoryPDF;
typedef struct _CutReportFactoryPDFClass CutReportFactoryPDFClass;

struct _CutReportFactoryPDF
{
    CutModuleFactory     object;
    gchar *filename;
};

struct _CutReportFactoryPDFClass
{
    CutModuleFactoryClass parent_class;
};

enum
{
    PROP_0,
    PROP_FILENAME
};

static GType cut_type_report_factory_pdf = 0;
static CutModuleFactoryClass *parent_class;

static void     dispose          (GObject         *object);
static void     set_property     (GObject         *object,
                                  guint            prop_id,
                                  const GValue    *value,
                                  GParamSpec      *pspec);
static void     get_property     (GObject         *object,
                                  guint            prop_id,
                                  GValue          *value,
                                  GParamSpec      *pspec);
static void     set_option_group (CutModuleFactory *factory,
                                  GOptionContext   *context);
static GObject *create           (CutModuleFactory *factory);

static void
class_init (CutModuleFactoryClass *klass)
{
    CutModuleFactoryClass *factory_class;
    GObjectClass *gobject_class;
    GParamSpec *spec;

    parent_class = g_type_class_peek_parent(klass);
    gobject_class = G_OBJECT_CLASS(klass);
    factory_class  = CUT_MODULE_FACTORY_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    factory_class->set_option_group = set_option_group;
    factory_class->create           = create;

    spec = g_param_spec_string("filename",
                               "Filename",
                               "The name of output file",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_FILENAME, spec);
}

static void
init (CutReportFactoryPDF *pdf)
{
    pdf->filename = NULL;
}

static void
dispose (GObject *object)
{
    CutReportFactoryPDF *pdf = CUT_REPORT_FACTORY_PDF(object);

    if (pdf->filename) {
        g_free(pdf->filename);
        pdf->filename = NULL;
    }
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutReportFactoryPDF *pdf = CUT_REPORT_FACTORY_PDF(object);

    switch (prop_id) {
      case PROP_FILENAME:
        if (pdf->filename)
            g_free(pdf->filename);
        pdf->filename = g_value_dup_string(value);
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
    CutReportFactoryPDF *pdf = CUT_REPORT_FACTORY_PDF(object);

    switch (prop_id) {
      case PROP_FILENAME:
        g_value_set_string(value, pdf->filename);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutReportFactoryPDFClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutReportFactoryPDF),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_report_factory_pdf =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_MODULE_FACTORY,
                                    "CutReportFactoryPDF",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_report_factory_pdf)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_report_factory_pdf));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_REPORT_FACTORY_PDF, first_property, var_args);
}

G_MODULE_EXPORT gchar *
CUT_MODULE_IMPL_GET_LOG_DOMAIN (void)
{
    return g_strdup(G_LOG_DOMAIN);
}

static void
set_option_group (CutModuleFactory *factory, GOptionContext *context)
{
    CutReportFactoryPDF *pdf = CUT_REPORT_FACTORY_PDF(factory);
    GOptionGroup *group;
    GOptionEntry entries[] = {
        {NULL}
    };

    if (CUT_MODULE_FACTORY_CLASS(parent_class)->set_option_group)
        CUT_MODULE_FACTORY_CLASS(parent_class)->set_option_group(factory, context);

    group = g_option_group_new(("report-pdf"),
                               _("PDF Report Options"),
                               _("Show PDF report options"),
                               pdf, NULL);
    g_option_group_add_entries(group, entries);
    g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);
    g_option_context_add_group(context, group);
}

GObject *
create (CutModuleFactory *factory)
{
    CutReportFactoryPDF *pdf = CUT_REPORT_FACTORY_PDF(factory);

    return G_OBJECT(cut_report_new("pdf",
                                   "filename", pdf->filename,
                                   NULL));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
