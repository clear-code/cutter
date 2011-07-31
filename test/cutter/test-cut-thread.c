/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2011  Kouhei Sutou <kou@clear-code.com>
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

#include <gcutter.h>
#include <cutter/cut-pipeline.h>

#include "../lib/cuttest-assertions.h"

void test_failure_not_crashed (void);

static CutRunContext *pipeline;

void
cut_setup (void)
{
    pipeline = cut_pipeline_new();
}

void
cut_teardown (void)
{
    if (pipeline)
        g_object_unref(pipeline);
}

static void
report_error (CutRunContext *context, GError *error, gpointer user_data)
{
    gcut_assert_error(error, cut_message("Pipeline Error"));
}

static gboolean
run (const gchar *test_dir)
{
    cut_run_context_set_test_directory(pipeline, test_dir);
    cut_run_context_set_source_directory(pipeline, test_dir);

    g_signal_connect(pipeline, "error", G_CALLBACK(report_error), NULL);

    return cut_run_context_start(pipeline);
}

void
test_failure_not_crashed (void)
{
    cut_assert_false(run(cut_build_path(cuttest_get_base_dir(),
                                        "fixtures",
                                        "thread",
                                        "failure",
                                        NULL)));
    cut_assert_false(cut_run_context_is_crashed(pipeline));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
