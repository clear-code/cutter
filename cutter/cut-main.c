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
#include <locale.h>
#include <glib.h>
#include <glib/gi18n-lib.h>

#ifdef HAVE_LIBBFD
#  include <bfd.h>
#endif

#ifdef NEED_ENVIRON_DECLARATION
char **environ = NULL;
#endif

#include "cut-main.h"

#include "cut-listener.h"
#include "cut-analyzer.h"
#include "cut-file-stream-reader.h"
#include "cut-test-runner.h"
#include "cut-test-suite.h"
#include "cut-ui.h"
#include "cut-module-factory.h"
#include "cut-contractor.h"
#include "cut-utils.h"
#include "../gcutter/gcut-main.h"
#include "../gcutter/gcut-error.h"

#ifdef G_OS_WIN32
#  include <winsock2.h>
#endif

#if GLIB_CHECK_VERSION(2, 36, 0)
#  define g_type_init()
#endif

#if GLIB_CHECK_VERSION(2, 32, 0)
#  define g_thread_supported() TRUE
#  define g_thread_init(vtable)
#endif

typedef enum {
    MODE_TEST,
    MODE_ANALYZE,
    MODE_PLAY
} RunMode;

static gboolean environment_initialized = FALSE;
static gboolean initialized = FALSE;
static RunMode mode = MODE_TEST;
static gchar *test_directory = NULL;
static gchar *log_directory = NULL;
static gchar *log_file = NULL;
static gchar *source_directory = NULL;
static gchar **test_case_names = NULL;
static gchar **test_names = NULL;
static gchar **exclude_files = NULL;
static gchar **exclude_directories = NULL;
static CutOrder test_case_order = CUT_ORDER_NONE_SPECIFIED;
static gboolean use_multi_thread = FALSE;
static gint max_threads = 10;
static gboolean disable_signal_handling = FALSE;
static GList *factories = NULL;
static CutContractor *contractor = NULL;
static gchar **original_argv = NULL;
static gchar *cutter_command_path = NULL;
static gboolean fatal_failures = FALSE;
static gboolean keep_opening_modules = FALSE;
static gboolean enable_convenience_attribute_definition = FALSE;
static gboolean stop_before_test = FALSE;

static gboolean
print_version (const gchar *option_name, const gchar *value,
               gpointer data, GError **error)
{
    g_print("%s\n", VERSION);
    exit(EXIT_SUCCESS);
    return TRUE;
}

static gboolean
parse_mode (const gchar *option_name, const gchar *value,
            gpointer data, GError **error)
{
    if (g_utf8_collate(value, "test") == 0) {
        mode = MODE_TEST;
    } else if (g_utf8_collate(value, "analyze") == 0) {
        mode = MODE_ANALYZE;
    } else if (g_utf8_collate(value, "play") == 0) {
        mode = MODE_PLAY;
    } else {
        g_set_error(error,
                    G_OPTION_ERROR,
                    G_OPTION_ERROR_BAD_VALUE,
                    _("Invalid run mode: %s"), value);
        return FALSE;
    }

    return TRUE;
}

static gboolean
parse_test_case_order (const gchar *option_name, const gchar *value,
                       gpointer data, GError **error)
{
    if (g_utf8_collate(value, "none") == 0) {
        test_case_order = CUT_ORDER_NONE_SPECIFIED;
    } else if (g_utf8_collate(value, "name") == 0) {
        test_case_order = CUT_ORDER_NAME_ASCENDING;
    } else if (g_utf8_collate(value, "name-desc") == 0) {
        test_case_order = CUT_ORDER_NAME_DESCENDING;
    } else {
        g_set_error(error,
                    G_OPTION_ERROR,
                    G_OPTION_ERROR_BAD_VALUE,
                    _("Invalid test case order value: %s"), value);
        return FALSE;
    }

    return TRUE;
}

