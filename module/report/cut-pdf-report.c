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
#include <math.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <glib/gstdio.h>
#include <gmodule.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-report.h>
#include <cutter/cut-listener.h>
#include <cutter/cut-run-context.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-enum-types.h>

#include "cut-cairo.h"
#include "cut-cairo-pie-chart.h"

#define CUT_TYPE_PDF_REPORT            cut_type_pdf_report
#define CUT_PDF_REPORT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_PDF_REPORT, CutPDFReport))
#define CUT_PDF_REPORT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_PDF_REPORT, CutPDFReportClass))
#define CUT_IS_PDF_REPORT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_PDF_REPORT))
#define CUT_IS_PDF_REPORT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_PDF_REPORT))
#define CUT_PDF_REPORT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_PDF_REPORT, CutPDFReportClass))

#define A4_WIDTH 596
#define A4_HEIGHT 842

typedef struct _CutPDFReport CutPDFReport;
typedef struct _CutPDFReportClass CutPDFReportClass;

struct _CutPDFReport
{
    CutReport object;
    CutRunContext *run_context;
    cairo_t *context;
};

struct _CutPDFReportClass
{
    CutReportClass parent_class;
};

enum
{
    PROP_0,
    PROP_RUN_CONTEXT
};

static GType cut_type_pdf_report = 0;
static CutPDFReportClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void attach_to_run_context             (CutListener *listener,
                                          CutRunContext   *run_context);
static void detach_from_run_context           (CutListener *listener,
                                          CutRunContext   *run_context);

static gchar   *get_all_results          (CutReport   *report);
static gchar   *get_success_results      (CutReport   *report);
static gchar   *get_error_results        (CutReport   *report);
static gchar   *get_failure_results      (CutReport   *report);
static gchar   *get_pending_results      (CutReport   *report);
static gchar   *get_notification_results (CutReport   *report);
static gchar   *get_test_result          (CutReport   *report,
                                          const gchar *test_name);

static void
class_init (CutPDFReportClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;
    CutReportClass *report_class;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    report_class = CUT_REPORT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    report_class->get_all_results          = get_all_results;
    report_class->get_success_results      = get_success_results;
    report_class->get_error_results        = get_error_results;
    report_class->get_failure_results      = get_failure_results;
    report_class->get_pending_results      = get_pending_results;
    report_class->get_notification_results = get_notification_results;
    report_class->get_test_result          = get_test_result;

    spec = g_param_spec_object("cut-run-context",
                               "CutRunContext object",
                               "A CutRunContext object",
                               CUT_TYPE_RUN_CONTEXT,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_RUN_CONTEXT, spec);
}

static void
init (CutPDFReport *report)
{
    report->run_context = NULL;
    report->context = NULL;
}

static void
listener_init (CutListenerClass *listener)
{
    listener->attach_to_run_context   = attach_to_run_context;
    listener->detach_from_run_context = detach_from_run_context;
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutPDFReportClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutPDFReport),
            0,
            (GInstanceInitFunc) init,
        };

    static const GInterfaceInfo listener_info =
        {
            (GInterfaceInitFunc) listener_init,
            NULL,
            NULL
        };

    cut_type_pdf_report =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_REPORT,
                                    "CutPDFReport",
                                    &info, 0);
    g_type_module_add_interface(type_module,
                                cut_type_pdf_report,
                                CUT_TYPE_LISTENER,
                                &listener_info);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (CUT_TYPE_PDF_REPORT)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(CUT_TYPE_PDF_REPORT));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_PDF_REPORT, first_property, var_args);
}

