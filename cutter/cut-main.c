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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <locale.h>
#include <glib.h>
#include <glib/gi18n-lib.h>

#ifdef HAVE_LIBBFD
#  include <bfd.h>
#endif

#include "cut-main.h"

#include "cut-listener.h"
#include "cut-test-runner.h"
#include "cut-test-suite.h"
#include "cut-ui.h"
#include "cut-module-factory.h"
#include "cut-contractor.h"
#include "cut-value-equal.h"

static gboolean initialized = FALSE;
static gchar *test_directory = NULL;
static gchar *source_directory = NULL;
static gchar **test_case_names = NULL;
static gchar **test_names = NULL;
static gchar **exclude_files = NULL;
static gchar **exclude_dirs = NULL;
static CutOrder test_case_order = CUT_ORDER_NONE_SPECIFIED;
static gboolean use_multi_thread = FALSE;
static GList *factories = NULL;
static CutContractor *contractor = NULL;
static gchar **original_argv = NULL;

static gboolean
print_version (const gchar *option_name, const gchar *value,
               gpointer data, GError **error)
{
    g_print("%s\n", VERSION);
    exit(0);
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
    {"source-directory", 's', 0, G_OPTION_ARG_STRING, &source_directory,
     N_("Set directory of source code"), "DIRECTORY"},
    {"name", 'n', 0, G_OPTION_ARG_STRING_ARRAY, &test_names,
     N_("Specify tests"), "TEST_NAME"},
    {"test-case", 't', 0, G_OPTION_ARG_STRING_ARRAY, &test_case_names,
     N_("Specify test cases"), "TEST_CASE_NAME"},
    {"multi-thread", 'm', 0, G_OPTION_ARG_NONE, &use_multi_thread,
     N_("Run test cases with multi-thread"), NULL},
    {"test-case-order", 0, 0, G_OPTION_ARG_CALLBACK, parse_test_case_order,
     N_("Sort test case by. Default is 'none'."), "[none|name|name-desc]"},
    {"exclude-file", 0, 0, G_OPTION_ARG_STRING_ARRAY, &exclude_files,
     N_("Skip files"), "FILE_NAME"},
    {"exclude-dir", 0, 0, G_OPTION_ARG_STRING_ARRAY, &exclude_dirs,
     N_("Skip directories"), "DIRECTORY_NAME"},
    {NULL}
};

void
cut_init (int *argc, char ***argv)
{
    int i;
    GOptionContext *option_context;
    GOptionGroup *main_group;
    GError *error = NULL;

    if (initialized)
        return;

    initialized = TRUE;

    original_argv = g_strdupv(*argv);

    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    setlocale(LC_ALL, "");

    g_type_init();
    if (!g_thread_supported())
        g_thread_init(NULL);

    cut_value_equal_init();

    option_context = g_option_context_new("TEST_DIRECTORY");
    g_option_context_set_help_enabled(option_context, FALSE);
    g_option_context_set_ignore_unknown_options(option_context, TRUE);

    contractor = cut_contractor_new();
    cut_contractor_set_option_context(contractor, option_context);

    main_group = g_option_group_new(NULL, NULL, NULL, option_context, NULL);
    g_option_group_add_entries(main_group, option_entries);
    g_option_group_set_translation_domain(main_group, "cutter");
    g_option_context_set_main_group(option_context, main_group);

    if (!g_option_context_parse(option_context, argc, argv, &error)) {
        g_print("%s\n", error->message);
        g_error_free(error);
        g_option_context_free(option_context);
        exit(1);
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
        exit(1);
    }

    if (*argc == 1) {
        gchar *help_string;
        help_string = g_option_context_get_help(option_context, TRUE, NULL);
        g_print("%s", help_string);
        g_free(help_string);
        g_option_context_free(option_context);
        exit(1);
    }

    test_directory = (*argv)[1];

#ifdef HAVE_LIBBFD
    bfd_init();
#endif

    cut_ui_init();

    g_option_context_free(option_context);


    /* to avoid CutTestContextClass initialization is done
     * in multi threads. */
    g_type_class_unref(g_type_class_ref(CUT_TYPE_TEST_CONTEXT));
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

    initialized = FALSE;
}

CutRunContext *
cut_create_run_context (void)
{
    CutRunContext *run_context;

    run_context = cut_test_runner_new();

    cut_run_context_set_test_directory(run_context, test_directory);
    if (source_directory)
        cut_run_context_set_source_directory(run_context, source_directory);
    cut_run_context_set_multi_thread(run_context, use_multi_thread);
    cut_run_context_set_exclude_files(run_context, (const gchar **)exclude_files);
    cut_run_context_set_exclude_directories(run_context, (const gchar **)exclude_dirs);
    cut_run_context_set_target_test_case_names(run_context, test_case_names);
    cut_run_context_set_target_test_names(run_context, test_names);
    cut_run_context_set_test_case_order(run_context, test_case_order);
    cut_run_context_set_command_line_args(run_context, original_argv);

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
    CutUI *ui;

    if (!initialized) {
        g_warning("not initialized");
        return FALSE;
    }

    listeners = create_listeners();
    add_listeners(run_context, listeners);

    ui = get_cut_ui(listeners);
    if (ui)
        success = cut_ui_run(ui, run_context);
    else
        success = cut_run_context_start(run_context);

    remove_listeners(run_context, listeners);
    g_list_free(listeners);

    return success;
}

gboolean
cut_run (void)
{
    CutRunContext *run_context;
    gboolean success = TRUE;

    run_context = cut_create_run_context();
    success = cut_start_run_context(run_context);
    g_object_unref(run_context);

    return success;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