static const GOptionEntry option_entries[] =
{
    {"version", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, print_version,
     N_("Show version"), NULL},
    {"mode", 0, 0, G_OPTION_ARG_CALLBACK, parse_mode,
     N_("Set run mode (default: test)"), "[test|analyze|play]"},
    {"source-directory", 's', 0, G_OPTION_ARG_STRING, &source_directory,
     N_("Set directory of source code"), "DIRECTORY"},
    {"name", 'n', 0, G_OPTION_ARG_STRING_ARRAY, &test_names,
     N_("Specify tests"), "TEST_NAME"},
    {"test-case", 't', 0, G_OPTION_ARG_STRING_ARRAY, &test_case_names,
     N_("Specify test cases"), "TEST_CASE_NAME"},
    {"multi-thread", 'm', 0, G_OPTION_ARG_NONE, &use_multi_thread,
     N_("Run test cases and iterated tests with multi-thread"), NULL},
    {"max-threads", 0, 0, G_OPTION_ARG_INT, &max_threads,
     N_("Run test cases and iterated tests with MAX_THREADS threads "
        "concurrently at a maximum "
        "(default: 10; -1 is no limit)"),
     "MAX_THREADS"},
    {"disable-signal-handling", 0, 0, G_OPTION_ARG_NONE,
     &disable_signal_handling,
     N_("Disable signal handling"), NULL},
    {"test-case-order", 0, 0, G_OPTION_ARG_CALLBACK, parse_test_case_order,
     N_("Sort test case by. Default is 'none'."), "[none|name|name-desc]"},
    {"exclude-file", 0, 0, G_OPTION_ARG_STRING_ARRAY, &exclude_files,
     N_("Skip files"), "FILE"},
    {"exclude-directory", 0, 0, G_OPTION_ARG_STRING_ARRAY, &exclude_directories,
     N_("Skip directories"), "DIRECTORY"},
    {"fatal-failures", 0, 0, G_OPTION_ARG_NONE, &fatal_failures,
     N_("Treat failures as fatal problem"), NULL},
    {"keep-opening-modules", 0, 0, G_OPTION_ARG_NONE, &keep_opening_modules,
     N_("Keep opening loaded modules to resolve symbols for debugging"), NULL},
    {"enable-convenience-attribute-definition", 0, 0, G_OPTION_ARG_NONE,
     &enable_convenience_attribute_definition,
     N_("Enable convenience but danger "
        "'#{ATTRIBUTE_NAME}_#{TEST_NAME - 'test_' PREFIX}' "
        "attribute set function"), NULL},
    {"stop-before-test", 0, 0, G_OPTION_ARG_NONE,
     &stop_before_test,
     N_("Set breakpoints at each line which invokes test. "
        "You can step into a test function with your debugger easily."), NULL},
    {NULL}
};

void
cut_init_environment (int *argc, char ***argv)
{
    if (environment_initialized)
        return;

    environment_initialized = TRUE;

    cut_set_cutter_command_path((*argv)[0]);

    setlocale(LC_ALL, "");

#ifdef G_OS_WIN32
    {
        gchar *locale_dir;
        locale_dir = g_build_filename(cut_win32_base_path(),
                                      "share", "locale", NULL);
        bindtextdomain(GETTEXT_PACKAGE, locale_dir);
        g_free(locale_dir);
    }
#else
    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
#endif
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    g_type_init();
    if (!g_thread_supported())
        g_thread_init(NULL);

    gcut_init();
}

