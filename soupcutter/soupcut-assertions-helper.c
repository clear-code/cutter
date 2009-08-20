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


void
soupcut_message_assert_equal_content_type_helper (const gchar *expected,
                                                  SoupMessage *actual,
                                                  const gchar     *expression_expected,
                                                  const gchar     *expression_actual)
{
    const gchar *content_type;
    content_type = soup_message_headers_get_content_type(actual->response_headers,
                                                         NULL);
    if (cut_utils_equal_string(expected, content_type)) {
        cut_test_pass();
    } else {
        GString *message;
        const gchar *fail_message;
        const gchar *inspected_expected;
        const gchar *inspected_actual;

        message = g_string_new(NULL);
        g_string_append_printf(message,
                               "<%s == %s[response][content-type]>\n",
                               expression_expected,
                               expression_actual);
        inspected_expected = cut_utils_inspect_string(expected);
        inspected_actual = cut_utils_inspect_string(content_type);
        g_string_append_printf(message,
                               "  expected: <%s>\n"
                               "    actual: <%s>",
                               inspected_expected,
                               inspected_actual);
        fail_message = cut_take_string(g_string_free(message, FALSE));
        cut_test_fail(fail_message);
    }
}


void
soupcut_client_assert_equal_content_type_helper (const gchar *expected,
                                                 SoupCutClient *client,
                                                 const gchar     *expression_expected,
                                                 const gchar     *expression_actual)
{
    const gchar *content_type;
    SoupMessage *message;

    message = soupcut_client_get_latest_message(client);
    if (!message) {
        GString *message;
        const gchar *fail_message;
        const gchar *inspected_actual;

        message = g_string_new(NULL);
        g_string_append_printf(message,
                               "<latest_message(%s) != NULL>\n",
                               expression_actual);
        inspected_actual = gcut_object_inspect(G_OBJECT(client));
        g_string_append_printf(message,
                               "    client: <%s>",
                               inspected_actual);
        fail_message = cut_take_string(g_string_free(message, FALSE));
        cut_test_fail(fail_message);
    }
    
    content_type = soup_message_headers_get_content_type(message->response_headers,
                                                         NULL);
    
    if (cut_utils_equal_string(expected, content_type)) {
        cut_test_pass();
    } else {
        GString *message;
        const gchar *fail_message;
        const gchar *inspected_expected;
        const gchar *inspected_actual;

        message = g_string_new(NULL);
        g_string_append_printf(message,
                               "<%s == latest_message(%s)[response][content-type]>\n",
                               expression_expected,
                               expression_actual);
        inspected_expected = cut_utils_inspect_string(expected);
        inspected_actual = cut_utils_inspect_string(content_type);
        g_string_append_printf(message,
                               "  expected: <%s>\n"
                               "    actual: <%s>",
                               inspected_expected,
                               inspected_actual);
        fail_message = cut_take_string(g_string_free(message, FALSE));
        cut_test_fail(fail_message);
    }
}

void soupcut_client_assert_response_helper (SoupCutClient *client,
                                            const gchar   *expression_client)
{
    SoupMessage *soup_message;

    soup_message = soupcut_client_get_latest_message(client);
    if (!soup_message) {
        GString *message;
        const gchar *fail_message;
        const gchar *inspected_actual;

        message = g_string_new(NULL);
        g_string_append_printf(message,
                               "<latest_message(%s) != NULL>\n",
                               expression_client);
        inspected_actual = gcut_object_inspect(G_OBJECT(client));
        g_string_append_printf(message,
                               "    client: <%s>",
                               inspected_actual);
        fail_message = cut_take_string(g_string_free(message, FALSE));
        cut_test_fail(fail_message);
    }
    
    if (soup_message->status_code >= 200 && soup_message->status_code < 300) {
        cut_test_pass();
    } else {
        GString *message;
        const gchar *fail_message;

        message = g_string_new(NULL);
        g_string_append_printf(message,
                               "<latest_message(%s)[response][status] == 2XX>\n",
                               expression_client);
        g_string_append_printf(message,
                               "    actual: <%u>(%s)",
                               soup_message->status_code,
                               soup_message->reason_phrase);
        fail_message = cut_take_string(g_string_free(message, FALSE));
        cut_test_fail(fail_message);
    }
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
