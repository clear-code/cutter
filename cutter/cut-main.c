/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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

#include "cut-runner.h"
#include "cut-test-suite.h"
#include "cut-ui.h"
#include "cut-module-factory.h"
#include "cut-value-equal.h"

static gboolean initialized = FALSE;
static gchar *test_directory = NULL;
static gchar *source_directory = NULL;
static const gchar **test_case_names = NULL;
static const gchar **test_names = NULL;
static gboolean use_multi_thread = FALSE;
static const gchar *ui_name = NULL;
static gboolean _show_all_uis = FALSE;

static CutModuleFactory *factory = NULL;

static const GOptionEntry option_entries[] =
{
    {"source-directory", 's', 0, G_OPTION_ARG_STRING, &source_directory,
     N_("Set directory of source code"), "DIRECTORY"},
    {"name", 'n', 0, G_OPTION_ARG_STRING_ARRAY, &test_names,
     N_("Specify tests"), "TEST_NAME"},
    {"test-case", 't', 0, G_OPTION_ARG_STRING_ARRAY, &test_case_names,
     N_("Specify test cases"), "TEST_CASE_NAME"},
    {"multi-thread", 'm', 0, G_OPTION_ARG_NONE, &use_multi_thread,
     N_("Run test cases with multi-thread"), NULL},
    {"ui", 'u', 0, G_OPTION_ARG_STRING, &ui_name,
     N_("Specify UI"), "[console|gtk]"},
    {"show-all-uis", 0, 0, G_OPTION_ARG_NONE, &_show_all_uis,
     N_("Show all available UIs and exit"), NULL},
    {NULL}
};

static void
show_all_uis (void)
{
    GList *names, *node;

    cut_module_factory_load(NULL);
    names = cut_module_factory_get_names("ui");
    for (node = names; node; node = g_list_next(node)) {
        const gchar *name = node->data;
        if (g_list_next(node))
            g_print("%s, ", name);
        else
            g_print("%s", name);
    }
    g_print("\n");
    g_list_foreach(names, (GFunc)g_free, NULL);
    g_list_free(names);
}

void
cut_init (int *argc, char ***argv)
{
    GOptionContext *option_context;
    GError *error = NULL;

    if (initialized)
        return;

    initialized = TRUE;

    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    setlocale(LC_ALL, "");

    g_type_init();
    if (!g_thread_supported())
        g_thread_init(NULL);

    cut_value_equal_init();

    option_context = g_option_context_new("TEST_DIRECTORY");
    g_option_context_add_main_entries(option_context, option_entries, "cutter");
    g_option_context_set_help_enabled(option_context, FALSE);
    g_option_context_set_ignore_unknown_options(option_context, TRUE);
    if (!g_option_context_parse(option_context, argc, argv, &error)) {
        g_print("%s\n", error->message);
        g_error_free(error);
        g_option_context_free(option_context);
        exit(1);
    }

    cut_module_factory_init();
    cut_module_factory_load(NULL);
    if (_show_all_uis) {
        show_all_uis();
        exit(1);
    }

    if (!ui_name)
        ui_name = "console";
    factory = cut_module_factory_new("ui", ui_name, NULL);
    if (!factory) {
        g_warning("can't find specified UI: %s", ui_name);
        exit(1);
    }
    cut_module_factory_set_option_group(factory, option_context);

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

    if (factory)
        g_object_unref(factory);
    factory = NULL;

    cut_ui_quit();
    cut_module_factory_quit();

    initialized = FALSE;
}

CutRunner *
cut_create_runner (void)
{
    CutRunner *runner;

    runner = cut_runner_new();

    cut_runner_set_test_directory(runner, test_directory);
    if (source_directory)
        cut_runner_set_source_directory(runner, source_directory);
    cut_runner_set_multi_thread(runner, use_multi_thread);
    cut_runner_set_target_test_case_names(runner, test_case_names);
    cut_runner_set_target_test_names(runner, test_names);

    return runner;
}

gboolean
cut_run_runner (CutRunner *runner)
{
    CutUI *ui;
    gboolean success;

    if (!initialized) {
        g_warning("not initialized");
        return FALSE;
    }

    ui = CUT_UI(cut_module_factory_create(factory));
    if (!ui) {
        g_warning("can't create UI: %s", ui_name);
        return FALSE;
    }

    cut_runner_add_listener(runner, CUT_LISTENER(ui)); 
    success = cut_runner_run(runner);
    g_object_unref(ui);
    return success;
}

gboolean
cut_run (void)
{
    CutRunner *runner;
    gboolean success = TRUE;

    runner = cut_create_runner();
    success = cut_run_runner(runner);
    g_object_unref(runner);

    return success;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