void
cut_init (int *argc, char ***argv)
{
    int i;
    GOptionContext *option_context;
    GOptionGroup *main_group;
    GError *error = NULL;
    gchar *program_name, *parameter_string;

    if (initialized)
        return;

    initialized = TRUE;

    original_argv = g_strdupv(*argv);

    cut_init_environment(argc, argv);

    cut_test_context_current_init();

    program_name = g_path_get_basename((*argv)[0]);
    parameter_string =
        g_strdup_printf(N_("TEST_DIRECTORY\n"
                           "  %s --mode=analyze %s LOG_DIRECTORY\n"
                           "  %s --mode=play %s LOG_FILE"),
                        program_name, _("[OPTION...]"),
                        program_name, _("[OPTION...]"));
    option_context = g_option_context_new(parameter_string);
    g_free(program_name);
    g_free(parameter_string);
    g_option_context_set_help_enabled(option_context, FALSE);
    g_option_context_set_ignore_unknown_options(option_context, TRUE);

    contractor = cut_contractor_new();
    cut_contractor_set_option_context(contractor, option_context);

    main_group = g_option_group_new(NULL, NULL, NULL, option_context, NULL);
    g_option_group_add_entries(main_group, option_entries);
    g_option_group_set_translation_domain(main_group, GETTEXT_PACKAGE);
    g_option_context_set_main_group(option_context, main_group);

    if (!g_option_context_parse(option_context, argc, argv, &error)) {
        g_print("%s\n", error->message);
        g_error_free(error);
        g_option_context_free(option_context);
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < *argc; i++) {
        if (g_str_has_prefix((*argv)[i], "--help-")) {
            factories = cut_contractor_build_all_factories(contractor);
            break;
        }
    }

    if (!factories)
        factories = cut_contractor_build_factories(contractor);

    g_option_context_set_help_enabled(option_context, TRUE);
    g_option_context_set_ignore_unknown_options(option_context, FALSE);
    if (!g_option_context_parse(option_context, argc, argv, &error)) {
        g_print("%s\n", error->message);
        g_error_free(error);
        g_option_context_free(option_context);
        exit(EXIT_FAILURE);
    }

    if (*argc == 1) {
#if GLIB_CHECK_VERSION(2, 14, 0)
        gchar *help_string;
        help_string = g_option_context_get_help(option_context, TRUE, NULL);
        g_print("%s", help_string);
        g_free(help_string);
        g_option_context_free(option_context);
#else
        gint help_argc = 2;
        gchar *help_argv[2];
        gchar **help_argv_p;
        help_argv[0] = (*argv)[0];
        help_argv[1] = "--help";
        help_argv_p = help_argv;
        g_option_context_parse(option_context, &help_argc, &help_argv_p, NULL);
        g_option_context_free(option_context);
#endif
        exit(EXIT_FAILURE);
    }

    switch (mode) {
    case MODE_TEST:
        test_directory = (*argv)[1];
        break;
    case MODE_ANALYZE:
        log_directory = (*argv)[1];
        break;
    case MODE_PLAY:
        log_file = (*argv)[1];
        break;
    }

#ifdef HAVE_LIBBFD
    bfd_init();
#endif

#ifdef G_OS_WIN32
    {
        WSADATA wsa_data;
        WSAStartup(MAKEWORD(2, 2), &wsa_data);
    }
#endif

    cut_ui_init();

    g_option_context_free(option_context);


    /* to avoid CutTestContextClass initialization is done
     * in multi threads. */
    g_type_class_unref(g_type_class_ref(CUT_TYPE_TEST_CONTEXT));
}

void
cut_quit_environment (void)
{
    if (!environment_initialized)
        return;

    gcut_quit();

    environment_initialized = FALSE;
}

void
cut_quit (void)
{
    if (!initialized)
        return;

    cut_ui_quit();

    if (contractor) {
        g_object_unref(contractor);
        contractor = NULL;
    }

    if (original_argv) {
        g_strfreev(original_argv);
        original_argv = NULL;
    }

    if (cutter_command_path) {
        g_free(cutter_command_path);
        cutter_command_path = NULL;
    }

    cut_test_context_current_quit();

    cut_quit_environment();

#ifdef G_OS_WIN32
    WSACleanup();
#endif

    initialized = FALSE;
}

void
cut_setup_run_context (CutRunContext *run_context)
{
    cut_run_context_set_test_directory(run_context, test_directory);
    cut_run_context_set_log_directory(run_context, log_directory);
    if (source_directory)
        cut_run_context_set_source_directory(run_context, source_directory);
    cut_run_context_set_multi_thread(run_context, use_multi_thread);
    cut_run_context_set_max_threads(run_context, max_threads);
    cut_run_context_set_handle_signals(run_context, !disable_signal_handling);
    cut_run_context_set_exclude_files(run_context,
                                      (const gchar **)exclude_files);
    cut_run_context_set_exclude_directories(run_context,
                                            (const gchar **)exclude_directories);
    cut_run_context_set_target_test_case_names(run_context,
                                               (const gchar **)test_case_names);
    cut_run_context_set_target_test_names(run_context,
                                          (const gchar **)test_names);
    cut_run_context_set_test_case_order(run_context, test_case_order);
    cut_run_context_set_fatal_failures(run_context, fatal_failures);
    cut_run_context_set_keep_opening_modules(run_context, keep_opening_modules);
    cut_run_context_set_enable_convenience_attribute_definition(run_context,
                                                                enable_convenience_attribute_definition);
    cut_run_context_set_stop_before_test(run_context, stop_before_test);
    cut_run_context_set_command_line_args(run_context, original_argv);
}

CutRunContext *
cut_create_run_context (void)
{
    CutRunContext *run_context;

    run_context = cut_test_runner_new();
    cut_setup_run_context(run_context);
    return run_context;
}

