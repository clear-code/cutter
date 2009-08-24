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

void test_build_uri(void);

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

static SoupServer *
serve (SoupCutClient *client)
{
    SoupServer *server;
    GMainContext *context;

    context = soupcut_client_get_async_context(client);
    server = soupcut_server_take_new(context);

    soup_server_add_handler(server, "/", server_callback,
                            NULL, NULL);
    soup_server_run_async(server);

    return server;
}

void
test_build_uri (void)
{
    SoupServer *server;
    SoupAddress *address;
    const gchar *uri;

    server = serve(client);
    uri = soupcut_server_build_uri(server, "/");

    g_object_get(server,
                 SOUP_SERVER_INTERFACE, &address,
                 NULL);
    cut_assert_equal_string(cut_take_printf("http://localhost:%u/",
                                            soup_address_get_port(address)),
                            uri);

    soupcut_client_set_base(client, NULL);
    
    soupcut_client_get(client, uri, NULL);
    soupcut_client_assert_equal_body(cut_take_printf("Hello %s", uri), client);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
