/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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

#include "cuttest-soup.h"
#include <cutter.h>

static void
cb_destroy (gpointer data)
{
    SoupServer *server = data;

    soup_server_quit(server);
    g_object_unref(server);
}

SoupServer *
cuttest_soup_server_take_new (GMainContext *context)
{
    SoupServer *server;
    SoupAddress *address;

    address = soup_address_new("localhost", SOUP_ADDRESS_ANY_PORT);
    soup_address_resolve_sync(address, NULL);
    server = soup_server_new(SOUP_SERVER_INTERFACE, address,
                             SOUP_SERVER_ASYNC_CONTEXT, context,
                             NULL);
    g_object_unref(address);
    cut_take(server, cb_destroy);

    return server;
}

const gchar *
cuttest_soup_server_build_uri(SoupServer   *server,
                              const gchar  *path)
{
    SoupAddress *address;
    SoupURI *uri;
    const gchar *uri_string;

    g_object_get(server,
                 SOUP_SERVER_INTERFACE, &address,
                 NULL);

    uri = soup_uri_new(NULL);
    soup_uri_set_scheme(uri, SOUP_URI_SCHEME_HTTP);
    soup_uri_set_host(uri, "localhost");
    soup_uri_set_port(uri, soup_address_get_port(address));
    soup_uri_set_path(uri, path);

    uri_string = cut_take_string(soup_uri_to_string(uri, FALSE));
    soup_uri_free(uri);

    return uri_string;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
