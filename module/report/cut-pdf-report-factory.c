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
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <gmodule.h>

#ifdef HAVE_GOFFICE
#  include <goffice/goffice.h>
#  include <goffice/app/go-plugin.h>
#  include <goffice/app/go-plugin-loader-module.h>
#endif

#include <cutter/cut-module-impl.h>
#include <cutter/cut-report.h>
#include <cutter/cut-module-factory.h>
#include <cutter/cut-enum-types.h>


#ifdef HAVE_GOFFICE
static gboolean goffice_initialized = FALSE;
static gboolean goffice_shutdowned = FALSE;
#endif

#define CUT_TYPE_PDF_REPORT_FACTORY            cut_type_pdf_factory_report
#define CUT_PDF_REPORT_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_PDF_REPORT_FACTORY, CutPDFReportFactory))
#define CUT_PDF_REPORT_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_PDF_REPORT_FACTORY, CutPDFReportFactoryClass))
#define CUT_IS_PDF_REPORT_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_PDF_REPORT_FACTORY))
#define CUT_IS_PDF_REPORT_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_PDF_REPORT_FACTORY))
#define CUT_PDF_REPORT_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_PDF_REPORT_FACTORY, CutPDFReportFactoryClass))

typedef struct _CutPDFReportFactory CutPDFReportFactory;
typedef struct _CutPDFReportFactoryClass CutPDFReportFactoryClass;

struct _CutPDFReportFactory
{
    CutModuleFactory     object;
    gchar *filename;
};

struct _CutPDFReportFactoryClass
{
    CutModuleFactoryClass parent_class;
};

enum
{
    PROP_0,
    PROP_FILENAME
};

static GType cut_type_pdf_factory_report = 0;
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
init (CutPDFReportFactory *pdf)
{
    pdf->filename = NULL;
}

static void
dispose (GObject *object)
{
    CutPDFReportFactory *pdf = CUT_PDF_REPORT_FACTORY(object);

    if (pdf->filename) {
        g_free(pdf->filename);
        pdf->filename = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutPDFReportFactory *pdf = CUT_PDF_REPORT_FACTORY(object);

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
    CutPDFReportFactory *pdf = CUT_PDF_REPORT_FACTORY(object);

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
            sizeof (CutPDFReportFactoryClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutPDFReportFactory),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_pdf_factory_report =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_MODULE_FACTORY,
                                    "CutPDFReportFactory",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

#ifdef HAVE_GOFFICE
    if (!goffice_initialized) {
        goffice_initialized = TRUE;
        libgoffice_init();
        go_plugins_init(NULL, NULL, NULL, NULL, TRUE,
                        GO_PLUGIN_LOADER_MODULE_TYPE);
    }
#endif

    register_type(type_module);
    if (cut_type_pdf_factory_report)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_pdf_factory_report));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
#ifdef HAVE_GOFFICE
    if (goffice_initialized && !goffice_shutdowned) {
        goffice_shutdowned = TRUE;
        go_plugins_shutdown();
        libgoffice_shutdown();
    }
#endif
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_PDF_REPORT_FACTORY, first_property, var_args);
}

static void
set_option_group (CutModuleFactory *factory, GOptionContext *context)
{
    CutPDFReportFactory *pdf = CUT_PDF_REPORT_FACTORY(factory);
    GOptionGroup *group;
    GOptionEntry entries[] = {
        {NULL}
    };

    if (CUT_MODULE_FACTORY_CLASS(parent_class)->set_option_group)
        CUT_MODULE_FACTORY_CLASS(parent_class)->set_option_group(factory, context);

    group = g_option_group_new(("pdf-report"),
                               _("PDF Report Options"),
                               _("Show PDF report options"),
                               pdf, NULL);
    g_option_group_add_entries(group, entries);
    g_option_group_set_translation_domain(group, GETTEXT_PACKAGE);
    g_option_context_add_group(context, group);
}

static GObject *
create (CutModuleFactory *factory)
{
    CutPDFReportFactory *pdf = CUT_PDF_REPORT_FACTORY(factory);

    return G_OBJECT(cut_report_new("pdf",
                                   "filename", pdf->filename,
                                   NULL));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
