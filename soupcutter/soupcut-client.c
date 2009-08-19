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

#include "soupcut-client.h"

#define SOUPCUT_CLIENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), SOUPCUT_TYPE_CLIENT, SoupCutClientPrivate))

typedef struct _SoupCutClientPrivate	SoupCutClientPrivate;
struct _SoupCutClientPrivate
{
    GList *messages;
};

G_DEFINE_TYPE (SoupCutClient, soupcut_client, G_TYPE_OBJECT)

static void dispose        (GObject         *object);

static void
soupcut_client_class_init (SoupCutClientClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    g_type_class_add_private(gobject_class, sizeof(SoupCutClientPrivate));
}

static void
soupcut_client_init (SoupCutClient *result)
{
    SoupCutClientPrivate *priv = SOUPCUT_CLIENT_GET_PRIVATE(result);

    priv->messages = NULL;
}

static void
dispose (GObject *object)
{
    SoupCutClientPrivate *priv = SOUPCUT_CLIENT_GET_PRIVATE(object);

    if (priv->messages) {
        g_list_foreach(priv->messages, (GFunc)g_object_unref, NULL);
        g_list_free(priv->messages);
        priv->messages = NULL;
    }

    G_OBJECT_CLASS(soupcut_client_parent_class)->dispose(object);
}

SoupCutClient *
soupcut_client_new (void)
{
    return g_object_new(SOUPCUT_TYPE_CLIENT,
                        NULL);
}

guint
soupcut_client_send_message (SoupCutClient *client, SoupMessage *message)
{
    SoupCutClientPrivate *priv = SOUPCUT_CLIENT_GET_PRIVATE(client);
    priv->messages = g_list_prepend(priv->messages, message);
    return SOUP_STATUS_OK;
}

guint
soupcut_client_get_n_messages (SoupCutClient *client)
{
    SoupCutClientPrivate *priv = SOUPCUT_CLIENT_GET_PRIVATE(client);
    return g_list_length(priv->messages);
}

SoupMessage *
soupcut_client_get_latest_message (SoupCutClient *client)
{
    SoupCutClientPrivate *priv = SOUPCUT_CLIENT_GET_PRIVATE(client);
    if (!priv->messages){
        return NULL;
    }
    
    return priv->messages->data;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

