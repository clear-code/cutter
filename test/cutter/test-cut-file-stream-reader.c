/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>
#include <cutter/cut-file-stream-reader.h>

#include "../lib/cuttest-assertions.h"

void test_start_all_success (void);
void test_start_error_test (void);

static CutRunContext *reader;
static gchar *path;

void
setup (void)
{
    reader = NULL;
    path = NULL;

    cut_set_fixture_data_dir(cuttest_get_base_dir(),
                             "fixtures",
                             "file-stream-reader",
                             NULL);
}

void
teardown (void)
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
