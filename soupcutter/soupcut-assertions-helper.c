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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "soupcut-assertions-helper.h"
#include "soupcutter.h"

static void
soupcut_test_fail_null_message (SoupCutClient *client,
                                const gchar *expression_client)
{
    GString *message;
    const gchar *fail_message;
    const gchar *inspected_client;

    message = g_string_new(NULL);
    g_string_append_printf(message,
                           "<latest_message(%s) != NULL>\n",
                           expression_client);
    inspected_client = gcut_object_inspect(G_OBJECT(client));
    g_string_append_printf(message,
                           "    client: <%s>",
                           inspected_client);
    fail_message = cut_take_string(g_string_free(message, FALSE));
    cut_test_fail(fail_message);
}

static void
soupcut_find_content_type (const gchar *name, const gchar *value,
                           gpointer user_data)
{
    const gchar **content_type = user_data;

    if (cut_utils_equal_string(name, "Content-Type")){
        *content_type = value;
    }
}

static const gchar *
soupcut_message_get_content_type (SoupMessage *message)
{
    const gchar *content_type = NULL;

    if (!message->response_headers)
        return NULL;

    soup_message_headers_foreach(message->response_headers,
                                 soupcut_find_content_type,
                                 &content_type);
    return content_type;
}

void
soupcut_message_assert_equal_content_type_helper (const gchar *expected,
                                                  SoupMessage *message,
                                                  const gchar *expression_expected,
                                                  const gchar *expression_message)
{
    const gchar *content_type;

    content_type = soupcut_message_get_content_type(message);
    if (cut_utils_equal_string(expected, content_type)) {
        cut_test_pass();
    } else {
        const gchar *fail_message;
        const gchar *inspected_expected;
        const gchar *inspected_actual;

        inspected_expected = cut_utils_inspect_string(expected);
        inspected_actual = cut_utils_inspect_string(content_type);
        fail_message = cut_take_printf("<%s == %s[response][content-type]>\n"
                                       "  expected: <%s>\n"
                                       "    actual: <%s>",
                                       expression_expected,
                                       expression_message,
                                       inspected_expected,
                                       inspected_actual);
        cut_test_fail(fail_message);
    }
}


void
soupcut_client_assert_equal_content_type_helper (const gchar *expected,
                                                 SoupCutClient *client,
                                                 const gchar *expression_expected,
                                                 const gchar *expression_client)
{
    const gchar *content_type;
    SoupMessage *message;

    message = soupcut_client_get_latest_message(client);
    if (!message) {
        soupcut_test_fail_null_message(client, expression_client);
    }

    content_type = soupcut_message_get_content_type(message);
    if (cut_utils_equal_string(expected, content_type)) {
        cut_test_pass();
    } else {
        const gchar *fail_message;
        const gchar *inspected_expected;
        const gchar *inspected_actual;

        inspected_expected = cut_utils_inspect_string(expected);
        inspected_actual = cut_utils_inspect_string(content_type);
        fail_message =
            cut_take_printf(
                "<%s == latest_message(%s)[response][content-type]>\n"
                "  expected: <%s>\n"
                "    actual: <%s>",
                expression_expected,
                expression_client,
                inspected_expected,
                inspected_actual);
        cut_test_fail(fail_message);
    }
}

void soupcut_client_assert_response_helper (SoupCutClient *client,
                                            const gchar   *expression_client)
{
    SoupMessage *message;

    message = soupcut_client_get_latest_message(client);
    if (!message) {
        soupcut_test_fail_null_message(client, expression_client);
    }

    if (200 <= message->status_code && message->status_code < 300) {
        cut_test_pass();
    } else {
        const gchar *fail_message;

        fail_message =
            cut_take_printf("<latest_message(%s)[response][status] == 2XX>\n"
                            "  expected: <2XX>\n"
                            "    actual: <%u>(%s)",
                            expression_client,
                            message->status_code,
                            message->reason_phrase);
        cut_test_fail(fail_message);
    }
}

void
soupcut_client_assert_equal_body_helper (const gchar   *expected,
                                         SoupCutClient *client,
                                         const gchar   *expression_expected,
                                         const gchar   *expression_client)
{
    const gchar *body;
    SoupMessage *message;

    message = soupcut_client_get_latest_message(client);
    if (!message) {
        soupcut_test_fail_null_message(client, expression_client);
    }

    body = message->response_body->data;
    if (cut_utils_equal_string(expected, body)) {
        cut_test_pass();
    } else {
        const gchar *fail_message;
        const gchar *inspected_expected;
        const gchar *inspected_actual;

        inspected_expected = cut_utils_inspect_string(expected);
        inspected_actual = cut_utils_inspect_string(body);
        fail_message =
            cut_take_printf("<%s == latest_message(%s)[response][body]>\n"
                            "  expected: <%s>\n"
                            "    actual: <%s>",
                            expression_expected,
                            expression_client,
                            inspected_expected,
                            inspected_actual);
        cut_test_fail(fail_message);
    }
}

void
soupcut_client_assert_match_body_helper (const gchar   *pattern,
                                         SoupCutClient *client,
                                         const gchar   *expression_pattern,
                                         const gchar   *expression_client)
{
    const gchar *body;
    SoupMessage *message;

    message = soupcut_client_get_latest_message(client);
    if (!message) {
        soupcut_test_fail_null_message(client, expression_client);
    }

    body = message->response_body->data;
    if (cut_utils_regex_match(pattern, body)) {
        cut_test_pass();
    } else {
        const gchar *fail_message;
        const gchar *inspected_pattern;
        const gchar *inspected_actual;

        inspected_pattern = cut_utils_inspect_string(pattern);
        inspected_actual = cut_utils_inspect_string(body);
        fail_message =
            cut_take_printf("<%s> =~ <latest_message(%s)[response][body]>\n"
                            " pattern: <%s>\n"
                            "  actual: <%s>",
                            expression_pattern,
                            expression_client,
                            inspected_pattern,
                            inspected_actual);
        cut_test_fail(fail_message);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
