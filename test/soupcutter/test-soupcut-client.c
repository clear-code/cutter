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

#include <string.h>
#include <soupcutter/soupcutter.h>

void test_send_message(void);
void test_get(void);
void test_get_query(void);
void test_set_base(void);
void test_set_base_null(void);

static SoupCutClient *client;
static GHashTable *received_query;

void
cut_setup (void)
{
    client = soupcut_client_new();
    received_query = NULL;
}

void
cut_teardown (void)
{
    if (client){
        g_object_unref(client);
    }

    if (received_query)
        g_hash_table_unref(received_query);
}

static void
assert_response_equal_body (const gchar *expected, SoupCutClient *client)
{
    soupcut_client_assert_response(client);
    soupcut_client_assert_equal_content_type("text/plain", client);
    soupcut_client_assert_equal_body(expected, client);
}

static void
server_callback (SoupServer *server,
                 SoupMessage *msg,
                 const gchar *path,
                 GHashTable *query,
                 SoupClientContext *client,
                 gpointer user_data)
{
    const gchar *body;
    body = cut_take_printf("Hello %s",
                           soupcut_server_build_uri(server, path));
    if (received_query){
        g_hash_table_unref(received_query);
        received_query = NULL;
    }
    if (query)
        received_query = gcut_hash_table_string_string_copy(query);

    soup_message_set_status(msg, SOUP_STATUS_OK);
    soup_message_set_response(msg, "text/plain", SOUP_MEMORY_COPY,
                              body, strlen(body));
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

void
test_send_message (void)
{
    SoupMessage *message;
    const gchar *uri;

    uri = serve(client);

    message = soup_message_new("GET", uri);
    cut_assert_equal_uint(0, soupcut_client_get_n_messages(client));
    soupcut_client_send_message(client, message);
    cut_assert_equal_uint(1, soupcut_client_get_n_messages(client));
    gcut_assert_equal_object(message, soupcut_client_get_latest_message(client));

    assert_response_equal_body(cut_take_printf("Hello %s", uri), client);
}

void
test_get (void)
{
    const gchar *uri;

    uri = serve(client);

    soupcut_client_get(client, uri, NULL);

    assert_response_equal_body(cut_take_printf("Hello %s", uri), client);
}

void
test_get_query (void)
{
    const gchar *uri;
    GHashTable *query;
    
    uri = serve(client);

    query = gcut_take_new_hash_table_string_string("name", "value", NULL);
    soupcut_client_get(client, uri, "name", "value", NULL);

    gcut_assert_equal_hash_table_string_string(query, received_query);
}

void
test_set_base (void)
{
    const gchar *uri;
    const gchar *uri_another;

    uri = serve(client);

    soupcut_client_set_base(client, uri);
    
    soupcut_client_get(client, NULL, NULL);
    assert_response_equal_body(cut_take_printf("Hello %s", uri), client);
    
    soupcut_client_get(client, "/", NULL);
    assert_response_equal_body(cut_take_printf("Hello %s", uri), client);

    soupcut_client_get(client, "/hello", NULL);
    assert_response_equal_body(cut_take_printf("Hello %shello", uri), client);

    soupcut_client_get(client, "goodbye", NULL);
    assert_response_equal_body(cut_take_printf("Hello %sgoodbye", uri), client);

    uri_another = serve(client);

    soupcut_client_get(client, uri_another, NULL);
    assert_response_equal_body(cut_take_printf("Hello %s", uri_another), client);

    soupcut_client_get(client, cut_take_printf("%s%s", uri_another, "another"),
                       NULL);
    assert_response_equal_body(cut_take_printf("Hello %sanother", uri_another), client);
}

void
test_set_base_null (void)
{
    const gchar *uri;

    uri = serve(client);

    soupcut_client_set_base(client, NULL);
    
    cut_assert_equal_uint(SOUP_STATUS_MALFORMED,
                          soupcut_client_get(client, NULL, NULL));
    
    soupcut_client_get(client, uri, NULL);
    assert_response_equal_body(cut_take_printf("Hello %s", uri), client);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
