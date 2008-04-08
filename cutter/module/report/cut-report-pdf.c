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
#  include <cutter/config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <glib/gstdio.h>
#include <gmodule.h>
#include <cairo.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-report.h>
#include <cutter/cut-runner.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_REPORT_PDF            cut_type_report_xml
#define CUT_REPORT_PDF(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_REPORT_PDF, CutReportPDF))
#define CUT_REPORT_PDF_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_REPORT_PDF, CutReportPDFClass))
#define CUT_IS_REPORT_PDF(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_REPORT_PDF))
#define CUT_IS_REPORT_PDF_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_REPORT_PDF))
#define CUT_REPORT_PDF_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_REPORT_PDF, CutReportPDFClass))

typedef struct _CutReportPDF CutReportPDF;
typedef struct _CutReportPDFClass CutReportPDFClass;

struct _CutReportPDF
{
    CutReport object;
    CutRunner *runner;
    cairo_surface_t *surface;
};

struct _CutReportPDFClass
{
    CutReportClass parent_class;
};

enum
{
    PROP_0,
    PROP_RUNNER
};

static GType cut_type_report_xml = 0;
static CutReportPDFClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void attach_to_runner             (CutListener *listener,
                                          CutRunner   *runner);
static void detach_from_runner           (CutListener *listener,
                                          CutRunner   *runner);

static gboolean result_to_file           (CutReport        *report,
                                          const gchar      *filename,
                                          CutReportFileMode mode);
static gchar   *get_all_results          (CutReport   *report);
static gchar   *get_success_results      (CutReport   *report);
static gchar   *get_error_results        (CutReport   *report);
static gchar   *get_failure_results      (CutReport   *report);
static gchar   *get_pending_results      (CutReport   *report);
static gchar   *get_notification_results (CutReport   *report);
static gchar   *get_test_result          (CutReport   *report,
                                          const gchar *test_name);

static void
class_init (CutReportPDFClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;
    CutListenerClass *listener_class;
    CutReportClass *report_class;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    listener_class = CUT_LISTENER_CLASS(klass);
    report_class = CUT_REPORT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    listener_class->attach_to_runner   = attach_to_runner;
    listener_class->detach_from_runner = detach_from_runner;

    report_class->result_to_file           = result_to_file;
    report_class->get_all_results          = get_all_results;
    report_class->get_success_results      = get_success_results;
    report_class->get_error_results        = get_error_results;
    report_class->get_failure_results      = get_failure_results;
    report_class->get_pending_results      = get_pending_results;
    report_class->get_notification_results = get_notification_results;
    report_class->get_test_result          = get_test_result;

    spec = g_param_spec_object("cut-runner",
                               "CutRunner object",
                               "A CutRunner object",
                               CUT_TYPE_RUNNER,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_RUNNER, spec);
}

static void
init (CutReportPDF *report)
{
    report->runner = NULL;
    report->surface = NULL;
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutReportPDFClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutReportPDF),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_report_xml =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_REPORT,
                                    "CutReportPDF",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_report_xml)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_report_xml));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_REPORT_PDF, first_property, var_args);
}

G_MODULE_EXPORT gchar *
CUT_MODULE_IMPL_GET_LOG_DOMAIN (void)
{
    return g_strdup(G_LOG_DOMAIN);
}

