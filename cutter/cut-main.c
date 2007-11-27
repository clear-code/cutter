/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#include "cut-context.h"
#include "cut-test-suite.h"
#include "cut-repository.h"
#include "cut-ui.h"
#include "cut-ui-factory.h"

static gboolean initialized = FALSE;
static gchar *source_directory = NULL;
static const gchar **test_case_names = NULL;
static const gchar **test_names = NULL;
static gboolean use_multi_thread = FALSE;
static const gchar *runner_name = NULL;
static gboolean _show_all_runners = FALSE;

static CutUIFactory *factory = NULL;

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
    {"runner", 'r', 0, G_OPTION_ARG_STRING, &runner_name,
     N_("Specify test runner"), "[console|gtk]"},
    {"show-all-runners", 0, 0, G_OPTION_ARG_NONE, &_show_all_runners,
     N_("Show all available runners and exit"), NULL},
    {NULL}
};

static void
show_all_runners (void)
{
    GList *names, *node;

    cut_ui_factory_load(NULL);
    names = cut_ui_factory_get_names();
    for (node = names; node; node = g_list_next(node)) {
        const gchar *name = node->data;
        if (g_list_next(node))
            g_print("%s, ", name);
        else
            g_print("%s", name);
    }
    g_print("\n");
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
	if (!g_thread_supported()) g_thread_init(NULL);

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

    if (_show_all_runners) {
        show_all_runners();
        exit(1);
    }

    cut_ui_factory_init();
    if (!runner_name)
        runner_name = "console";
    factory = cut_ui_factory_new(runner_name, NULL);
    if (!factory) {
        g_warning("can't find specified runner: %s", runner_name);
        exit(1);
    }
    cut_ui_factory_set_option_group(factory, option_context);

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

#ifdef HAVE_LIBBFD
    bfd_init();
#endif

    cut_ui_init();

    g_option_context_free(option_context);
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
    cut_ui_factory_quit();

    initialized = FALSE;
}

CutContext *
cut_create_context (void)
{
    CutContext *context;

    context = cut_context_new();

    if (source_directory)
        cut_context_set_source_directory(context, source_directory);
    cut_context_set_multi_thread(context, use_multi_thread);
    cut_context_set_target_test_case_names(context, test_case_names);
    cut_context_set_target_test_names(context, test_names);

    return context;
}

CutTestSuite *
cut_create_test_suite (const gchar *directory)
{
    CutRepository *repository;
    CutTestSuite *suite;

    repository = cut_repository_new(directory);
    suite = cut_repository_create_test_suite(repository);
    g_object_unref(repository);

    return suite;
}

gboolean
cut_run_test_suite (CutTestSuite *suite, CutContext *context)
{
    CutUI *runner;
    gboolean success;

    if (!initialized) {
        g_warning("not initialized");
        return FALSE;
    }

    if (!suite)
        return TRUE;

    runner = cut_ui_factory_create(factory);
    if (!runner) {
        g_warning("can't create runner: %s", runner_name);
        return FALSE;
    }

    success = cut_ui_run(runner, suite, context);
    g_object_unref(runner);
    return success;
}

gboolean
cut_run (const gchar *directory)
{
    CutContext *context;
    CutTestSuite *suite;
    gboolean success = TRUE;

    context = cut_create_context();
    if (!cut_context_get_source_directory(context))
        cut_context_set_source_directory(context, directory);

    suite = cut_create_test_suite(directory);
    if (suite) {
        success = cut_run_test_suite(suite, context);
        g_object_unref(suite);
    }

    g_object_unref(context);

    return success;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
