/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Yuto HAYAMIZU <y.hayamizu@gmail.com>
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

#include <soupcutter/soupcutter.h>

#include <cutter/cut-test.h>
#include <cutter/cut-run-context.h>
#include <cutter/cut-test-runner.h>
#include <soupcutter.h>
#include "../lib/cuttest-assertions.h"

void test_message_equal_content_type(void);
void test_client_equal_content_type(void);
void test_client_equal_content_type_null(void);
void test_client_response(void);
void test_client_response_null(void);
void test_client_equal_body(void);
void test_client_equal_body_null(void);
void test_client_match_body(void);

static CutTest *test;
static CutRunContext *run_context;
static CutTestContext *test_context;
static CutTestResult *test_result;
static SoupCutClient *client;

static gint fail_line;

#define MARK_FAIL(assertion) do                 \
{                                               \
    fail_line = __LINE__;                       \
    assertion;                                  \
} while (0)

#define FAIL_LOCATION (cut_take_printf("%s:%d", __FILE__, fail_line))

static gboolean
run (void)
{
    gboolean success;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());

    test_context = cut_test_context_new(run_context, NULL, NULL, NULL, test);
    cut_test_context_current_push(test_context);
    success = cut_test_runner_run_test(CUT_TEST_RUNNER(run_context),
                                       test, test_context);
    cut_test_context_current_pop();

    return success;
}

void
cut_setup (void)
{
    test = NULL;
    run_context = NULL;
    test_context = NULL;
    test_result = NULL;
    client = NULL;

    fail_line = 0;
}

void
cut_teardown (void)
{
    if (test)
        g_object_unref(test);
    if (run_context)
        g_object_unref(run_context);
    if (test_context)
        g_object_unref(test_context);
    if (test_result)
        g_object_unref(test_result);
    if (client)
        g_object_unref(client);
}

static void
stub_message_equal_content_type (void)
{
    SoupMessage *message;

    message = soup_message_new("GET", "http://localhost/");
    soup_message_set_response(message, "text/html", SOUP_MEMORY_COPY, "", 0);

    soupcut_message_assert_equal_content_type("text/html", message);
    MARK_FAIL(soupcut_message_assert_equal_content_type("text/plain", message));
}

void
test_message_equal_content_type (void)
{
    const gchar *message;

    test = cut_test_new("message equal content-type test",
                        stub_message_equal_content_type);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);

    message =
        cut_take_printf("<\"text/plain\" == message[response][content-type]>\n"
                        "  expected: <%s>\n"
                        "    actual: <%s>",
                        "text/plain", "text/html");
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "message equal content-type test",
                           NULL,
                           message,
                           FAIL_LOCATION,
                           "stub_message_equal_content_type");
}

static void
server_callback (SoupServer *server,
                 SoupMessage *msg,
                 const gchar *path,
                 GHashTable *query,
                 SoupClientContext *client,
                 gpointer user_data)
{
    soup_message_set_status(msg, SOUP_STATUS_OK);
    soup_message_set_response(msg, "text/plain", SOUP_MEMORY_COPY,
                              "Hello", 5);
}

static const gchar *
serve (SoupCutClient *client)
{
    SoupServer *server;
    GMainContext *context;
    const gchar *uri;

    context = soupcut_client_get_async_context(client);
    server = soupcut_server_take_new(context);
    uri = soupcut_server_build_uri(server, "/");
    soup_server_add_handler(server, "/", server_callback,
                            NULL, NULL);
    soup_server_run_async(server);

    return uri;
}

static void
stub_client_equal_content_type (void)
{
    const gchar *uri;

    client = soupcut_client_new();
    uri = serve(client);
    soupcut_client_get(client, uri, NULL);

    soupcut_client_assert_equal_content_type("text/plain", client);
    MARK_FAIL(soupcut_client_assert_equal_content_type("text/html", client));
}

void
test_client_equal_content_type (void)
{
    const gchar *message;

    test = cut_test_new("client equal content-type test",
                        stub_client_equal_content_type);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);

    message =
        cut_take_printf("<\"text/html\" == "
                        "latest_message(client)[response][content-type]>\n"
                        "  expected: <%s>\n"
                        "    actual: <%s>",
                        "text/html", "text/plain");
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "client equal content-type test",
                           NULL,
                           message,
                           FAIL_LOCATION,
                           "stub_client_equal_content_type");
}


