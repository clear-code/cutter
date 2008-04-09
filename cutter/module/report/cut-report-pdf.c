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
#include <cutter/cut-runner.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_REPORT_PDF            cut_type_report_pdf
#define CUT_REPORT_PDF(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_REPORT_PDF, CutReportPDF))
#define CUT_REPORT_PDF_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_REPORT_PDF, CutReportPDFClass))
#define CUT_IS_REPORT_PDF(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_REPORT_PDF))
#define CUT_IS_REPORT_PDF_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_REPORT_PDF))
#define CUT_REPORT_PDF_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_REPORT_PDF, CutReportPDFClass))

#define A4_WIDTH 596
#define A4_HEIGHT 842

typedef struct _CutReportPDF CutReportPDF;
typedef struct _CutReportPDFClass CutReportPDFClass;

struct _CutReportPDF
{
    CutReport object;
    CutRunner *runner;
    cairo_t *context;
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

static GType cut_type_report_pdf = 0;
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
    CutReportClass *report_class;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    report_class = CUT_REPORT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

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
    report->context = NULL;
}

static void
listener_init (CutListenerClass *listener)
{
    listener->attach_to_runner   = attach_to_runner;
    listener->detach_from_runner = detach_from_runner;
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

    static const GInterfaceInfo listener_info =
        {
            (GInterfaceInitFunc) listener_init,
            NULL,
            NULL
        };

    cut_type_report_pdf =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_REPORT,
                                    "CutReportPDF",
                                    &info, 0);
    g_type_module_add_interface(type_module,
                                cut_type_report_pdf,
                                CUT_TYPE_LISTENER,
                                &listener_info);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (CUT_TYPE_REPORT_PDF)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(CUT_TYPE_REPORT_PDF));

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
init_page (CutReportPDF *report)
{
    cairo_move_to(report->context, 10, 10);
}

static void
relative_move_to (CutReportPDF *report, double x, double y)
{
    double current_x, current_y;

    cairo_get_current_point(report->context, &current_x, &current_y);
    cairo_move_to(report->context, current_x + x, current_y + y);
}

static void
cb_ready_test_suite (CutRunner *runner, CutTestSuite *test_suite,
                     guint n_test_cases, guint n_tests,
                     CutReportPDF *report)
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

    init_page(report);
}

static PangoLayout *
create_pango_layout (CutReportPDF *report, const gchar *utf8, gint font_size)
{
    PangoLayout *layout;
    PangoFontDescription *description;
    gchar *description_string;

    if (!utf8)
        return NULL;

    layout = pango_cairo_create_layout(report->context);

    if (font_size < 0)
        description_string = g_strdup("Mono");
    else
        description_string = g_strdup_printf("Mono %d", font_size);
    description = pango_font_description_from_string(description_string);
    g_free(description_string);

    pango_layout_set_font_description(layout, description);
    pango_font_description_free(description);

    pango_layout_set_text(layout, utf8, -1);

    return layout;
}

static void
show_text_at_center (CutReportPDF *report, const gchar *utf8,
                     guint center_x, guint center_y)
{
    PangoLayout *layout;
    int width, height;

    if (!utf8)
        return;

    layout = create_pango_layout(report, utf8, 10);
    if (!layout)
        return;

    pango_layout_get_pixel_size(layout, &width, &height);

    cairo_move_to(report->context,
                  center_x - (width / 2),
                  center_y - (height / 2));
    pango_cairo_show_layout(report->context, layout);
    g_object_unref(layout);
}

static void
show_text_at_current_position_with_page_feed (CutReportPDF *report, const gchar *utf8)
{
    PangoLayout *layout;
    double x, y;
    int width, height;

    if (!utf8)
        return;

    layout = create_pango_layout(report, utf8, 10);
    if (!layout)
        return;

    cairo_get_current_point(report->context, &x, &y);
    pango_layout_get_pixel_size(layout, &width, &height);

    if (A4_HEIGHT < y + height) {
        cairo_show_page(report->context);
        init_page(report);
        cairo_get_current_point(report->context, NULL, &y);
        cairo_move_to(report->context, x, y);
    }

    pango_cairo_show_layout(report->context, layout);
    cairo_move_to(report->context, x, y + height);
    g_object_unref(layout);
}

static void
cb_start_test_suite (CutRunner *runner, CutTestSuite *test_suite,
                     CutReportPDF *report)
{
    show_text_at_current_position_with_page_feed(report, cut_test_get_name(CUT_TEST(test_suite)));
}

static void
cb_start_test_case (CutRunner *runner, CutTestCase *test_case,
                    CutReportPDF *report)
{
    show_text_at_current_position_with_page_feed(report, cut_test_get_name(CUT_TEST(test_case)));
    relative_move_to(report, 10, 0);
}

