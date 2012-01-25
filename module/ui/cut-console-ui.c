/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2011  Kouhei Sutou <kou@clear-code.com>
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
#include <glib/gstdio.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-listener.h>
#include <cutter/cut-ui.h>
#include <cutter/cut-test-runner.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-suite.h>
#include <cutter/cut-test-context.h>
#include <cutter/cut-backtrace-entry.h>
#include <cutter/cut-verbose-level.h>
#include <cutter/cut-colorize-differ.h>
#include <cutter/cut-console-diff-writer.h>
#include <cutter/cut-console.h>
#include <cutter/cut-enum-types.h>

#define CUT_TYPE_CONSOLE_UI            cut_type_console_ui
#define CUT_CONSOLE_UI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_CONSOLE_UI, CutConsoleUI))
#define CUT_CONSOLE_UI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_CONSOLE_UI, CutConsoleUIClass))
#define CUT_IS_CONSOLE_UI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_CONSOLE_UI))
#define CUT_IS_CONSOLE_UI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_CONSOLE_UI))
#define CUT_CONSOLE_UI_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_CONSOLE_UI, CutConsoleUIClass))

#define CRASH_COLOR                             \
    CUT_CONSOLE_COLOR_RED_BACK                  \
    CUT_CONSOLE_COLOR_WHITE

typedef struct _CutConsoleUI CutConsoleUI;
typedef struct _CutConsoleUIClass CutConsoleUIClass;

struct _CutConsoleUI
{
    GObject       object;
    gchar        *name;
    gboolean      use_color;
    CutVerboseLevel verbose_level;
    gchar        *notify_command;
    GList        *errors;
    gint          progress_row;
    gint          progress_row_max;
    gboolean      show_detail_immediately;
};

struct _CutConsoleUIClass
{
    GObjectClass parent_class;
};

enum
{
    PROP_0,
    PROP_USE_COLOR,
    PROP_VERBOSE_LEVEL,
    PROP_NOTIFY_COMMAND,
    PROP_PROGRESS_ROW_MAX,
    PROP_SHOW_DETAIL_IMMEDIATELY
};

static GType cut_type_console_ui = 0;
static GObjectClass *parent_class;

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void     attach_to_run_context   (CutListener   *listener,
                                         CutRunContext *run_context);
static void     detach_from_run_context (CutListener   *listener,
                                         CutRunContext *run_context);
static gboolean run                     (CutUI         *ui,
                                         CutRunContext *run_context);

static void     print_result_detail     (CutConsoleUI  *console,
                                         CutTestResultStatus status,
                                         CutTestResult *result);

static void
class_init (CutConsoleUIClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_boolean("use-color",
                                "Use color",
                                "Whether use color",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_USE_COLOR, spec);

    spec = g_param_spec_enum("verbose-level",
                             "Verbose Level",
                             "The number of representing verbosity level",
                             CUT_TYPE_VERBOSE_LEVEL,
                             CUT_VERBOSE_LEVEL_NORMAL,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VERBOSE_LEVEL, spec);

    spec = g_param_spec_string("notify-command",
                               "Notify Command",
                               "The notify command for notifying test result",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NOTIFY_COMMAND, spec);

    spec = g_param_spec_int("progress-row-max",
                            "Progress Row Max",
                            "The max number of progress row",
                            -1, G_MAXINT16, -1,
                            G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_PROGRESS_ROW_MAX, spec);

    spec = g_param_spec_boolean("show-detail-immediately",
                                "Show Detail Immediately",
                                "Whether shows test details immediately.",
                                TRUE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_SHOW_DETAIL_IMMEDIATELY,
                                    spec);
}

static void
init (CutConsoleUI *console)
{
    console->use_color = FALSE;
    console->verbose_level = CUT_VERBOSE_LEVEL_NORMAL;
    console->notify_command = NULL;
    console->errors = NULL;
    console->progress_row = 0;
    console->progress_row_max = -1;
    console->show_detail_immediately = TRUE;
}

static void
listener_init (CutListenerClass *listener)
{
    listener->attach_to_run_context   = attach_to_run_context;
    listener->detach_from_run_context = detach_from_run_context;
}

