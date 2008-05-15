/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_LISTENER_H__
#define __CUT_LISTENER_H__

#include <glib-object.h>
#include <cutter/cut-private.h>

G_BEGIN_DECLS

#define CUT_TYPE_LISTENER            (cut_listener_get_type ())
#define CUT_LISTENER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_LISTENER, CutListener))
#define CUT_LISTENER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_LISTENER, CutListenerClass))
#define CUT_IS_LISTENER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_LISTENER))
#define CUT_IS_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_LISTENER))
#define CUT_LISTENER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_INTERFACE((obj), CUT_TYPE_LISTENER, CutListenerClass))

typedef struct _CutListenerClass CutListenerClass;

struct _CutListenerClass
{
    GTypeInterface base_iface;

    void (*attach_to_run_context)    (CutListener   *listener,
                                      CutRunContext *run_context);
    void (*detach_from_run_context)  (CutListener   *listener,
                                      CutRunContext *run_context);
};

GType        cut_listener_get_type  (void) G_GNUC_CONST;

void cut_listener_attach_to_run_context  (CutListener   *listener,
                                          CutRunContext *run_context);
void cut_listener_detach_from_run_context(CutListener   *listener,
                                          CutRunContext *run_context);

G_END_DECLS

#endif /* __CUT_LISTENER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
