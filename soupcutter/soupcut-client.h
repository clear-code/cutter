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

#ifndef __SOUPCUT_CLIENT_H__
#define __SOUPCUT_CLIENT_H__

#include <glib-object.h>
#include <libsoup/soup.h>

G_BEGIN_DECLS

/**
 * SECTION: soupcut-client
 * @title: Convenience HTTP client API
 * @short_description: API to work as HTTP client.
 * @see_also: <link
 *            linkend="cutter-soupcut-assertions">Assertions
 *            for HTTP client/server based on libsoup</link>
 *
 * #SoupCutClient works as a HTTP client. It accepts
 * multiple requests and responses are managed by
 * #SoupCutClient. #SoupCutClient related assertions are
 * also provided.
 */

#define SOUPCUT_TYPE_CLIENT            (soupcut_client_get_type ())
#define SOUPCUT_CLIENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SOUPCUT_TYPE_CLIENT, SoupCutClient))
#define SOUPCUT_CLIENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), SOUPCUT_TYPE_CLIENT, SoupCutClientClass))
#define SOUPCUT_IS_CLIENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SOUPCUT_TYPE_CLIENT))
#define SOUPCUT_IS_CLIENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SOUPCUT_TYPE_CLIENT))
#define SOUPCUT_CLIENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), SOUPCUT_TYPE_CLIENT, SoupCutClientClass))

typedef struct _SoupCutClient SoupCutClient;
typedef struct _SoupCutClientClass SoupCutClientClass;

struct _SoupCutClient
{
    GObject object;
};

struct _SoupCutClientClass
{
    GObjectClass parent_class;
};

GType          soupcut_client_get_type           (void) G_GNUC_CONST;

/**
 * soupcut_client_new:
 *
 * Creates and returns a new #SoupCutClient
 *
 * Returns: a new #SoupCutClient.
 *
 * Since: 1.0.8
 */
SoupCutClient *soupcut_client_new                (void);

/**
 * soupcut_client_set_base:
 * @client: a #SoupCutClient.
 * @uri: a base uri.
 *
 * Set @uri as a base uri of @client.
 *
 * Since: 1.0.8
 */
void           soupcut_client_set_base           (SoupCutClient *client,
                                                  const gchar   *uri);

/**
 * soupcut_client_send_message:
 * @client: a #SoupCutClient.
 * @message: a #SoupMessage.
 *
 * Send @message with @client. @client remebers all the messages it sent.
 *
 * Returns: a status code of the request.
 *
 * Since: 1.0.8
 */
guint          soupcut_client_send_message       (SoupCutClient *client,
                                                  SoupMessage   *message);

/**
 * soupcut_client_get:
 * @client: a #SoupCutClient.
 * @uri: an URI string.
 * @first_query_name: the name of the first query parameter.
 * @...: pairs of query parameter names and values, starting with @first_query_name, terminated by %NULL
 *
 * Send a GET request to @uri with queries. If the base URI of @client
 * is set by soupcut_client_set_base(), the destination URI is composed of the base URI and @uri. If not, @uri must be a fully qualified URI.
 *
 * Returns: a status code of the request.
 *
 * Since: 1.0.8
 */
guint          soupcut_client_get                (SoupCutClient *client,
                                                  const gchar   *uri,
                                                  const gchar   *first_query_name,
                                                  ...);

/**
 * soupcut_client_get_n_messages:
 * @client: a #SoupCutClient.
 *
 * Returns the number of messages @client sent.
 *
 * Returns: the number of messages.
 *
 * Since: 1.0.8
 */
guint          soupcut_client_get_n_messages     (SoupCutClient *client);

/**
 * soupcut_client_get_latest_message:
 * @client: a #SoupCutClient.
 *
 * Returns the latest #SoupMessage @client sent.
 *
 * Returns: the latest #SoupMessage.
 *
 * Since: 1.0.8
 */
SoupMessage   *soupcut_client_get_latest_message (SoupCutClient *client);

/**
 * soupcut_client_get_async_context:
 * @client: a #SoupCutClient.
 *
 * Returns the asynch_context of @client.
 *
 * Returns: a #GMainContext of @client, which may be %NULL.
 *
 * Since: 1.0.8
 */
GMainContext  *soupcut_client_get_async_context  (SoupCutClient *client);


G_END_DECLS

#endif /* __SOUPCUT_CLIENT_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