static void
cb_start_test (CutRunner *runner, CutTest *test, CutTestContext *test_context,
               CutReportPDF *report)
{
    show_text_at_current_position_with_page_feed(report, cut_test_get_name(test));
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
    relative_move_to(report, -10, 5);
}

#define CENTER_X 100
#define CENTER_Y 100
#define RADIUS 50

static gdouble
show_pie_piece (CutReportPDF *report,
                gdouble start, gdouble percent,
                gdouble red, gdouble green, gdouble blue)
{
    gdouble end;
    gdouble text_x, text_y;
    gchar *string;

    if (percent == 0.0)
        return start;

    cairo_move_to(report->context, CENTER_X, CENTER_Y);
    end = start + 2 * M_PI * percent;
    cairo_arc(report->context, CENTER_X, CENTER_Y, RADIUS, start, end);
    cairo_set_source_rgba(report->context, red, green, blue, 0.8);
    cairo_fill_preserve(report->context);
    cairo_set_source_rgba(report->context, 0, 0, 0, 0.8);
    cairo_stroke(report->context);

    text_x = CENTER_X + sin((end - start) / 2) * (RADIUS + 10);
    text_y = CENTER_Y - cos((end - start) / 2) * (RADIUS + 10);
    string = g_strdup_printf("%.1f%%", percent * 100);
    show_text_at_center(report, string, text_x, text_y);
    g_free(string);

    return end;
}

static void
get_color_from_test_status (CutTestResultStatus status,
                            gdouble *red, gdouble *green, gdouble *blue)
{
    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        *red = 0x8a / (gdouble)0xff;
        *green = 0xe2 / (gdouble)0xff;
        *blue = 0x34 / (gdouble)0xff;
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        *red = 0x72 / (gdouble)0xff;
        *green = 0x9f / (gdouble)0xff;
        *blue = 0xcf / (gdouble)0xff;
        break;
      case CUT_TEST_RESULT_OMISSION:
        *red = 0x20 / (gdouble)0xff;
        *green = 0x4a / (gdouble)0xff;
        *blue = 0x87 / (gdouble)0xff;
        break;
      case CUT_TEST_RESULT_PENDING:
        *red = 0x5c / (gdouble)0xff;
        *green = 0x35 / (gdouble)0xff;
        *blue = 0x66 / (gdouble)0xff;
        break;
      case CUT_TEST_RESULT_FAILURE:
        *red = 0xef / (gdouble)0xff;
        *green = 0x29 / (gdouble)0xff;
        *blue = 0x29 / (gdouble)0xff;
        break;
      case CUT_TEST_RESULT_ERROR:
        *red = 0xfc / (gdouble)0xff;
        *green = 0xe9 / (gdouble)0xff;
        *blue = 0x4f / (gdouble)0xff;
        break;
      default:
        break;
    }
}

static gdouble
show_status_pie_piece (CutReportPDF *report, CutRunner *runner,
                       gdouble start, CutTestResultStatus status)
{
    gdouble red, green, blue;
    guint n_tests = 0, n_results;
    gdouble end;

    get_color_from_test_status(status,
                               &red, &green, &blue);
    n_tests = cut_runner_get_n_tests(runner);

    switch (status) {
      case CUT_TEST_RESULT_SUCCESS:
        n_results = cut_runner_get_n_successes(runner);
        break;
      case CUT_TEST_RESULT_NOTIFICATION:
        n_results = cut_runner_get_n_notifications(runner);
        break;
      case CUT_TEST_RESULT_OMISSION:
        n_results = cut_runner_get_n_omissions(runner);
        break;
      case CUT_TEST_RESULT_PENDING:
        n_results = cut_runner_get_n_pendings(runner);
        break;
      case CUT_TEST_RESULT_FAILURE:
        n_results = cut_runner_get_n_failures(runner);
        break;
      case CUT_TEST_RESULT_ERROR:
        n_results = cut_runner_get_n_errors(runner);
        break;
      default:
        return start;
        break;
    }

    end = show_pie_piece(report, start,
                         ((gdouble)n_results / (gdouble)n_tests),
                         red, green, blue);

    return end;
}

static void
cb_complete_test_suite (CutRunner *runner, CutTestSuite *test_suite,
                        CutReportPDF *report)
{
    double start;

    cairo_show_page(report->context);

    cairo_set_line_width(report->context, 0.75);

    start = 2 * M_PI * 0.75;
    start = show_status_pie_piece(report, runner, start, CUT_TEST_RESULT_SUCCESS);
    start = show_status_pie_piece(report, runner, start, CUT_TEST_RESULT_FAILURE);
    start = show_status_pie_piece(report, runner, start, CUT_TEST_RESULT_ERROR);
    start = show_status_pie_piece(report, runner, start, CUT_TEST_RESULT_PENDING);
    start = show_status_pie_piece(report, runner, start, CUT_TEST_RESULT_NOTIFICATION);
    start = show_status_pie_piece(report, runner, start, CUT_TEST_RESULT_OMISSION);
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
