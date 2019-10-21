/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2011-2019  Kouhei Sutou <kou@clear-code.com>
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

#include "gcut-event-loop.h"

G_DEFINE_ABSTRACT_TYPE(GCutEventLoop, gcut_event_loop, G_TYPE_OBJECT)

static void     dispose        (GObject         *object);

static void
gcut_event_loop_class_init (GCutEventLoopClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    klass->run = NULL;
    klass->quit = NULL;
    klass->watch_io = NULL;
    klass->watch_child_full = NULL;
    klass->add_timeout_full = NULL;
    klass->add_idle_full = NULL;
    klass->remove = NULL;
}

static void
gcut_event_loop_init (GCutEventLoop *loop)
{
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(gcut_event_loop_parent_class)->dispose(object);
}

GQuark
gcut_event_loop_error_quark (void)
{
    return g_quark_from_static_string("gcut-event-loop-error-quark");
}

void
gcut_event_loop_run (GCutEventLoop *loop)
{
    GCutEventLoopClass *loop_class;

    g_return_if_fail(loop != NULL);
    g_return_if_fail(GCUT_IS_EVENT_LOOP(loop));

    loop_class = GCUT_EVENT_LOOP_GET_CLASS(loop);
    loop_class->run(loop);
}

gboolean
gcut_event_loop_iterate (GCutEventLoop *loop, gboolean may_block)
{
    GCutEventLoopClass *loop_class;

    g_return_val_if_fail(loop != NULL, FALSE);
    g_return_val_if_fail(GCUT_IS_EVENT_LOOP(loop), FALSE);

    loop_class = GCUT_EVENT_LOOP_GET_CLASS(loop);
    return loop_class->iterate(loop, may_block);
}

void
gcut_event_loop_quit (GCutEventLoop *loop)
{
    GCutEventLoopClass *loop_class;

    g_return_if_fail(loop != NULL);
    g_return_if_fail(GCUT_IS_EVENT_LOOP(loop));

    loop_class = GCUT_EVENT_LOOP_GET_CLASS(loop);
    loop_class->quit(loop);
}

guint
gcut_event_loop_watch_io (GCutEventLoop *loop,
                          GIOChannel      *channel,
                          GIOCondition     condition,
                          GIOFunc          function,
                          gpointer         data)
{
    GCutEventLoopClass *loop_class;

    g_return_val_if_fail(loop != NULL, 0);
    g_return_val_if_fail(GCUT_IS_EVENT_LOOP(loop), 0);

    loop_class = GCUT_EVENT_LOOP_GET_CLASS(loop);
    return loop_class->watch_io(loop, channel, condition, function, data);
}

guint
gcut_event_loop_watch_child (GCutEventLoop *loop,
                             GPid             pid,
                             GChildWatchFunc  function,
                             gpointer         data)
{
    return gcut_event_loop_watch_child_full(loop, G_PRIORITY_DEFAULT,
                                            pid,
                                            function, data,
                                            NULL);
}

guint
gcut_event_loop_watch_child_full (GCutEventLoop *loop,
                                  gint             priority,
                                  GPid             pid,
                                  GChildWatchFunc  function,
                                  gpointer         data,
                                  GDestroyNotify   notify)
{
    GCutEventLoopClass *loop_class;

    g_return_val_if_fail(loop != NULL, 0);
    g_return_val_if_fail(GCUT_IS_EVENT_LOOP(loop), 0);

    loop_class = GCUT_EVENT_LOOP_GET_CLASS(loop);
    return loop_class->watch_child_full(loop, priority, pid,
                                        function, data, notify);
}

guint
gcut_event_loop_add_timeout (GCutEventLoop *loop,
                             gdouble          interval_in_seconds,
                             GSourceFunc      function,
                             gpointer         data)
{
    return gcut_event_loop_add_timeout_full(loop, G_PRIORITY_DEFAULT,
                                            interval_in_seconds,
                                            function, data,
                                            NULL);
}

guint
gcut_event_loop_add_timeout_full (GCutEventLoop *loop,
                                  gint             priority,
                                  gdouble          interval_in_seconds,
                                  GSourceFunc      function,
                                  gpointer         data,
                                  GDestroyNotify   notify)
{
    GCutEventLoopClass *loop_class;

    g_return_val_if_fail(loop != NULL, 0);
    g_return_val_if_fail(GCUT_IS_EVENT_LOOP(loop), 0);

    loop_class = GCUT_EVENT_LOOP_GET_CLASS(loop);
    return loop_class->add_timeout_full(loop, priority, interval_in_seconds,
                                        function, data, notify);
}

guint
gcut_event_loop_add_idle (GCutEventLoop *loop,
                          GSourceFunc      function,
                          gpointer         data)
{
    return gcut_event_loop_add_idle_full(loop, G_PRIORITY_DEFAULT_IDLE,
                                         function, data, NULL);
}

guint
gcut_event_loop_add_idle_full (GCutEventLoop *loop,
                               gint             priority,
                               GSourceFunc      function,
                               gpointer         data,
                               GDestroyNotify   notify)
{
    GCutEventLoopClass *loop_class;

    g_return_val_if_fail(loop != NULL, 0);
    g_return_val_if_fail(GCUT_IS_EVENT_LOOP(loop), 0);

    loop_class = GCUT_EVENT_LOOP_GET_CLASS(loop);
    return loop_class->add_idle_full(loop, priority, function, data, notify);
}

gboolean
gcut_event_loop_remove (GCutEventLoop *loop, guint tag)
{
    GCutEventLoopClass *loop_class;

    g_return_val_if_fail(loop != NULL, FALSE);
    g_return_val_if_fail(GCUT_IS_EVENT_LOOP(loop), FALSE);

    loop_class = GCUT_EVENT_LOOP_GET_CLASS(loop);
    return loop_class->remove(loop, tag);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
