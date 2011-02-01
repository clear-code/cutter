/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2011  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __GCUT_GLIB_EVENT_LOOP_H__
#define __GCUT_GLIB_EVENT_LOOP_H__

#include <gcutter/gcut-event-loop.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-glib-event-loop
 * @title: GLib event loop
 * @short_description: Event loop API for GLib main context
 * and loop.
 *
 * #GCutGLibEventLoop is an #GCutEventLoop implementation
 * for GLib's main context and loop.
 *
 * Since: 1.1.6
 */

#define GCUT_TYPE_GLIB_EVENT_LOOP            (gcut_glib_event_loop_get_type())
#define GCUT_GLIB_EVENT_LOOP(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GCUT_TYPE_GLIB_EVENT_LOOP, GCutGLibEventLoop))
#define GCUT_GLIB_EVENT_LOOP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GCUT_TYPE_GLIB_EVENT_LOOP, GCutGLibEventLoopClass))
#define GCUT_IS_GLIB_EVENT_LOOP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GCUT_TYPE_GLIB_EVENT_LOOP))
#define GCUT_IS_GLIB_EVENT_LOOP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GCUT_TYPE_GLIB_EVENT_LOOP))
#define GCUT_GLIB_EVENT_LOOP_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GCUT_TYPE_GLIB_EVENT_LOOP, GCutGLibEventLoopClass))

typedef struct _GCutGLibEventLoop         GCutGLibEventLoop;
typedef struct _GCutGLibEventLoopClass    GCutGLibEventLoopClass;

struct _GCutGLibEventLoop
{
    GCutEventLoop object;
};

struct _GCutGLibEventLoopClass
{
    GCutEventLoopClass parent_class;
};

GType                gcut_glib_event_loop_get_type     (void) G_GNUC_CONST;

/**
 * gcut_glib_event_loop_run:
 * @context: a #GMainContext or %NULL
 *
 * Creates a new #GCutEeventLoop for @context. If @context
 * is %NULL, the default GLib's main context is used.
 *
 * Returns: a new #GCutEventLoop.
 *
 * Since: 1.1.6
 */
GCutEventLoop     *gcut_glib_event_loop_new            (GMainContext *context);


G_END_DECLS

#endif /* __GCUT_GLIB_EVENT_LOOP_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
