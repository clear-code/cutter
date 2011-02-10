/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2011  Kouhei Sutou <kou@clear-code.com>
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

#include <string.h>

#include <gcutter.h>
#include <cutter/cut-file-stream-reader.h>

#include "../lib/cuttest-assertions.h"

void test_start_all_success (void);
void test_start_error_test (void);

static CutRunContext *reader;
static gchar *path;

void
cut_setup (void)
{
    reader = NULL;
    path = NULL;

    cut_set_fixture_data_dir(cuttest_get_base_dir(),
                             "fixtures",
                             "file-stream-reader",
                             NULL);
}

void
cut_teardown (void)
{
    if (reader)
        g_object_unref(reader);

    if (path)
        g_free(path);
}

void
test_start_all_success (void)
{
    path = cut_build_fixture_data_path("all-success.xml", NULL);
    reader = cut_file_stream_reader_new(path);

    cut_assert_true(cut_run_context_start(reader));
    cut_assert_test_result_summary(reader, 19, 49, 19, 0, 0, 0, 0, 0);
}

void
test_start_error_test (void)
{
    path = cut_build_fixture_data_path("error-test.xml", NULL);
    reader = cut_file_stream_reader_new(path);

    cut_assert_false(cut_run_context_start(reader));
    cut_assert_test_result_summary(reader, 6, 16, 4, 1, 1, 0, 0, 0);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
