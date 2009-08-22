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

#include <glib.h>
#include <gcutter.h>

#include "soupcut-client.h"

#define SOUPCUT_CLIENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), SOUPCUT_TYPE_CLIENT, SoupCutClientPrivate))

typedef struct _SoupCutClientPrivate	SoupCutClientPrivate;
struct _SoupCutClientPrivate
{
    GList *messages;
    SoupSession *session;
    SoupURI *base;
    GMainContext *main_context;

    gboolean async;
};


enum
{
    PROP_0,
    PROP_ASYNC,
};

G_DEFINE_TYPE (SoupCutClient, soupcut_client, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void
soupcut_client_class_init (SoupCutClientClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_boolean("async",
                                "Async",
                                "True if the session is asynchronous",
                                FALSE,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ASYNC, spec);

    g_type_class_add_private(gobject_class, sizeof(SoupCutClientPrivate));
}

static void
soupcut_client_init (SoupCutClient *result)
{
    SoupCutClientPrivate *priv = SOUPCUT_CLIENT_GET_PRIVATE(result);

    priv->base = NULL;
    priv->messages = NULL;
    priv->main_context = g_main_context_new();
    priv->session =
        soup_session_async_new_with_options(
            SOUP_SESSION_ASYNC_CONTEXT, priv->main_context,
            NULL);
}

static void
dispose (GObject *object)
{
    SoupCutClientPrivate *priv = SOUPCUT_CLIENT_GET_PRIVATE(object);

    if (priv->base) {
        soup_uri_free(priv->base);
        priv->base = NULL;
    }

    if (priv->messages) {
        g_list_foreach(priv->messages, (GFunc)g_object_unref, NULL);
        g_list_free(priv->messages);
        priv->messages = NULL;
    }

    if (priv->main_context) {
        g_main_context_unref(priv->main_context);
        priv->main_context = NULL;
    }

    if (priv->session) {
        g_object_unref(priv->session);
        priv->session = NULL;
    }

    G_OBJECT_CLASS(soupcut_client_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    SoupCutClientPrivate *priv;

    priv = SOUPCUT_CLIENT_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_ASYNC:
        priv->async = g_value_get_boolean(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    SoupCutClientPrivate *priv;

    priv = SOUPCUT_CLIENT_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_ASYNC:
        g_value_set_boolean(value, priv->async);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

SoupCutClient *
soupcut_client_new (void)
{
    return g_object_new(SOUPCUT_TYPE_CLIENT,
                        NULL);
}

void
soupcut_client_set_base(SoupCutClient *client, const gchar *uri)
{
    SoupCutClientPrivate *priv;

    priv = SOUPCUT_CLIENT_GET_PRIVATE(client);
    if (priv->base) {
        soup_uri_free(priv->base);
        priv->base = NULL;
    }

    if (uri)
        priv->base = soup_uri_new(uri);
}

guint
soupcut_client_send_message (SoupCutClient *client, SoupMessage *message)
{
    SoupCutClientPrivate *priv;

    priv = SOUPCUT_CLIENT_GET_PRIVATE(client);
    priv->messages = g_list_prepend(priv->messages, message);
    return soup_session_send_message(priv->session, message);
}

guint
soupcut_client_get (SoupCutClient *client, const gchar *uri_string,
                    const gchar *first_query_name, ...)
{
    SoupMessage *message;
    GHashTable *params;
    va_list args;
    SoupURI *uri;
    gchar *built_uri;
    SoupCutClientPrivate *priv;

    priv = SOUPCUT_CLIENT_GET_PRIVATE(client);

    if (priv->base) {
        if (uri_string) {
            uri = soup_uri_new_with_base(priv->base, uri_string);
        } else {
            uri = soup_uri_copy(priv->base);
        }
    } else {
        if (!uri_string)
            return SOUP_STATUS_MALFORMED;
        uri = soup_uri_new(uri_string);
    }

    if (first_query_name) {
        va_start(args, first_query_name);
        params = gcut_hash_table_string_string_new_va_list(first_query_name,
                                                           args);
        va_end(args);

        soup_uri_set_query_from_form(uri, params);
        g_hash_table_unref(params);
    }

    built_uri = soup_uri_to_string(uri, FALSE);
    message = soup_message_new("GET", built_uri);
    g_free(built_uri);
    soup_uri_free(uri);

    return soupcut_client_send_message(client, message);
}

guint
soupcut_client_get_n_messages (SoupCutClient *client)
{
    SoupCutClientPrivate *priv;

    priv = SOUPCUT_CLIENT_GET_PRIVATE(client);
    return g_list_length(priv->messages);
}

SoupMessage *
soupcut_client_get_latest_message (SoupCutClient *client)
{
    SoupCutClientPrivate *priv;

    priv = SOUPCUT_CLIENT_GET_PRIVATE(client);
    if (!priv->messages)
        return NULL;

    return priv->messages->data;
}

GMainContext *
soupcut_client_get_async_context (SoupCutClient *client)
{
    SoupCutClientPrivate *priv;

    priv = SOUPCUT_CLIENT_GET_PRIVATE(client);
    if (!priv->session)
        return NULL;

    return soup_session_get_async_context(priv->session);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