static void
stub_client_equal_content_type_null (void)
{
    client = soupcut_client_new();
    MARK_FAIL(soupcut_client_assert_equal_content_type("text/plain", client));
}

void
test_client_equal_content_type_null (void)
{
    const gchar *inspected_client, *message;

    test = cut_test_new("client equal content-type test null",
                        stub_client_equal_content_type_null);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);

    inspected_client = cut_take_string(gcut_object_inspect(G_OBJECT(client)));
    message = cut_take_printf("<latest_message(client) != NULL>\n"
                              "    client: <%s>",
                              inspected_client);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "client equal content-type test null",
                           NULL,
                           message,
                           FAIL_LOCATION,
                           "stub_client_equal_content_type_null");
}


static void
stub_client_response (void)
{
    SoupMessage *message;
    const gchar *uri;

    client = soupcut_client_new();
    uri = serve(client);

    message = soup_message_new("GET", uri);
    soupcut_client_send_message(client, message);
    soupcut_client_assert_response(client);

    soup_message_set_status_full(message,
                                 SOUP_STATUS_NOT_FOUND, "Blog Not Found");
    MARK_FAIL(soupcut_client_assert_response(client));
}

void
test_client_response (void)
{
    const gchar *message;

    test = cut_test_new("client response test", stub_client_response);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);

    message =
        "<latest_message(client)[response][status] == 2XX>\n"
        "  expected: <2XX>"
        "    actual: <404>(Blog Not Found)";
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "client response test",
                           NULL,
                           message,
                           FAIL_LOCATION,
                           "stub_client_response");
}


static void
stub_client_response_null (void)
{
    client = soupcut_client_new();
    MARK_FAIL(soupcut_client_assert_response(client));
}

void
test_client_response_null (void)
{
    const gchar *inspected_client, *message;

    test = cut_test_new("client response test null",
                        stub_client_response_null);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);

    inspected_client = cut_take_string(gcut_object_inspect(G_OBJECT(client)));
    message = cut_take_printf("<latest_message(client) != NULL>\n"
                              "    client: <%s>",
                              inspected_client);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "client response test null",
                           NULL,
                           message,
                           FAIL_LOCATION,
                           "stub_client_response_null");
}

static void
stub_client_equal_body (void)
{
    const gchar *uri;

    client = soupcut_client_new();
    uri = serve(client);
    soupcut_client_get(client, uri, NULL);

    soupcut_client_assert_equal_body("Hello", client);
    MARK_FAIL(soupcut_client_assert_equal_body("Goodbye", client));
}

void
test_client_equal_body (void)
{
    const gchar *message;

    test = cut_test_new("client equal body test", stub_client_equal_body);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);

    message =
        "<\"Goodbye\" == latest_message(client)[response][body]>\n"
        "  expected: <Goodbye>\n"
        "    actual: <Hello>";
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "client equal body test",
                           NULL,
                           message,
                           FAIL_LOCATION,
                           "stub_client_equal_body");
}

static void
stub_client_equal_body_null (void)
{
    client = soupcut_client_new();
    MARK_FAIL(soupcut_client_assert_equal_body("Hello", client));
}

void
test_client_equal_body_null (void)
{
    const gchar *inspected_client, *message;

    test = cut_test_new("client equal body test null",
                        stub_client_equal_body_null);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);

    inspected_client = cut_take_string(gcut_object_inspect(G_OBJECT(client)));
    message = cut_take_printf("<latest_message(client) != NULL>\n"
                              "    client: <%s>",
                              inspected_client);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "client equal body test null",
                           NULL,
                           message,
                           FAIL_LOCATION,
                           "stub_client_equal_body_null");
}

static void
stub_client_match_body (void)
{
    const gchar *uri;

    client = soupcut_client_new();
    uri = serve(client);
    soupcut_client_get(client, uri, NULL);

    soupcut_client_assert_match_body("\\w+", client);
    MARK_FAIL(soupcut_client_assert_match_body("\\d+", client));
}

void
test_client_match_body (void)
{
    const gchar *message;

    test = cut_test_new("client match body test", stub_client_match_body);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);

    message =
        "<\"\\\\d+\"> =~ <latest_message(client)[response][body]>\n"
        " pattern: <\\d+>\n"
        "  actual: <Hello>";
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "client match body test",
                           NULL,
                           message,
                           FAIL_LOCATION,
                           "stub_client_match_body");
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
