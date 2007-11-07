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
#include <glib.h>

#include "cut-context.h"
#include "cut-test-suite.h"
#include "cut-repository.h"

GPrivate *cut_context_private = NULL;

static gchar *verbose_level = NULL;
static gchar *source_directory = NULL;
static gboolean use_color = FALSE;
static gchar **test_case_names = NULL;
static gchar **test_names = NULL;

static const GOptionEntry option_entries[] =
{
    {"verbose", 'v', 0, G_OPTION_ARG_STRING, &verbose_level,
     "Set verbose level", "LEVEL"},
    {"source-directory", 's', 0, G_OPTION_ARG_STRING, &source_directory,
     "Set directory of source code", "DIRECTORY"},
    {"color", 'c', 0, G_OPTION_ARG_NONE, &use_color,
     "Output log with colors", NULL},
    {"name", 'n', 0, G_OPTION_ARG_STRING_ARRAY, &test_case_names,
     "Specify test cases", "TEST_CASE_NAME1,TEST_CASE_NAME2,..."},
    {"test", 't', 0, G_OPTION_ARG_STRING_ARRAY, &test_names,
     "Specify tests", "TEST_NAME1,TEST_NAME2,..."},
    {NULL}
};

static void
cut_context_private_cleanup (gpointer data)
{

}

static void
show_no_argument_error (GOptionContext *option_context)
{
    gchar *help_string;

    g_print("You should specify directory stored in shared object or execution file.\n");

    help_string = g_option_context_get_help(option_context,
                                            TRUE, NULL);
    g_print("%s", help_string);
    g_free(help_string);
}

static gboolean
run_tests (CutTestSuite *suite, CutContext *context)
{
    gboolean all_success = TRUE;
    gboolean success = FALSE;

    if (test_names && test_case_names) {
        gint i, j;
        for (i = 0; test_case_names[i] != NULL; i++) {
            for (j = 0; test_names[i] != NULL; j++) {
                success = cut_test_suite_run_test_function_in_test_case(suite,
                                                                        context,
                                                                        test_case_names[i],
                                                                        test_names[j]);
                if (!success)
                    all_success = FALSE;
            }
        }
    } else if (test_case_names) {
        gint i;
        for (i = 0; test_case_names[i] != NULL; i++) {
            success = cut_test_suite_run_test_case(suite, context,
                                                   test_case_names[i]);
            if (!success)
                all_success = FALSE;
        }
    } else if (test_names) {
        gint i;
        for (i = 0; test_names[i] != NULL; i++) {
            success = cut_test_suite_run_test_function(suite, context, test_names[i]);
            if (!success)
                all_success = FALSE;
        }
    } else {
        all_success = cut_test_suite_run(suite, context);
    }

    return all_success;
}

int
main (int argc, char *argv[])
{
    gboolean success = TRUE;
    GOptionContext *option_context;
    CutTestSuite *suite;
    CutRepository *repository;
    CutContext *context;

    option_context = g_option_context_new("");
    g_option_context_add_main_entries(option_context, option_entries, "cutter");
    g_option_context_parse(option_context, &argc, &argv, NULL);

    if (argc == 1) {
        show_no_argument_error(option_context);
        g_option_context_free(option_context);
        exit(1);
    }

    g_type_init();

    g_thread_init(NULL);

    context = cut_context_new();
    cut_context_private = g_private_new(cut_context_private_cleanup);
    g_private_set(cut_context_private, context);

    cut_context_set_verbose_level_by_name(context, verbose_level);
    if (source_directory) {
        cut_context_set_source_directory(context, source_directory);
    } else {
        cut_context_set_source_directory(context, argv[1]);
    }
    cut_context_set_use_color(context, use_color);

    repository = cut_repository_new(argv[1]);
    suite = cut_repository_create_test_suite(repository);

    if (suite) {
        success = run_tests(suite, context);
        g_object_unref(suite);
    }
    g_object_unref(repository);
    g_option_context_free(option_context);

    exit(success ? 0 : 1);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
