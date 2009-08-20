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

GType          soupcut_client_get_type   (void) G_GNUC_CONST;

SoupCutClient *soupcut_client_new        (void);

guint          soupcut_client_send_message(SoupCutClient *client, SoupMessage *message);
guint          soupcut_client_get(SoupCutClient *client, const gchar *uri,
                                  const gchar *first_name, ...);
guint          soupcut_client_get_n_messages(SoupCutClient *client);
SoupMessage    *soupcut_client_get_latest_message(SoupCutClient *client);
GMainContext   *soupcut_client_get_async_context(SoupCutClient *client);


G_END_DECLS

#endif /* __SOUPCUT_CLIENT_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
