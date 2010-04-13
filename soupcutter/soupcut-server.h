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

#ifndef __CUTTEST_SOUP_H__
#define __CUTTEST_SOUP_H__

#include <glib.h>
#include <libsoup/soup.h>

G_BEGIN_DECLS

/**
 * SECTION: soupcut-server
 * @title: Convenience HTTP server API
 * @short_description: API to work as HTTP server.
 * @see_also: <link
 *            linkend="cutter-Assertions-for-HTTP-client-and-server-based-on-libsoup">
 *            Assertions for HTTP client/server based on libsoup</link>
 *
 * Useful wrappers for #SoupServer are provided.
 *
 */

/**
 * soupcut_server_take:
 * @server: a #SoupServer.
 *
 * @server is shut down and unref when tear down.
 *
 * Returns: @server owned by Cutter. Don't g_object_unref() it.
 *
 * Since: 1.0.8
 */
SoupServer  *soupcut_server_take      (SoupServer *server);

/**
 * soupcut_server_take_new:
 * @context: a #GMainContext for the main loop of a new server.
 *
 * Creates and returns a new #SoupServer, whose context is @context.
 * The server is shut down and unref when tear down.
 *
 * Returns: a new #SoupServer owned by Cutter. Don't g_object_unref() it.
 *
 * Since: 1.0.8
 */
SoupServer  *soupcut_server_take_new  (GMainContext *context);

/**
 * soupcut_server_build_uri:
 * @server: a #SoupServer.
 * @path: a path.
 *
 * an URI is built and returned with the base uri of @server and @path.
 *
 * Returns: a new uri string owned by Cutter. Don't free it.
 *
 * Since: 1.0.8
 */
const gchar *soupcut_server_build_uri (SoupServer   *server,
                                       const gchar  *path);

G_END_DECLS

#endif


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
