/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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

#include <glib/gi18n.h>
#include "cut-main.h"
#include "cut-colorize-differ.h"
#include "cut-console.h"
#include "cut-console-diff-writer.h"

static gboolean use_color = FALSE;

static gboolean
print_version (const gchar *option_name, const gchar *value,
               gpointer data, GError **error)
{
    g_print("%s\n", VERSION);
    exit(EXIT_SUCCESS);
    return TRUE;
}

static gboolean
parse_color_arg (const gchar *option_name, const gchar *value,
                 gpointer data, GError **error)
{
    return cut_console_parse_color_arg(option_name, value,
                                       &use_color, error);
}

static const GOptionEntry option_entries[] =
{
    {"version", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, print_version,
     N_("Show version"), NULL},
    {"color", 'c', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK,
     parse_color_arg, N_("Output log with colors"),
     "[yes|true|no|false|auto]"},
    {NULL}
};

static gboolean
pre_parse (GOptionContext *context, GOptionGroup *group, gpointer data,
           GError **error)
{
    use_color = cut_console_guess_color_usability();

    return TRUE;
}

static void
print_diff_header (CutDiffWriter *writer, const gchar *from, const gchar *to)
{
    cut_diff_writer_mark_line(writer, "---", from,
                              CUT_DIFF_WRITER_TAG_DELETED_MARK);
    cut_diff_writer_mark_line(writer, "+++", to,
                              CUT_DIFF_WRITER_TAG_INSERTED_MARK);
}

static gboolean
print_diff (const gchar *from, const gchar *to)
{
    GError *error = NULL;
    gchar *from_contents, *to_contents;
    CutDiffer *differ;
    CutDiffWriter *writer;

    if (!g_file_get_contents(from, &from_contents, NULL, &error)) {
        g_print("%s\n", error->message);
        g_error_free(error);
        return FALSE;
    }

    if (!g_file_get_contents(to, &to_contents, NULL, &error)) {
        g_print("%s\n", error->message);
        g_error_free(error);
        g_free(from_contents);
        return FALSE;
    }

    if (use_color) {
        differ = cut_colorize_differ_new(from_contents, to_contents);
    } else {
        differ = cut_readable_differ_new(from_contents, to_contents);
    }
    writer = cut_console_diff_writer_new(use_color);
    print_diff_header(writer, from, to);
    cut_differ_diff(differ, writer);
    g_object_unref(writer);
    g_object_unref(differ);

    return TRUE;
}

int
main (int argc, char *argv[])
{
    gboolean success = TRUE;
    GOptionContext *option_context;
    GOptionGroup *main_group;
    GError *error = NULL;

    cut_init_environment(&argc, &argv);

    option_context = g_option_context_new(N_("FILE_FROM FILE_TO"));
    g_option_context_set_help_enabled(option_context, TRUE);
    g_option_context_set_ignore_unknown_options(option_context, FALSE);

    main_group = g_option_group_new(NULL, NULL, NULL, option_context, NULL);
    g_option_group_add_entries(main_group, option_entries);
    g_option_group_set_parse_hooks(main_group, pre_parse, NULL);
    g_option_group_set_translation_domain(main_group, GETTEXT_PACKAGE);
    g_option_context_set_main_group(option_context, main_group);

    if (!g_option_context_parse(option_context, &argc, &argv, &error)) {
        g_print("%s\n", error->message);
        g_error_free(error);
        g_option_context_free(option_context);
        exit(EXIT_FAILURE);
    }

    if (argc != 3) {
        gchar *help_string;
        help_string = g_option_context_get_help(option_context, TRUE, NULL);
        g_print("%s", help_string);
        g_free(help_string);
        g_option_context_free(option_context);
        exit(EXIT_FAILURE);
    }

    success = print_diff(argv[1], argv[2]);

    cut_quit_environment();

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
