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

static gint verbose_level = 0;
static gchar *base_dir = NULL;
static gboolean use_color = FALSE;

static const GOptionEntry option_entries[] =
{
    {"verbose", 'v', 0, G_OPTION_ARG_INT, &verbose_level, "Set verbose level", "L"},
    {"base", 'b', 0, G_OPTION_ARG_STRING, &base_dir, "Set base dirirectory of source code", "B"},
    {"color", 'c', 0, G_OPTION_ARG_NONE, &use_color, "Output log with colors", NULL},
    {NULL},
};

static void
cut_context_private_cleanup (gpointer data)
{

}

int
main (int argc, char* argv[])
{
    gboolean success = TRUE;
    GOptionContext *option_context;
    CutTestSuite *suite;
    CutRepository *repository;

    option_context = g_option_context_new("");
    g_option_context_add_main_entries(option_context, option_entries, "cutter");
    g_option_context_parse(option_context, &argc, &argv, NULL);

    g_type_init();

    g_thread_init(NULL);

    cut_context_private = g_private_new(cut_context_private_cleanup);

    cut_context_set_verbose_level(cut_context_get_current(), verbose_level);
    cut_context_set_base_dir(cut_context_get_current(), base_dir);
    cut_context_set_use_color(cut_context_get_current(), use_color);

    repository = cut_repository_new(argv[1]);
    suite = cut_repository_create_test_suite(repository);

    if (suite) {
        success = cut_test_run(CUT_TEST(suite));
        g_object_unref(suite);
    }
    g_object_unref(repository);

    exit(success ? 0 : 1);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
