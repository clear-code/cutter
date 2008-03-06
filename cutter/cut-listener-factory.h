/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifndef __CUT_LISTENER_FACTORY_H__
#define __CUT_LISTENER_FACTORY_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CUT_TYPE_LISTENER_FACTORY            (cut_listener_factory_get_type ())
#define CUT_LISTENER_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_LISTENER_FACTORY, CutListenerFactory))
#define CUT_LISTENER_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_LISTENER_FACTORY, CutListenerFactoryClass))
#define CUT_IS_LISTENER_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_LISTENER_FACTORY))
#define CUT_IS_LISTENER_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_LISTENER_FACTORY))
#define CUT_LISTENER_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_LISTENER_FACTORY, CutListenerFactoryClass))

typedef struct _CutListenerFactory         CutListenerFactory;
typedef struct _CutListenerFactoryClass    CutListenerFactoryClass;

struct _CutListenerFactory
{
    GObject object;
};

struct _CutListenerFactoryClass
{
    GObjectClass parent_class;
    void (*set_option_context) (CutListenerFactory *factory,
                                GOptionContext *context);
    void (*activate)           (CutListenerFactory *factory);
};

GType cut_listener_factory_get_type (void) G_GNUC_CONST;

void  cut_listener_factory_set_option_context
                                    (CutListenerFactory *factory,
                                     GOptionContext *context);
void  cut_listener_factory_activate (CutListenerFactory *factory);


G_END_DECLS

#endif /* __CUT_LISTENER_FACTORY_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
