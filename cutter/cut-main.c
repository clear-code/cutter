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

#ifdef HAVE_LIBBFD
#  include <bfd.h>
#endif

#include "cut-main.h"

#include "cut-context.h"
#include "cut-test-suite.h"
#include "cut-repository.h"
#include "cut-verbose-level.h"

static CutVerboseLevel verbose_level = CUT_VERBOSE_LEVEL_NORMAL;
static gchar *source_directory = NULL;
static gboolean use_color = FALSE;
static gchar **test_case_names = NULL;
static gchar **test_names = NULL;
static gboolean use_multi_thread = FALSE;

static gboolean
parse_verbose_level_arg (const gchar *option_name, const gchar *value,
                         gpointer data, GError **error)
{
    GError *verbose_level_error = NULL;

    verbose_level = cut_verbose_level_parse(value, &verbose_level_error);
    if (!verbose_level_error)
        return TRUE;

    g_set_error(error,
                G_OPTION_ERROR,
                G_OPTION_ERROR_BAD_VALUE,
                "%s", verbose_level_error->message);
    g_error_free(verbose_level_error);
    return FALSE;
}

static gboolean
parse_color_arg (const gchar *option_name, const gchar *value,
                 gpointer data, GError **error)
{
    if (value == NULL ||
        g_utf8_collate(value, "yes") == 0 ||
        g_utf8_collate(value, "true") == 0) {
        use_color = TRUE;
    } else if (g_utf8_collate(value, "no") == 0 ||
               g_utf8_collate(value, "false") == 0) {
        use_color = FALSE;
    } else if (g_utf8_collate(value, "auto") == 0) {
        const gchar *term;
        term = g_getenv("TERM");
        use_color = term && g_str_has_suffix(term, "term");
    } else {
        g_set_error(error,
                    G_OPTION_ERROR,
                    G_OPTION_ERROR_BAD_VALUE,
                    "Invalid color value: %s", value);
        return FALSE;
    }

    return TRUE;
}

static const GOptionEntry option_entries[] =
{
    {"verbose", 'v', 0, G_OPTION_ARG_CALLBACK, parse_verbose_level_arg,
     "Set verbose level", "[s|silent|n|normal|v|verbose]"},
    {"source-directory", 's', 0, G_OPTION_ARG_STRING, &source_directory,
     "Set directory of source code", "DIRECTORY"},
    {"color", 'c', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK,
     parse_color_arg, "Output log with colors", "[yes|true|no|false|auto]"},
    {"name", 'n', 0, G_OPTION_ARG_STRING_ARRAY, &test_case_names,
     "Specify test cases", "TEST_CASE_NAME1,TEST_CASE_NAME2,..."},
    {"test", 't', 0, G_OPTION_ARG_STRING_ARRAY, &test_names,
     "Specify tests", "TEST_NAME1,TEST_NAME2,..."},
    {"multi-thread", 'm', 0, G_OPTION_ARG_NONE, &use_multi_thread,
     "Run test cases with multi-thread", NULL},
    {NULL}
};

void
cut_init (int *argc, char ***argv)
{
    GOptionContext *option_context;
    GError *error = NULL;

    option_context = g_option_context_new("TEST_DIRECTORY");
    g_option_context_add_main_entries(option_context, option_entries, "cutter");
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

    g_type_init();

    g_thread_init(NULL);

#ifdef HAVE_LIBBFD
    bfd_init();
#endif

    cut_output_init();

    g_option_context_free(option_context);
}

gboolean
cut_run (const char *directory)
{
    CutContext *context;
    gboolean success = TRUE;
    CutTestSuite *suite;
    CutRepository *repository;

    context = cut_context_new();

    cut_context_set_verbose_level(context, verbose_level);
    if (source_directory) {
        cut_context_set_source_directory(context, source_directory);
    } else {
        cut_context_set_source_directory(context, directory);
    }
    cut_context_set_use_color(context, use_color);
    cut_context_set_multi_thread(context, use_multi_thread);


    repository = cut_repository_new(directory);
    suite = cut_repository_create_test_suite(repository);
    if (suite) {
        success = cut_test_suite_run_with_filter(suite, context,
                                                 test_case_names, test_names);
        g_object_unref(suite);
    }
    g_object_unref(repository);

    g_object_unref(context);

    return success;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