static void
dispose (GObject *object)
{
    CutPDFReport *report = CUT_PDF_REPORT(object);

    if (report->run_context) {
        g_object_unref(report->run_context);
        report->run_context = NULL;
    }
    if (report->context) {
        cairo_destroy(report->context);
        report->context = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutPDFReport *report = CUT_PDF_REPORT(object);

    switch (prop_id) {
      case PROP_RUN_CONTEXT:
        attach_to_run_context(CUT_LISTENER(report), CUT_RUN_CONTEXT(g_value_get_object(value)));
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
    CutPDFReport *report = CUT_PDF_REPORT(object);

    switch (prop_id) {
      case PROP_RUN_CONTEXT:
        g_value_set_object(value, G_OBJECT(report->run_context));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
init_page (cairo_t *cr)
{
    cairo_move_to(cr, 10, 10);
}

static void
cb_ready_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                     guint n_test_cases, guint n_tests,
                     CutPDFReport *report)
{
    const gchar *filename;
    cairo_surface_t *surface;

    filename = cut_report_get_filename(CUT_REPORT(report));
    if (!filename)
        return;

    if (report->context) {
        cairo_destroy(report->context);
        report->context = NULL;
    }

    surface = cairo_pdf_surface_create(filename, A4_WIDTH, A4_HEIGHT);
    report->context = cairo_create(surface);
    cairo_surface_destroy(surface);

    init_page(report->context);
}

static PangoLayout *
show_text_with_page_feed (cairo_t *cr, const gchar *utf8)
{
    PangoLayout *layout;
    double x, y;
    int width, height;

    if (!utf8)
        return NULL;

    layout = cut_cairo_create_pango_layout(cr, utf8, 10);
    if (!layout)
        return NULL;

    cairo_get_current_point(cr, &x, &y);
    pango_layout_get_pixel_size(layout, &width, &height);

    if (A4_HEIGHT < y + height) {
        cairo_show_page(cr);
        init_page(cr);
        cairo_get_current_point(cr, NULL, &y);
        cairo_move_to(cr, x, y);
    }

    pango_cairo_show_layout(cr, layout);
    return layout;
}

static void
cb_start_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                     CutPDFReport *report)
{
}

static void
cb_start_test_case (CutRunContext *run_context, CutTestCase *test_case,
                    CutPDFReport *report)
{
}

static void
cb_start_test (CutRunContext *run_context, CutTest *test, CutTestContext *test_context,
               CutPDFReport *report)
{
}

static void
cb_test_signal (CutRunContext      *run_context,
                CutTest        *test,
                CutTestContext *test_context,
                CutTestResult  *result,
                CutPDFReport   *report)
{
}

static void
cb_complete_test (CutRunContext *run_context, CutTest *test,
                  CutTestContext *test_context, CutPDFReport *report)
{
}

static void
cb_complete_test_case (CutRunContext *run_context, CutTestCase *test_case,
                       CutPDFReport *report)
{
}

static void
show_test_case (cairo_t *cr, CutTestCase *test_case, CutTestResultStatus status,
                guint n_tests, guint n_successes, guint n_failures,
                guint n_errors, guint n_pendings, guint n_omissions)
{
    PangoLayout *layout;
    const gchar *utf8;
    int width, height;
    gdouble x, y;

    utf8 = cut_test_get_name(CUT_TEST(test_case));
    layout = show_text_with_page_feed(cr, utf8);
    if (!layout)
        return;

    cairo_get_current_point(cr, &x, &y);
    cairo_save(cr);
    pango_layout_get_pixel_size(layout, &width, &height);
    cairo_rectangle(cr, A4_WIDTH - 100 - 10, y + 2, 100, height - 4);
    cut_cairo_set_source_result_color(cr, status);
    cairo_fill_preserve(cr);
    cairo_set_line_width(cr, 0.5);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_stroke(cr);
    cairo_restore(cr);

    cairo_move_to(cr, x, y + height);
    g_object_unref(layout);
}

static void
show_summary (CutPDFReport *report, CutRunContext *run_context)
{
    const GList *node;
    CutTestCase *test_case = NULL;
    cairo_t *cr;
    gdouble x, y, after_x, after_y, width, height;
    guint n_tests, n_successes, n_failures, n_errors, n_pendings, n_omissions;
    CutTestResultStatus test_case_status;

    cr = report->context;
    n_tests = n_successes = n_failures = n_errors = n_pendings = n_omissions = 0;
    test_case_status = CUT_TEST_RESULT_SUCCESS;
    cairo_get_current_point(cr, &x, &y);
    for (node = cut_run_context_get_results(run_context); node; node = g_list_next(node)) {
        CutTestResult *result = node->data;
        CutTestCase *current_test_case;
        CutTestResultStatus status;

        status = cut_test_result_get_status(result);
        current_test_case = cut_test_result_get_test_case(result);
        if (!test_case)
            test_case = current_test_case;

        if (test_case != current_test_case) {
            show_test_case(cr, test_case, test_case_status,
                           n_tests, n_successes, n_failures,
                           n_errors, n_pendings, n_omissions);
            n_tests = n_successes = n_failures =
                n_errors = n_pendings = n_omissions = 0;
            test_case_status = CUT_TEST_RESULT_SUCCESS;
            test_case = current_test_case;
        }

        n_tests++;
        switch (status) {
          case CUT_TEST_RESULT_SUCCESS:
            n_successes++;
            break;
          case CUT_TEST_RESULT_FAILURE:
            n_failures++;
            break;
          case CUT_TEST_RESULT_ERROR:
            n_errors++;
            break;
          case CUT_TEST_RESULT_PENDING:
            n_pendings++;
            break;
          case CUT_TEST_RESULT_OMISSION:
            n_omissions++;
            break;
          default:
            break;
        }
        if (test_case_status < status)
            test_case_status = status;
    }

    if (test_case)
        show_test_case(cr, test_case, test_case_status,
                       n_tests, n_successes, n_failures,
                       n_errors, n_pendings, n_omissions);

    cairo_get_current_point(cr, &after_x, &after_y);
    width = A4_WIDTH - x * 2;
    height = after_y - y;
}

static void
cb_complete_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                        CutPDFReport *report)
{
    CutCairoPieChart *chart;

    cairo_move_to(report->context, 50, 50);
    chart = cut_cairo_pie_chart_new(400, 300);
    cut_cairo_pie_chart_draw(chart, report->context, run_context);
    g_object_unref(chart);

    cairo_show_page(report->context);
    init_page(report->context);
    show_summary(report, run_context);
}

static void
cb_crashed (CutRunContext *run_context, const gchar *backtrace,
            CutPDFReport *report)
{
}

static void
connect_to_run_context (CutPDFReport *report, CutRunContext *run_context)
{
#define CONNECT(name) \
    g_signal_connect(run_context, #name, G_CALLBACK(cb_ ## name), report)

#define CONNECT_TO_TEST(name) \
    g_signal_connect(run_context, #name, G_CALLBACK(cb_test_signal), report)

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
disconnect_from_run_context (CutPDFReport *report, CutRunContext *run_context)
{
#define DISCONNECT(name)                                               \
    g_signal_handlers_disconnect_by_func(run_context,                       \
                                         G_CALLBACK(cb_ ## name),      \
                                         report)

    DISCONNECT(start_test_suite);
    DISCONNECT(start_test_case);
    DISCONNECT(start_test);

    DISCONNECT(complete_test);
    DISCONNECT(complete_test_case);
    DISCONNECT(complete_test_suite);

    DISCONNECT(crashed);

    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_test_signal),
                                         report);

#undef DISCONNECT
}

static void
attach_to_run_context (CutListener *listener,
                  CutRunContext   *run_context)
{
    CutPDFReport *report = CUT_PDF_REPORT(listener);

    if (report->run_context)
        detach_from_run_context(listener, report->run_context);

    if (run_context) {
        report->run_context = g_object_ref(run_context);
        connect_to_run_context(CUT_PDF_REPORT(listener), run_context);
    }
}

static void
detach_from_run_context (CutListener *listener,
                    CutRunContext   *run_context)
{
    CutPDFReport *report = CUT_PDF_REPORT(listener);
    if (report->run_context != run_context)
        return;

    disconnect_from_run_context(report, run_context);
    g_object_unref(report->run_context);
    report->run_context = NULL;
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
