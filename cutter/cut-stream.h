/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>
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

#ifndef __CUT_STREAMER_H__
#define __CUT_STREAMER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CUT_TYPE_STREAM            (cut_stream_get_type ())
#define CUT_STREAM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_STREAM, CutStream))
#define CUT_STREAM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_STREAM, CutStreamClass))
#define CUT_IS_STREAM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_STREAM))
#define CUT_IS_STREAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_STREAM))
#define CUT_STREAM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_STREAM, CutStreamClass))

typedef gboolean (*CutStreamFunction) (const gchar *message,
                                       GError **error,
                                       gpointer user_data);

typedef struct _CutStream         CutStream;
typedef struct _CutStreamClass    CutStreamClass;

struct _CutStream
{
    GObject object;
};

struct _CutStreamClass
{
    GObjectClass parent_class;
};

GType        cut_stream_get_type  (void) G_GNUC_CONST;

CutStream   *cut_stream_new       (const gchar *name,
                                   const gchar *first_property,
                                   ...);
G_END_DECLS

#endif /* __CUT_STREAM_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
