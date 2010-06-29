/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007, 2009  Kouhei Sutou <kou@clear-code.com>
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

#include <glib.h>

static gboolean output_to_stderr = FALSE;
static const gchar parameter_string[] = "STRING";

static const GOptionEntry option_entries[] =
{
    {"stderr", 'e', 0, G_OPTION_ARG_NONE, &output_to_stderr,
     ("Output string to stderr"), NULL},
    {NULL}
};

int
main (int argc, char *argv[])
{
    GError *error = NULL;
    gboolean success = FALSE;
    GOptionContext *option_context;

    option_context = g_option_context_new(parameter_string);
    g_option_context_add_main_entries(option_context, option_entries, NULL);
    if (!g_option_context_parse(option_context, &argc, &argv, &error)) {
        g_print("%s\n", error->message);
        g_error_free(error);
        g_option_context_free(option_context);
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
        if (output_to_stderr)
            g_printerr("%s\n", argv[1]);
        else
            g_print("%s\n", argv[1]);
        success = TRUE;
    }
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
