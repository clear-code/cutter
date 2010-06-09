/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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

#include <cutter.h>
#include <cutter/cut-test-runner.h>
#include <cutter/cut-listener.h>
#include <cutter/cut-stream.h>
#include "../lib/cuttest-utils.h"

#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

void attributes_stream (void);
void data_stream (void);
void test_stream (gconstpointer data);

static CutStream *stream;
static CutRunContext *run_context;
static CutTest *test;
static CutIteratedTest *iterated_test;
static CutTestIterator *test_iterator;
static CutTestCase *test_case;
static CutTestSuite *test_suite;
static CutTestContext *test_context;

static GString *xml;

static void
stub_success_test (void)
{
}

static void
stub_iterated_data (void)
{
    cut_add_data("first data", NULL, NULL,
                 "second data", NULL, NULL,
		 NULL);
}

static void
stub_iterated_test (gconstpointer data)
{
}

void
cut_setup (void)
{
    const gchar *test_names[] = {"/.*/", NULL};

    cut_set_fixture_data_dir(cuttest_get_base_dir(), "fixtures", "xml-stream",
                             NULL);

    test = NULL;
    iterated_test = NULL;
    test_iterator = NULL;
    test_context = NULL;
    stream = NULL;
    xml = NULL;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());
    cut_run_context_set_target_test_names(run_context, test_names);

    test_case = cut_test_case_new("stub test case",
                                  NULL, NULL, NULL, NULL);
    test_suite = cut_test_suite_new_empty();
    cut_test_suite_add_test_case(test_suite, test_case);
}

void
cut_teardown (void)
{
    if (test)
        g_object_unref(test);
    if (iterated_test)
        g_object_unref(iterated_test);
    if (test_iterator)
        g_object_unref(test_iterator);
    if (test_context)
        g_object_unref(test_context);
    if (stream)
        g_object_unref(stream);
    if (test_case)
        g_object_unref(test_case);
    if (test_suite)
        g_object_unref(test_suite);
    g_object_unref(run_context);
    if (xml)
        g_string_free(xml, TRUE);
}

static const gchar *
normalize_xml (const gchar *xml)
{
    const gchar *elapsed_normalized_xml, *start_time_normalized_xml;

    elapsed_normalized_xml = cut_take_replace(xml,
                                              "<elapsed>(.*?)</elapsed>",
                                              "<elapsed>0.000001</elapsed>");
    start_time_normalized_xml =
        cut_take_replace(elapsed_normalized_xml,
                         "<start-time>"
                         "20\\d{2}-" ISO8601_PATTERN_WITHOUT_YEAR
                         "</start-time>",
                         "<start-time>2008-07-30T04:55:42Z</start-time>");

    return start_time_normalized_xml;
}

static gboolean
run (void)
{
    gboolean success;

    test_context = cut_test_context_new(run_context,
                                        NULL, test_case, NULL, test);
    cut_test_context_current_push(test_context);
    success = cut_test_runner_run_test_suite(CUT_TEST_RUNNER(run_context),
                                             test_suite);
    cut_test_context_current_pop();

    g_object_unref(test_context);
    test_context = NULL;
    return success;
}

static gboolean
stream_to_string (const gchar *message, GError **error, gpointer user_data)
{
    GString *string = user_data;

    g_string_append(string, message);

    return TRUE;
}

typedef void (*TestSetupFunction) (void);

typedef struct _StreamTestData
{
    gchar *data_file_name;
    TestSetupFunction test_setup;
} StreamTestData;

static StreamTestData *
stream_test_data_new (const gchar *data_file_name, TestSetupFunction test_setup)
{
    StreamTestData *data;

    data = g_new(StreamTestData, 1);
    data->data_file_name = g_strdup(data_file_name);
    data->test_setup = test_setup;

    return data;
}

static void
stream_test_data_free (StreamTestData *data)
{
    g_free(data->data_file_name);
    g_free(data);
}

static void
setup_success_test (void)
{
    test = cut_test_new("stub-success-test", stub_success_test);
    cut_test_case_add_test(test_case, test);
}

static void
setup_iterated_test (void)
{
    test_iterator = cut_test_iterator_new("stub-iterated-test",
                                          stub_iterated_test,
                                          stub_iterated_data);
    cut_test_case_add_test(test_case, CUT_TEST(test_iterator));
}

void
attributes_stream (void)
{
    cut_set_attributes("multi-thread", "false", NULL);
}

void
data_stream (void)
{
    cut_add_data("test",
                 stream_test_data_new("test.xml", setup_success_test),
                 stream_test_data_free,
                 "iterated test",
                 stream_test_data_new("iterated-test.xml", setup_iterated_test),
                 stream_test_data_free,
		 NULL);
}

void
test_stream (gconstpointer data)
{
    const StreamTestData *test_data = data;
    const gchar *expected;

    test_data->test_setup();

    xml = g_string_new(NULL);
    stream = cut_stream_new("xml",
                            "stream-function", stream_to_string,
                            "stream-function-user-data", xml,
                            NULL);
    cut_listener_attach_to_run_context(CUT_LISTENER(stream), run_context);
    cut_assert(run());
    cut_listener_detach_from_run_context(CUT_LISTENER(stream), run_context);

    expected = cut_get_fixture_data_string(test_data->data_file_name, NULL);
    cut_assert_equal_string(expected, normalize_xml(xml->str));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