static void
dispose (GObject *object)
{
    CutReportPDF *report = CUT_REPORT_PDF(object);

    if (report->runner) {
        g_object_unref(report->runner);
        report->runner = NULL;
    }
    if (report->surface) {
        cairo_surface_destroy(report->surface);
        report->surface = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutReportPDF *report = CUT_REPORT_PDF(object);

    switch (prop_id) {
      case PROP_RUNNER:
        attach_to_runner(CUT_LISTENER(report), CUT_RUNNER(g_value_get_object(value)));
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
    CutReportPDF *report = CUT_REPORT_PDF(object);

    switch (prop_id) {
      case PROP_RUNNER:
        g_value_set_object(value, G_OBJECT(report->runner));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
cb_ready_test_suite (CutRunner *runner, CutTestSuite *test_suite,
                     guint n_test_cases, guint n_tests,
                     CutReportPDF *report)
{
}

static void
cb_start_test_suite (CutRunner *runner, CutTestSuite *test_suite,
                     CutReportPDF *report)
{
}

static void
cb_start_test_case (CutRunner *runner, CutTestCase *test_case,
                    CutReportPDF *report)
{
}

static void
cb_start_test (CutRunner *runner, CutTest *test, CutTestContext *test_context,
               CutReportPDF *report)
{
}

static void
cb_test_signal (CutRunner      *runner,
                CutTest        *test,
                CutTestContext *test_context,
                CutTestResult  *result,
                CutReportPDF   *report)
{
}

static void
cb_complete_test (CutRunner *runner, CutTest *test,
                  CutTestContext *test_context, CutReportPDF *report)
{
}

static void
cb_complete_test_case (CutRunner *runner, CutTestCase *test_case,
                       CutReportPDF *report)
{
g_warning("hoge");
}

static void
cb_complete_test_suite (CutRunner *runner, CutTestSuite *test_suite,
                        CutReportPDF *report)
{
}

static void
cb_crashed (CutRunner *runner, const gchar *stack_trace,
            CutReportPDF *report)
{
}

static void
connect_to_runner (CutReportPDF *report, CutRunner *runner)
{
#define CONNECT(name) \
    g_signal_connect(runner, #name, G_CALLBACK(cb_ ## name), report)

#define CONNECT_TO_TEST(name) \
    g_signal_connect(runner, #name, G_CALLBACK(cb_test_signal), report)

    CONNECT(ready_test_suite);
    CONNECT(start_test_suite);
    CONNECT(start_test_case);
    CONNECT(start_test);

    CONNECT_TO_TEST(success_test);
    CONNECT_TO_TEST(failure_test);
    CONNECT_TO_TEST(error_test);
    CONNECT_TO_TEST(pending_test);
    CONNECT_TO_TEST(notification_test);

    CONNECT(complete_test);
    CONNECT(complete_test_case);
    CONNECT(complete_test_suite);

    CONNECT(crashed);

#undef CONNECT
}

static void
disconnect_from_runner (CutReportPDF *report, CutRunner *runner)
{
#define DISCONNECT(name)                                               \
    g_signal_handlers_disconnect_by_func(runner,                       \
                                         G_CALLBACK(cb_ ## name),      \
                                         report)

    DISCONNECT(start_test_suite);
    DISCONNECT(start_test_case);
    DISCONNECT(start_test);

    DISCONNECT(complete_test);
    DISCONNECT(complete_test_case);
    DISCONNECT(complete_test_suite);

    DISCONNECT(crashed);

    g_signal_handlers_disconnect_by_func(runner,
                                         G_CALLBACK(cb_test_signal),
                                         report);

#undef DISCONNECT
}

static void
attach_to_runner (CutListener *listener,
                  CutRunner   *runner)
{
    CutReportPDF *report = CUT_REPORT_PDF(listener);
    if (report->runner)
        detach_from_runner(listener, report->runner);
    
    if (runner) {
        report->runner = g_object_ref(runner);
        connect_to_runner(CUT_REPORT_PDF(listener), runner);
    }
    g_warning("hoge");
}

static void
detach_from_runner (CutListener *listener,
                    CutRunner   *runner)
{
    CutReportPDF *report = CUT_REPORT_PDF(listener);
    if (report->runner != runner)
        return;

    disconnect_from_runner(report, runner);
    g_object_unref(report->runner);
    report->runner = NULL;
}

static gboolean
result_to_file (CutReport        *report,
                const gchar      *filename,
                CutReportFileMode mode)
{
    return FALSE;
}

static gchar *
get_all_results (CutReport *report)
{
    return NULL;
}

static gchar *
get_success_results (CutReport *report)
{
    return NULL;
}

static gchar *
get_error_results (CutReport *report)
{
    return NULL;
}

static gchar *
get_failure_results (CutReport *report)
{
    return NULL;
}

static gchar *
get_pending_results (CutReport *report)
{
    return NULL;
}

static gchar *
get_notification_results (CutReport *report)
{
    return NULL;
}

static gchar *
get_test_result (CutReport *report, const gchar *test_name)
{
    return NULL;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