static GList *
create_listeners (void)
{
    GList *listeners = NULL, *node;

    for (node = factories; node; node = g_list_next(node)) {
        GObject *listener;
        listener = cut_module_factory_create(CUT_MODULE_FACTORY(node->data));
        listeners = g_list_prepend(listeners, listener);
    }

    return listeners;
}

static void
add_listeners (CutRunContext *run_context, GList *listeners)
{
    GList *node;

    for (node = listeners; node; node = g_list_next(node)) {
        CutListener *listener = CUT_LISTENER(node->data);
        cut_run_context_add_listener(run_context, listener);
    }
}

static void
remove_listeners (CutRunContext *run_context, GList *listeners)
{
    GList *node;

    for (node = listeners; node; node = g_list_next(node)) {
        CutListener *listener = CUT_LISTENER(node->data);
        cut_run_context_remove_listener(run_context, listener);
        g_object_unref(listener);
    }
}

static CutUI *
get_cut_ui (GList *listeners)
{
    GList *node;

    for (node = listeners; node; node = g_list_next(node)) {
        CutListener *listener = node->data;

        if (CUT_IS_UI(listener)) {
            return CUT_UI(listener);
        }
    }

    return NULL;
}

gboolean
cut_start_run_context (CutRunContext *run_context)
{
    gboolean success;
    GList *listeners;
    CutUI *ui = NULL;

    if (!initialized) {
        g_warning("not initialized");
        return FALSE;
    }

    listeners = create_listeners();
    add_listeners(run_context, listeners);

    switch (mode) {
    case MODE_TEST:
    case MODE_PLAY:
        ui = get_cut_ui(listeners);
        break;
    case MODE_ANALYZE:
        ui = NULL;
        break;
    }

    if (ui)
        success = cut_ui_run(ui, run_context);
    else
        success = cut_run_context_start(run_context);

    remove_listeners(run_context, listeners);
    g_list_free(listeners);

    return success;
}

static gboolean
cut_run_in_test_mode (void)
{
    CutRunContext *run_context;
    gboolean success;

    run_context = cut_create_run_context();
    success = cut_start_run_context(run_context);
    g_object_unref(run_context);

    return success;
}

static gboolean
cut_run_in_analyze_mode (void)
{
    CutAnalyzer *analyzer;
    GError *error = NULL;
    gboolean success;

    analyzer = cut_analyzer_new();
    success = cut_analyzer_analyze(analyzer, log_directory, &error);
    if (error) {
        cut_utils_report_error(error);
    }
    g_object_unref(analyzer);

    return success;
}


static gboolean
cut_run_in_play_mode (void)
{
    CutRunContext *run_context;
    gboolean success;

    run_context = cut_file_stream_reader_new(log_file);
    cut_setup_run_context(run_context);
    success = cut_start_run_context(run_context);
    g_object_unref(run_context);

    return success;
}

gboolean
cut_run (void)
{
    gboolean success = TRUE;

    switch (mode) {
    case MODE_TEST:
        success = cut_run_in_test_mode();
        break;
    case MODE_ANALYZE:
        success = cut_run_in_analyze_mode();
        break;
    case MODE_PLAY:
        success = cut_run_in_play_mode();
        break;
    }

    return success;
}

gboolean
cut_run_iteration (void)
{
    /* static GStaticRecMutex mutex = G_STATIC_REC_MUTEX_INIT; */
    gboolean dispatched;

    /* should lock? */
    /* g_static_rec_mutex_lock(&mutex); */
    dispatched = g_main_context_iteration(NULL, FALSE);
    /* g_static_rec_mutex_unlock(&mutex); */

    return dispatched;
}

const gchar *
cut_get_cutter_command_path (void)
{
    return cutter_command_path;
}

void
cut_set_cutter_command_path (const gchar *argv0)
{
    if (cutter_command_path)
        g_free(cutter_command_path);
    cutter_command_path = NULL;

    if (argv0 && g_path_is_absolute(argv0))
        cutter_command_path = g_strdup(argv0);

    if (!cutter_command_path && argv0)
        cutter_command_path = g_find_program_in_path(argv0);

    if (!cutter_command_path) {
        gchar *current_dir;

        current_dir = g_get_current_dir();
        cutter_command_path = g_build_filename(current_dir, argv0, NULL);
        g_free(current_dir);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
