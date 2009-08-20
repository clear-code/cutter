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
#include "../lib/cuttest-soup.h"

void test_send_message(void);
void test_get(void);

void
cut_setup (void)
{
}

void
cut_teardown (void)
{
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
    server = cuttest_soup_server_take_new(context);
    uri = cuttest_soup_server_build_uri(server, "/");

    soup_server_add_handler(server, "/", server_callback,
                            NULL, NULL);
    soup_server_run_async(server);

    return uri;
}

void
test_send_message (void)
{
    SoupCutClient *client;
    SoupMessage *message;
    const gchar *uri;

    client = soupcut_client_new();
    uri = serve(client);

    message = soup_message_new("GET", uri);
    cut_assert_equal_uint(0, soupcut_client_get_n_messages(client));
    soupcut_client_send_message(client, message);
    cut_assert_equal_uint(1, soupcut_client_get_n_messages(client));
    gcut_assert_equal_object(message, soupcut_client_get_latest_message(client));

    soupcut_client_assert_response(client);
    soupcut_client_assert_equal_content_type("text/plain", client);
    soupcut_client_assert_equal_body("Hello", client);
}

void
test_get (void)
{
    SoupCutClient *client;
    const gchar *uri;

    client = soupcut_client_new();
    uri = serve(client);

    soupcut_client_get(client, uri, NULL);

    soupcut_client_assert_response(client);
    soupcut_client_assert_equal_content_type("text/plain", client);
    soupcut_client_assert_equal_body("Hello", client);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
