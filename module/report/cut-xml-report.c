/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>
 *  Copyright (C) 2009-2013  Kouhei Sutou <kou@clear-code.com>
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
#include <glib/gstdio.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-report.h>
#include <cutter/cut-listener.h>
#include <cutter/cut-run-context.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_XML_REPORT            cut_type_xml_report
#define CUT_XML_REPORT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_XML_REPORT, CutXMLReport))
#define CUT_XML_REPORT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_XML_REPORT, CutXMLReportClass))
#define CUT_IS_XML_REPORT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_XML_REPORT))
#define CUT_IS_XML_REPORT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_XML_REPORT))
#define CUT_XML_REPORT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_XML_REPORT, CutXMLReportClass))

typedef struct _CutXMLReport CutXMLReport;
typedef struct _CutXMLReportClass CutXMLReportClass;

struct _CutXMLReport
{
    CutReport     object;
    CutRunContext    *run_context;
};

struct _CutXMLReportClass
{
    CutReportClass parent_class;
};

enum
{
    PROP_0,
    PROP_RUN_CONTEXT
};

static GType cut_type_xml_report = 0;
static CutReportClass *parent_class;

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
static gchar   *get_omission_results     (CutReport   *report);
static gchar   *get_crash_results        (CutReport   *report);
static gchar   *get_test_result          (CutReport   *report,
                                          const gchar *test_name);

static void
class_init (CutXMLReportClass *klass)
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
    report_class->get_omission_results     = get_omission_results;
    report_class->get_crash_results        = get_crash_results;
    report_class->get_test_result          = get_test_result;

    spec = g_param_spec_object("cut-run-context",
                               "CutRunContext object",
                               "A CutRunContext object",
                               CUT_TYPE_RUN_CONTEXT,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_RUN_CONTEXT, spec);
}

static void
init (CutXMLReport *report)
{
    report->run_context = NULL;
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
            sizeof (CutXMLReportClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutXMLReport),
            0,
            (GInstanceInitFunc) init,
        };

    static const GInterfaceInfo listener_info =
        {
            (GInterfaceInitFunc) listener_init,
            NULL,
            NULL
        };

    cut_type_xml_report =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_REPORT,
                                    "CutXMLReport",
                                    &info, 0);

    g_type_module_add_interface(type_module,
                                cut_type_xml_report,
                                CUT_TYPE_LISTENER,
                                &listener_info);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_xml_report)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_xml_report));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_XML_REPORT, first_property, var_args);
}

static void
dispose (GObject *object)
{
    CutXMLReport *report = CUT_XML_REPORT(object);

    if (report->run_context) {
        g_object_unref(report->run_context);
        report->run_context = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutXMLReport *report = CUT_XML_REPORT(object);

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
    CutXMLReport *report = CUT_XML_REPORT(object);

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
output_to_file (CutXMLReport *report, gchar *string)
{
    const gchar *filename;
    FILE *fp;
    int n_retries = 0;

    if (!string)
        return;

    filename = cut_report_get_filename(CUT_REPORT(report));
    if (!filename)
        return;

    fp = g_fopen(filename, "a");
    if (!fp)
        return;

    while (fwrite(string, strlen(string), 1, fp) != 1) {
        n_retries++;
        if (n_retries >= 3) {
            g_warning("can't write XML report to file [%s]: [%s]",
                      filename, string);
            break;
        }
    }

    fclose(fp);
}

static void
cb_ready_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                     guint n_test_cases, guint n_tests,
                     CutXMLReport *report)
{
    const gchar *filename;

    filename = cut_report_get_filename(CUT_REPORT(report));
    if (!filename)
        return;
    if (g_file_test(filename, G_FILE_TEST_EXISTS))
        g_unlink(filename);
}

static void
cb_start_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                     CutXMLReport *report)
{
    output_to_file(report, "<report>\n");
}

static void
cb_start_test_case (CutRunContext *run_context, CutTestCase *test_case,
                    CutXMLReport *report)
{
}

static void
cb_start_test (CutRunContext *run_context, CutTest *test, CutTestContext *test_context,
               CutXMLReport *report)
{
}

