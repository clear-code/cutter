/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>
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

#include "cut-report.h"
#include "cut-module.h"
#include "cut-listener-utils.h"

#define CUT_REPORT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_REPORT, CutReportPrivate))

typedef struct _CutReportPrivate	CutReportPrivate;
struct _CutReportPrivate
{
    gchar *filename;
};

enum
{
    PROP_0,
    PROP_FILENAME
};

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

G_DEFINE_ABSTRACT_TYPE (CutReport, cut_report, G_TYPE_OBJECT)
CUT_DEFINE_LISTENER_MODULE(report, REPORT)

static void
cut_report_class_init (CutReportClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("filename",
                               "Filename",
                               "The name of output file",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_FILENAME, spec);

    g_type_class_add_private(gobject_class, sizeof(CutReportPrivate));
}

static void
cut_report_init (CutReport *report)
{
    CutReportPrivate *priv = CUT_REPORT_GET_PRIVATE(report);

    priv->filename = NULL;
}

static void
dispose (GObject *object)
{
    CutReportPrivate *priv = CUT_REPORT_GET_PRIVATE(object);

    if (priv->filename) {
        g_free(priv->filename);
        priv->filename = NULL;
    }

    G_OBJECT_CLASS(cut_report_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutReportPrivate *priv = CUT_REPORT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_FILENAME:
        if (priv->filename)
            g_free(priv->filename);
        priv->filename = g_value_dup_string(value);
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
    CutReportPrivate *priv = CUT_REPORT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_FILENAME:
        g_value_set_string(value, priv->filename);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutReport *
cut_report_new (const gchar *name,
                const gchar *first_property, ...)
{
    CutModule *module;
    GObject *report;
    va_list var_args;

    module = cut_report_load_module(name);
    g_return_val_if_fail(module != NULL, NULL);

    va_start(var_args, first_property);
    report = cut_module_instantiate(module,
                                    first_property, var_args);
    va_end(var_args);

    return CUT_REPORT(report);
}

const gchar *
cut_report_get_filename (CutReport *report)
{
    return CUT_REPORT_GET_PRIVATE(report)->filename;
}

#define GET_RESULTS(name)                               \
gchar *                                                 \
cut_report_get_ ## name ## _results (CutReport *report) \
{                                                       \
    CutReportClass *klass;                              \
                                                        \
    g_return_val_if_fail(CUT_IS_REPORT(report), NULL);  \
                                                        \
    klass = CUT_REPORT_GET_CLASS(report);               \
    if (klass->get_ ## name ## _results)                \
        return klass->get_ ## name ## _results(report); \
    else                                                \
        return NULL;                                    \
}

GET_RESULTS(all)
GET_RESULTS(success)
GET_RESULTS(error)
GET_RESULTS(failure)
GET_RESULTS(pending)
GET_RESULTS(notification)

#undef GET_RESULTS

gchar *
cut_report_get_test_result(CutReport *report,
                           const gchar *test_name)
{
    CutReportClass *klass;

    g_return_val_if_fail(CUT_IS_REPORT(report), NULL);
    g_return_val_if_fail(test_name, NULL);

    klass = CUT_REPORT_GET_CLASS(report);
    if (klass->get_test_result)
        return klass->get_test_result(report, test_name);
    else
        return NULL;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