static void
ui_init (CutUIClass *ui)
{
    ui->run = run;
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutConsoleUIClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutConsoleUI),
            0,
            (GInstanceInitFunc) init,
        };

	static const GInterfaceInfo ui_info =
	    {
            (GInterfaceInitFunc) ui_init,
            NULL,
            NULL
        };

	static const GInterfaceInfo listener_info =
	    {
            (GInterfaceInitFunc) listener_init,
            NULL,
            NULL
        };

    cut_type_console_ui = g_type_module_register_type(type_module,
                                                      G_TYPE_OBJECT,
                                                      "CutConsoleUI",
                                                      &info, 0);

    g_type_module_add_interface(type_module,
                                cut_type_console_ui,
                                CUT_TYPE_UI,
                                &ui_info);

    g_type_module_add_interface(type_module,
                                cut_type_console_ui,
                                CUT_TYPE_LISTENER,
                                &listener_info);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_console_ui)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_console_ui));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_CONSOLE_UI, first_property, var_args);
}

static void
dispose (GObject *object)
{
    CutConsoleUI *console;

    console = CUT_CONSOLE_UI(object);

    if (console->errors) {
        g_list_foreach(console->errors, (GFunc)g_error_free, NULL);
        g_list_free(console->errors);
        console->errors = NULL;
    }

    if (console->notify_command) {
        g_free(console->notify_command);
        console->notify_command = NULL;
    }

    G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutConsoleUI *console = CUT_CONSOLE_UI(object);

    switch (prop_id) {
    case PROP_USE_COLOR:
        console->use_color = g_value_get_boolean(value);
        break;
    case PROP_VERBOSE_LEVEL:
        console->verbose_level = g_value_get_enum(value);
        break;
    case PROP_NOTIFY_COMMAND:
        if (console->notify_command)
            g_free(console->notify_command);
        console->notify_command = g_value_dup_string(value);
        break;
    case PROP_PROGRESS_ROW_MAX:
        console->progress_row_max = g_value_get_int(value);
        break;
    case PROP_SHOW_DETAIL_IMMEDIATELY:
        console->show_detail_immediately = g_value_get_boolean(value);
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
    CutConsoleUI *console = CUT_CONSOLE_UI(object);

    switch (prop_id) {
    case PROP_USE_COLOR:
        g_value_set_boolean(value, console->use_color);
        break;
    case PROP_VERBOSE_LEVEL:
        g_value_set_enum(value, console->verbose_level);
        break;
    case PROP_NOTIFY_COMMAND:
        g_value_set_string(value, console->notify_command);
        break;
    case PROP_PROGRESS_ROW_MAX:
        g_value_set_int(value, console->progress_row_max);
        break;
    case PROP_SHOW_DETAIL_IMMEDIATELY:
        g_value_set_boolean(value, console->show_detail_immediately);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static const gchar *
status_to_name(CutTestResultStatus status)
{
    GEnumClass *enum_class;
    GEnumValue *value;
    const gchar *name = "unknown";

    enum_class = g_type_class_ref(CUT_TYPE_TEST_RESULT_STATUS);
    value = g_enum_get_value(enum_class, status);
    if (value)
        name = value->value_nick;
    g_type_class_unref(enum_class);
    return name;
}

static const gchar *
status_to_label(CutTestResultStatus status)
{
    const gchar *name;

    switch (status) {
    case CUT_TEST_RESULT_SUCCESS:
        name = "Success";
        break;
    case CUT_TEST_RESULT_NOTIFICATION:
        name = "Notification";
        break;
    case CUT_TEST_RESULT_OMISSION:
        name = "Omission";
        break;
    case CUT_TEST_RESULT_PENDING:
        name = "Pending";
        break;
    case CUT_TEST_RESULT_FAILURE:
        name = "Failure";
        break;
    case CUT_TEST_RESULT_ERROR:
        name = "Error";
        break;
    case CUT_TEST_RESULT_CRASH:
        name = "Crash";
        break;
    default:
        name = "MUST NOT HAPPEN!!!";
        break;
    }

    return name;
}

static const gchar *
status_to_color(CutTestResultStatus status)
{
    const gchar *color;

    switch (status) {
    case CUT_TEST_RESULT_SUCCESS:
        color = CUT_CONSOLE_COLOR_GREEN;
        break;
    case CUT_TEST_RESULT_NOTIFICATION:
        color = CUT_CONSOLE_COLOR_CYAN;
        break;
    case CUT_TEST_RESULT_OMISSION:
        color = CUT_CONSOLE_COLOR_BLUE;
        break;
    case CUT_TEST_RESULT_PENDING:
        color = CUT_CONSOLE_COLOR_MAGENTA;
        break;
    case CUT_TEST_RESULT_FAILURE:
        color = CUT_CONSOLE_COLOR_RED;
        break;
    case CUT_TEST_RESULT_ERROR:
        color = CUT_CONSOLE_COLOR_YELLOW;
        break;
    case CUT_TEST_RESULT_CRASH:
        color = CRASH_COLOR;
        break;
    default:
        color = "";
        break;
    }

    return color;
}

static void
print_with_colorv (CutConsoleUI *console, const gchar *color,
                   gchar const *format, va_list args)
{
   if (console->use_color) {
        gchar *message;
        message = g_strdup_vprintf(format, args);
        g_print("%s%s%s", color, message, CUT_CONSOLE_COLOR_NORMAL);
        g_free(message);
    } else {
        g_vprintf(format, args);
    }
}

static void
print_with_color (CutConsoleUI *console, const gchar *color,
                  gchar const *format, ...)
{
    va_list args;

    va_start(args, format);
    print_with_colorv(console, color, format, args);
    va_end(args);
}

static void
print_for_status (CutConsoleUI *console, CutTestResultStatus status,
                  gchar const *format, ...)
{
    va_list args;

    va_start(args, format);
    print_with_colorv(console, status_to_color(status), format, args);
    va_end(args);
}

static void
print_progress (CutConsoleUI *console, CutTestResultStatus status,
                gchar const *mark)
{
    print_with_color(console, status_to_color(status), "%s", mark);

    console->progress_row += strlen(mark);
    if (console->progress_row >= console->progress_row_max) {
        if (console->progress_row_max != -1 &&
            console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
            g_print("\n");
        console->progress_row = 0;
    }
}

static void
print_progress_in_detail (CutConsoleUI *console, CutTestResult *result)
{
    CutTestResultStatus status;
    char mark = '=';
    GString *marker = NULL;
    int i;

    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;

    if (!console->show_detail_immediately)
        return;

    if (console->progress_row_max != -1) {
        marker = g_string_new(NULL);
        for (i = 0; i < console->progress_row_max; i++) {
            g_string_append_c(marker, mark);
        }
    }

    status = cut_test_result_get_status(result);
    g_print("\n");
    if (marker) {
        print_for_status(console, status, "%s", marker->str);
        g_print("\n");
    }
    print_result_detail(console, status, result);
    if (marker) {
        print_for_status(console, status, "%s", marker->str);
        g_print("\n");
    }

    if (marker) {
        g_string_free(marker, TRUE);
    }

    console->progress_row = 0;
}

static void
cb_start_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                     CutConsoleUI *console)
{
}

static void
cb_start_test_case (CutRunContext *run_context, CutTestCase *test_case,
                    CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    print_with_color(console, CUT_CONSOLE_COLOR_GREEN_BACK,
                     "%s", cut_test_get_name(CUT_TEST(test_case)));
    g_print(":\n");
}

static void
cb_start_test_iterator (CutRunContext *run_context,
                        CutTestIterator *test_iterator,
                        CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    g_print("  ");
    print_with_color(console, CUT_CONSOLE_COLOR_BLUE_BACK,
                     "%s", cut_test_get_name(CUT_TEST(test_iterator)));
    g_print(":\n");
}

static guint
utf8_n_spaces (const gchar *string)
{
    guint n = 0;

    for (; string[0]; string = g_utf8_next_char(string)) {
        if (g_unichar_iswide_cjk(g_utf8_get_char(string))) {
            n += 2;
        } else if (string[0] == '\t') {
            n += 8;
        } else {
            n++;
        }
    }

    return n;
}

static void
print_test_on_start (CutConsoleUI *console, const gchar *name,
                     CutTest *test, const gchar *indent)
{
    GString *tab_stop;
    guint name_length;
    const gchar *description;

    description = cut_test_get_description(test);
    if (description)
        g_print("  %s%s\n", indent, description);

    name_length = utf8_n_spaces(indent) + utf8_n_spaces(name) + 2;
    tab_stop = g_string_new("");
    while (name_length < (8 * 7 - 1)) {
        g_string_append_c(tab_stop, '\t');
        name_length += 8;
    }
    g_print("  %s%s:%s", indent, name, tab_stop->str);
    g_string_free(tab_stop, TRUE);
    fflush(stdout);
}

static void
cb_start_test (CutRunContext *run_context, CutTest *test,
               CutTestContext *test_context, CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    print_test_on_start(console, cut_test_get_name(test), test, "");
}

static void
cb_start_iterated_test (CutRunContext *run_context,
                        CutIteratedTest *iterated_test,
                        CutTestContext *test_context, CutConsoleUI *console)
{
    const gchar *name = NULL;

    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    if (cut_test_context_have_data(test_context)) {
        CutTestData *data;

        data = cut_test_context_get_current_data(test_context);
        name = cut_test_data_get_name(data);
    }

    if (!name)
        name = cut_test_get_name(CUT_TEST(iterated_test));

    print_test_on_start(console, name, CUT_TEST(iterated_test), "  ");
}

static void
cb_success_test (CutRunContext  *run_context,
                 CutTest        *test,
                 CutTestContext *context,
                 CutTestResult  *result,
                 CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_progress(console, CUT_TEST_RESULT_SUCCESS, ".");
    fflush(stdout);
}

static void
cb_notification_test (CutRunContext  *run_context,
                      CutTest        *test,
                      CutTestContext *test_context,
                      CutTestResult  *result,
                      CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_progress(console, CUT_TEST_RESULT_NOTIFICATION, "N");
    print_progress_in_detail(console, result);
    fflush(stdout);
}

static void
cb_omission_test (CutRunContext  *run_context,
                  CutTest        *test,
                  CutTestContext *test_context,
                  CutTestResult  *result,
                  CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_progress(console, CUT_TEST_RESULT_OMISSION, "O");
    print_progress_in_detail(console, result);
    fflush(stdout);
}

static void
cb_pending_test (CutRunContext  *run_context,
                 CutTest        *test,
                 CutTestContext *test_context,
                 CutTestResult  *result,
                 CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_progress(console, CUT_TEST_RESULT_PENDING, "P");
    print_progress_in_detail(console, result);
    fflush(stdout);
}

static void
cb_failure_test (CutRunContext  *run_context,
                 CutTest        *test,
                 CutTestContext *test_context,
                 CutTestResult  *result,
                 CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_progress(console, CUT_TEST_RESULT_FAILURE, "F");
    print_progress_in_detail(console, result);
    fflush(stdout);
}

static void
cb_error_test (CutRunContext  *run_context,
               CutTest        *test,
               CutTestContext *test_context,
               CutTestResult  *result,
               CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_progress(console, CUT_TEST_RESULT_ERROR, "E");
    print_progress_in_detail(console, result);
    fflush(stdout);
}

static void
handle_crash (CutRunContext  *run_context,
              CutTestResult  *result,
              CutConsoleUI   *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;
    print_progress(console, CUT_TEST_RESULT_CRASH, "!");
    print_progress_in_detail(console, result);
    fflush(stdout);
}

static void
cb_crash_test (CutRunContext  *run_context,
               CutTest        *test,
               CutTestContext *test_context,
               CutTestResult  *result,
               CutConsoleUI   *console)
{
    handle_crash(run_context, result, console);
}

static void
cb_crash_test_suite (CutRunContext  *run_context,
                     CutTestSuite   *test_suite,
                     CutTestResult  *result,
                     CutConsoleUI   *console)
{
    handle_crash(run_context, result, console);
}

static void
cb_complete_test (CutRunContext *run_context, CutTest *test,
                  CutTestContext *test_context, gboolean success,
                  CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;

    g_print(": (%f)\n", cut_test_get_elapsed(test));
    fflush(stdout);
}

static void
cb_complete_iterated_test (CutRunContext *run_context,
                           CutIteratedTest *iterated_test,
                           CutTestContext *test_context, gboolean success,
                           CutConsoleUI *console)
{
    cb_complete_test(run_context, CUT_TEST(iterated_test),
                     test_context, success, console);
}

static void
cb_complete_test_iterator (CutRunContext *run_context,
                           CutTestIterator *test_iterator,
                           gboolean success, CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;
}

static void
cb_complete_test_case (CutRunContext *run_context, CutTestCase *test_case,
                       gboolean success, CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;
}

static void
cb_complete_test_suite (CutRunContext *run_context, CutTestSuite *test_suite,
                        gboolean success, CutConsoleUI *console)
{
    if (console->verbose_level < CUT_VERBOSE_LEVEL_VERBOSE)
        return;
}

typedef struct _ConsoleAndStatus {
    CutConsoleUI *console;
    CutTestResultStatus status;
} ConsoleAndStatus;

static void
print_each_attribute (gpointer key, gpointer value, gpointer data)
{
    ConsoleAndStatus *info = (ConsoleAndStatus*)data;
    g_print("\n");
    print_with_color(info->console, status_to_color(info->status),
                     "  %s: %s",
                     (const gchar*)key, (const gchar*)value,
                     NULL);
}

static void
print_test_attributes (CutConsoleUI *console, CutTestResultStatus status,
                       CutTest *test)
{
    ConsoleAndStatus info;
    info.console = console;
    info.status = status;
    g_hash_table_foreach(cut_test_get_attributes(test),
                         (GHFunc)print_each_attribute, &info);
}

static CutDiffWriter *
console_diff_writer_new (gboolean use_color)
{
    CutDiffWriter *writer;
    gchar *deleted_mark_color, *inserted_mark_color;
    gchar *deleted_line_color, *inserted_line_color;
    gchar *deleted_segment_color, *inserted_segment_color;

    writer = cut_console_diff_writer_new(use_color);

    deleted_mark_color =
        g_strdup(cut_console_diff_writer_get_deleted_mark_color(writer));
    inserted_mark_color =
        g_strdup(cut_console_diff_writer_get_inserted_mark_color(writer));
    deleted_line_color =
        g_strdup(cut_console_diff_writer_get_deleted_line_color(writer));
    inserted_line_color =
        g_strdup(cut_console_diff_writer_get_inserted_line_color(writer));
    deleted_segment_color =
        g_strdup(cut_console_diff_writer_get_deleted_segment_color(writer));
    inserted_segment_color =
        g_strdup(cut_console_diff_writer_get_inserted_segment_color(writer));

    cut_console_diff_writer_set_inserted_mark_color(writer, deleted_mark_color);
    cut_console_diff_writer_set_deleted_mark_color(writer, inserted_mark_color);
    cut_console_diff_writer_set_inserted_line_color(writer, deleted_line_color);
    cut_console_diff_writer_set_deleted_line_color(writer, inserted_line_color);
    cut_console_diff_writer_set_inserted_segment_color(writer,
                                                       deleted_segment_color);
    cut_console_diff_writer_set_deleted_segment_color(writer,
                                                      inserted_segment_color);

    g_free(deleted_mark_color);
    g_free(inserted_mark_color);
    g_free(deleted_line_color);
    g_free(inserted_line_color);
    g_free(deleted_segment_color);
    g_free(inserted_segment_color);

    return writer;
}

static void
print_result_message (CutConsoleUI *console, CutTestResultStatus status,
                      CutTestResult *result)
{
    const gchar *expected, *actual;

    expected = cut_test_result_get_expected(result);
    actual = cut_test_result_get_actual(result);
    if (console->use_color && expected && actual) {
        const gchar *user_message, *system_message;
        CutDiffer *differ;

        user_message = cut_test_result_get_user_message(result);
        system_message = cut_test_result_get_system_message(result);
        if (user_message)
            g_print("\n%s", user_message);
        if (system_message)
            g_print("\n%s", system_message);

        g_print("\n");
        g_print("expected: <");
        print_for_status(console, CUT_TEST_RESULT_SUCCESS, "%s", expected);
        g_print(">\n");
        g_print("  actual: <");
        print_for_status(console, CUT_TEST_RESULT_FAILURE, "%s", actual);
        g_print(">");

        differ = cut_colorize_differ_new(expected, actual);
        if (cut_differ_need_diff(differ)) {
            CutDiffWriter *writer;

            g_print("\n\n");
            g_print("diff:\n");
            writer = console_diff_writer_new(console->use_color);
            cut_differ_diff(differ, writer);
            g_object_unref(writer);
        }
        g_object_unref(differ);
    } else {
        const gchar *message;

        message = cut_test_result_get_message(result);
        if (message) {
            g_print("\n%s", message);
        }
    }
    g_print("\n");
}

static void
print_result_backtrace (CutConsoleUI *console, CutTestResultStatus status,
                        CutTestResult *result)
{
    const GList *node;
    for (node = cut_test_result_get_backtrace(result);
         node;
         node = g_list_next(node)) {
        CutBacktraceEntry *entry = node->data;
        gchar *formatted_entry;

        formatted_entry = cut_backtrace_entry_format(entry);
        g_print("%s\n", formatted_entry);
        g_free(formatted_entry);
    }
}

static void
print_result_detail (CutConsoleUI *console, CutTestResultStatus status,
                     CutTestResult *result)
{
    const gchar *name;
    CutTest *test;

    print_for_status(console, status, "%s", status_to_label(status));

    name = cut_test_result_get_test_name(result);
    if (!name)
        name = cut_test_result_get_test_case_name(result);
    if (!name)
        name = cut_test_result_get_test_suite_name(result);

    g_print(": %s", name);

    test = cut_test_result_get_test(result);
    if (test)
        print_test_attributes(console, status, test);

    print_result_message(console, status, result);
    print_result_backtrace(console, status, result);
}

static void
print_error_detail (CutConsoleUI *console, GError *error)
{
    print_for_status(console, CUT_TEST_RESULT_ERROR,
                     "SystemError: %s:%d",
                     g_quark_to_string(error->domain),
                     error->code);
    if (error->message) {
        g_print("\n");
        print_for_status(console, CUT_TEST_RESULT_ERROR,
                         "%s", error->message);
    }
    g_print("\n");
}

static void
print_results (CutConsoleUI *console, CutRunContext *run_context)
{
    gint i;
    const GList *node;

    if (console->show_detail_immediately)
        return;

    i = 1;
    for (node = console->errors; node; node = g_list_next(node)) {
        GError *error = node->data;

        g_print("\n%d) ", i);
        print_error_detail(console, error);
        i++;
    }

    for (node = cut_run_context_get_results(run_context);
         node;
         node = g_list_next(node)) {
        CutTestResult *result = node->data;
        CutTestResultStatus status;

        status = cut_test_result_get_status(result);
        if (status == CUT_TEST_RESULT_SUCCESS)
            continue;

        g_print("\n%d) ", i);
        print_result_detail(console, status, result);
        i++;
    }
}

static gdouble
compute_pass_percentage (CutRunContext *run_context)
{
    guint n_tests, n_successes;
    gdouble pass_percentage;

    n_tests = cut_run_context_get_n_tests(run_context);
    n_successes = cut_run_context_get_n_successes(run_context);

    if (n_tests == 0) {
        pass_percentage = 0;
    } else {
        pass_percentage = 100.0 * ((gdouble)n_successes / (gdouble)n_tests);
    }

    return pass_percentage;
}

static gchar *
format_summary (CutRunContext *run_context)
{
    guint n_tests, n_assertions, n_failures, n_errors;
    guint n_pendings, n_notifications, n_omissions;

    n_tests = cut_run_context_get_n_tests(run_context);
    n_assertions = cut_run_context_get_n_assertions(run_context);
    n_failures = cut_run_context_get_n_failures(run_context);
    n_errors = cut_run_context_get_n_errors(run_context);
    n_pendings = cut_run_context_get_n_pendings(run_context);
    n_notifications = cut_run_context_get_n_notifications(run_context);
    n_omissions = cut_run_context_get_n_omissions(run_context);

    return g_strdup_printf("%d test(s), %d assertion(s), %d failure(s), "
                           "%d error(s), %d pending(s), %d omission(s), "
                           "%d notification(s)",
                           n_tests, n_assertions, n_failures, n_errors,
                           n_pendings, n_omissions, n_notifications);
}

static void
print_summary (CutConsoleUI *console, CutRunContext *run_context)
{
    const gchar *color;
    gchar *summary;

    color = status_to_color(cut_run_context_get_status(run_context));
    summary = format_summary(run_context);
    print_with_color(console, color, "%s", summary);
    g_free(summary);
    g_print("\n");

    print_with_color(console, color,
                     "%g%% passed", compute_pass_percentage(run_context));
    g_print("\n");
}

static gchar *
search_icon_path (CutTestResultStatus status, gboolean success)
{
    GList *candiate_icon_names = NULL, *node;
    const gchar *icons_dir = NULL;
    const gchar *icon_theme = "kinotan";
    gchar *icon_path = NULL;

    candiate_icon_names = g_list_append(candiate_icon_names,
                                        (gchar *)status_to_name(status));
    if (success) {
        candiate_icon_names = g_list_append(candiate_icon_names, "pass");
    } else {
        switch (status) {
        case CUT_TEST_RESULT_FAILURE:
            candiate_icon_names = g_list_append(candiate_icon_names, "error");
            break;
        case CUT_TEST_RESULT_ERROR:
            candiate_icon_names = g_list_append(candiate_icon_names, "failure");
            break;
        default:
            break;
        }
    }
    candiate_icon_names = g_list_append(candiate_icon_names, "default");

    icons_dir = g_getenv("CUT_ICONS_DIR");
    if (!icons_dir) {
#ifdef G_OS_WIN32
        icons_dir = cut_win32_icons_dir();
#else
        icons_dir = ICONS_DIR;
#endif
    }

    for (node = candiate_icon_names; node; node = g_list_next(node)) {
        const gchar *icon_name = node->data;
        gchar *icon_base_path;

        icon_base_path = g_strdup_printf("%s.png", icon_name);
        icon_path = g_build_filename(icons_dir, icon_theme, icon_base_path,
                                     NULL);
        g_free(icon_base_path);
        if (g_file_test(icon_path, G_FILE_TEST_IS_REGULAR)) {
            break;
        } else {
            g_free(icon_path);
            icon_path = NULL;
        }
    }

    return icon_path;
}

static void
run_notify_command (CutConsoleUI *console, gchar **args)
{
    GError *error = NULL;

    g_spawn_async(NULL,
                  args,
                  NULL,
                  G_SPAWN_SEARCH_PATH,
                  NULL,
                  NULL,
                  NULL,
                  &error);
    if (error) {
        gchar *command_line;
        command_line = g_strjoinv(" ", args);
        g_print("failed to run <%s>: <%s>: <%s>\n",
                console->notify_command,
                command_line,
                error->message);
        g_free(command_line);
        g_error_free(error);
    }
}

static void
notify_by_growlnotify (CutConsoleUI *console, CutRunContext *run_context,
                       gboolean success)
{
    GPtrArray *args;
    CutTestResultStatus status;
    gchar *icon_path;

    status = cut_run_context_get_status(run_context);
    icon_path = search_icon_path(status, success);

    args = g_ptr_array_new();
    g_ptr_array_add(args, g_strdup(console->notify_command));
#ifdef G_OS_WIN32
    g_ptr_array_add(args,
                    g_strdup_printf("/t:\"%s [%g%%] (%gs)\"",
                                    status_to_label(status),
                                    compute_pass_percentage(run_context),
                                    cut_run_context_get_elapsed(run_context)));
    if (success) {
        g_ptr_array_add(args, g_strdup("/p:0"));
    } else {
        g_ptr_array_add(args, g_strdup("/p:2"));
    }
    if (icon_path) {
        g_ptr_array_add(args, g_strdup_printf("/i:\"%s\"", icon_path));
    }
    g_ptr_array_add(args, g_strdup_printf("%s", format_summary(run_context)));
#else
    g_ptr_array_add(args, g_strdup("--message"));
    g_ptr_array_add(args, format_summary(run_context));
    g_ptr_array_add(args, g_strdup("--priority"));
    if (success) {
        g_ptr_array_add(args, g_strdup("Normal"));
    } else {
        g_ptr_array_add(args, g_strdup("Emergency"));
    }
    if (icon_path) {
        g_ptr_array_add(args, g_strdup("--image"));
        g_ptr_array_add(args, icon_path);
    }
    g_ptr_array_add(args,
                    g_strdup_printf("%s [%g%%] (%gs)",
                                    status_to_label(status),
                                    compute_pass_percentage(run_context),
                                    cut_run_context_get_elapsed(run_context)));
#endif
    g_ptr_array_add(args, NULL);

    run_notify_command(console, (gchar **)args->pdata);

    g_ptr_array_foreach(args, (GFunc)g_free, NULL);
    g_ptr_array_free(args, TRUE);
}

static void
notify_by_notify_send (CutConsoleUI *console, CutRunContext *run_context,
                       gboolean success)
{
    GPtrArray *args;
    CutTestResultStatus status;
    gchar *icon_path;
    gchar *summary;

    status = cut_run_context_get_status(run_context);
    icon_path = search_icon_path(status, success);

    args = g_ptr_array_new();
    g_ptr_array_add(args, g_strdup(console->notify_command));
    g_ptr_array_add(args, g_strdup("--expire-time"));
    g_ptr_array_add(args, g_strdup("5000"));
    g_ptr_array_add(args, g_strdup("--urgency"));
    if (success) {
        g_ptr_array_add(args, g_strdup("normal"));
    } else {
        g_ptr_array_add(args, g_strdup("critical"));
    }
    if (icon_path) {
        g_ptr_array_add(args, g_strdup("--icon"));
        g_ptr_array_add(args, icon_path);
    }
    g_ptr_array_add(args,
                    g_strdup_printf("%s [%g%%] (%gs)",
                                    status_to_label(status),
                                    compute_pass_percentage(run_context),
                                    cut_run_context_get_elapsed(run_context)));
    summary = format_summary(run_context);
    g_ptr_array_add(args, g_markup_escape_text(summary, -1));
    g_free(summary);
    g_ptr_array_add(args, NULL);

    run_notify_command(console, (gchar **)args->pdata);

    g_ptr_array_foreach(args, (GFunc)g_free, NULL);
    g_ptr_array_free(args, TRUE);
}

static void
notify (CutConsoleUI *console, CutRunContext *run_context, gboolean success)
{
    if (!console->notify_command)
        return;

    if (strcmp(console->notify_command, "notify-send") == 0) {
        notify_by_notify_send(console, run_context, success);
    } else if (strcmp(console->notify_command, "growlnotify") == 0) {
        notify_by_growlnotify(console, run_context, success);
    }
}

static void
cb_complete_run (CutRunContext *run_context, gboolean success,
                 CutConsoleUI *console)
{
    CutVerboseLevel verbose_level;

    notify(console, run_context, success);

    verbose_level = console->verbose_level;
    if (verbose_level < CUT_VERBOSE_LEVEL_NORMAL)
        return;

    if (verbose_level == CUT_VERBOSE_LEVEL_NORMAL)
        g_print("\n");

    print_results(console, run_context);

    g_print("\n");
    g_print("Finished in %f seconds (total: %f seconds)",
            cut_run_context_get_elapsed(run_context),
            cut_run_context_get_total_elapsed(run_context));
    g_print("\n\n");

    print_summary(console, run_context);
}

static void
cb_error (CutRunContext *run_context, GError *error, CutConsoleUI *console)
{
    if (console->verbose_level >= CUT_VERBOSE_LEVEL_NORMAL) {
        print_with_color(console, status_to_color(CUT_TEST_RESULT_ERROR), "E");
        if (console->show_detail_immediately) {
            g_print("\n");
            print_error_detail(console, error);
        }
        fflush(stdout);
    }

    console->errors = g_list_append(console->errors, g_error_copy(error));
}

static void
connect_to_run_context (CutConsoleUI *console, CutRunContext *run_context)
{
#define CONNECT(name) \
    g_signal_connect(run_context, #name, G_CALLBACK(cb_ ## name), console)

    CONNECT(start_test_suite);
    CONNECT(start_test_case);
    CONNECT(start_test_iterator);
    CONNECT(start_test);
    CONNECT(start_iterated_test);

    CONNECT(success_test);
    CONNECT(notification_test);
    CONNECT(omission_test);
    CONNECT(pending_test);
    CONNECT(failure_test);
    CONNECT(error_test);
    CONNECT(crash_test);

    CONNECT(crash_test_suite);

    CONNECT(complete_test);
    CONNECT(complete_iterated_test);
    CONNECT(complete_test_iterator);
    CONNECT(complete_test_case);
    CONNECT(complete_test_suite);
    CONNECT(complete_run);

    CONNECT(error);

#undef CONNECT
}

static void
disconnect_from_run_context (CutConsoleUI *console, CutRunContext *run_context)
{
#define DISCONNECT(name)                                                \
    g_signal_handlers_disconnect_by_func(run_context,                   \
                                         G_CALLBACK(cb_ ## name),       \
                                         console)

    DISCONNECT(start_test_suite);
    DISCONNECT(start_test_case);
    DISCONNECT(start_test_iterator);
    DISCONNECT(start_test);
    DISCONNECT(start_iterated_test);

    DISCONNECT(success_test);
    DISCONNECT(notification_test);
    DISCONNECT(omission_test);
    DISCONNECT(pending_test);
    DISCONNECT(failure_test);
    DISCONNECT(error_test);
    DISCONNECT(crash_test);

    DISCONNECT(crash_test_suite);

    DISCONNECT(complete_iterated_test);
    DISCONNECT(complete_test);
    DISCONNECT(complete_test_iterator);
    DISCONNECT(complete_test_case);
    DISCONNECT(complete_test_suite);
    DISCONNECT(complete_run);

    DISCONNECT(error);

#undef DISCONNECT
}

static void
attach_to_run_context (CutListener *listener,
                       CutRunContext   *run_context)
{
    connect_to_run_context(CUT_CONSOLE_UI(listener), run_context);
}

static void
detach_from_run_context (CutListener *listener,
                         CutRunContext   *run_context)
{
    disconnect_from_run_context(CUT_CONSOLE_UI(listener), run_context);
}

static gboolean
run (CutUI *ui, CutRunContext *run_context)
{
    return cut_run_context_start(run_context);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