static void
cb_test_signal (CutRunContext      *run_context,
                CutTest        *test,
                CutTestContext *test_context,
                CutTestResult  *result,
                CutXMLReport   *report)
{
    GString *string;

    string = g_string_new(NULL);
    cut_test_result_to_xml_string(result, string, 2);
    output_to_file(report, string->str);
    g_string_free(string, TRUE);
}

static void
cb_complete_test (CutRunContext *run_context, CutTest *test,
                  CutTestContext *test_context, gboolean success,
                  CutXMLReport *report)
{
}

static void
cb_complete_test_case (CutRunContext *run_context, CutTestCase *test_case,
                       gboolean success, CutXMLReport *report)
{
}

static void
cb_complete_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                        gboolean success, CutXMLReport *report)
{
    output_to_file(report, "</report>");
}

static void
connect_to_run_context (CutXMLReport *report, CutRunContext *run_context)
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
    CONNECT_TO_TEST(omission_test);
    CONNECT_TO_TEST(crash_test);

    CONNECT(complete_test);
    CONNECT(complete_test_case);
    CONNECT(complete_test_suite);

#undef CONNECT
}

static void
disconnect_from_run_context (CutXMLReport *report, CutRunContext *run_context)
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

    g_signal_handlers_disconnect_by_func(run_context,
                                         G_CALLBACK(cb_test_signal),
                                         report);

#undef DISCONNECT
}

static void
attach_to_run_context (CutListener *listener,
                  CutRunContext   *run_context)
{
    CutXMLReport *report = CUT_XML_REPORT(listener);
    if (report->run_context)
        detach_from_run_context(listener, report->run_context);

    if (run_context) {
        report->run_context = g_object_ref(run_context);
        connect_to_run_context(CUT_XML_REPORT(listener), run_context);
    }
}

static void
detach_from_run_context (CutListener *listener,
                    CutRunContext   *run_context)
{
    CutXMLReport *report = CUT_XML_REPORT(listener);
    if (report->run_context != run_context)
        return;

    disconnect_from_run_context(report, run_context);
    g_object_unref(report->run_context);
    report->run_context = NULL;
}

static gchar *
get_all_results (CutReport *report)
{
    const GList *node;
    GString *xml = g_string_new("");
    CutXMLReport *xml_report = CUT_XML_REPORT(report);

    for (node = cut_run_context_get_results(xml_report->run_context);
         node;
         node = g_list_next(node)) {
        CutTestResult *result = node->data;

        cut_test_result_to_xml_string(result, xml, 2);
    }

    return g_string_free(xml, FALSE);
}

static gchar *
get_status_results (CutXMLReport *report, CutTestResultStatus status)
{
    const GList *node;
    GString *xml = g_string_new("");
    CutXMLReport *xml_report = CUT_XML_REPORT(report);

    for (node = cut_run_context_get_results(xml_report->run_context);
         node;
         node = g_list_next(node)) {
        CutTestResult *result = node->data;

        if (status != cut_test_result_get_status(result))
            continue;

        cut_test_result_to_xml_string(result, xml, 2);
    }

    return g_string_free(xml, FALSE);
}

static gchar *
get_success_results (CutReport *report)
{
    return get_status_results(CUT_XML_REPORT(report), CUT_TEST_RESULT_SUCCESS);
}

static gchar *
get_error_results (CutReport *report)
{
    return get_status_results(CUT_XML_REPORT(report), CUT_TEST_RESULT_ERROR);
}

static gchar *
get_failure_results (CutReport *report)
{
    return get_status_results(CUT_XML_REPORT(report), CUT_TEST_RESULT_FAILURE);
}

static gchar *
get_pending_results (CutReport *report)
{
    return get_status_results(CUT_XML_REPORT(report), CUT_TEST_RESULT_PENDING);
}

static gchar *
get_notification_results (CutReport *report)
{
    return get_status_results(CUT_XML_REPORT(report), CUT_TEST_RESULT_NOTIFICATION);
}

static gchar *
get_omission_results (CutReport *report)
{
    return get_status_results(CUT_XML_REPORT(report), CUT_TEST_RESULT_OMISSION);
}

static gchar *
get_crash_results (CutReport *report)
{
    return get_status_results(CUT_XML_REPORT(report), CUT_TEST_RESULT_CRASH);
}

static gchar *
get_test_result (CutReport *report, const gchar *test_name)
{
    return NULL;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
