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

#ifndef __GCUT_EVENT_LOOP_H__
#define __GCUT_EVENT_LOOP_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-event-loop
 * @title: Abstracted event loop
 * @short_description: Abstracted event loop API for
 * customizing event loop in GCutter.
 *
 * #GCutEventLoop encapsulates event loop. For example,
 * event loop is used in #GCutProcess. It uses the GLib's
 * default main context for it.
 *
 * Normally, a custom #GCutEventLoop isn't required. It is
 * needed some special case. For example, using libev as
 * event loop backend instead of GLib's main loop.
 *
 * GCutter provides #GCutEventLoop for GLib's main context
 * and main loop, #GCutGLibEventLoop.
 *
 * Since: 1.1.6
 */

#define GCUT_TYPE_EVENT_LOOP            (gcut_event_loop_get_type ())
#define GCUT_EVENT_LOOP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GCUT_TYPE_EVENT_LOOP, GCutEventLoop))
#define GCUT_EVENT_LOOP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GCUT_TYPE_EVENT_LOOP, GCutEventLoopClass))
#define GCUT_IS_EVENT_LOOP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GCUT_TYPE_EVENT_LOOP))
#define GCUT_IS_EVENT_LOOP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GCUT_TYPE_EVENT_LOOP))
#define GCUT_EVENT_LOOP_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GCUT_TYPE_EVENT_LOOP, GCutEventLoopClass))

#define GCUT_EVENT_LOOP_ERROR           (gcut_event_loop_error_quark())

typedef struct _GCutEventLoop      GCutEventLoop;
typedef struct _GCutEventLoopClass GCutEventLoopClass;

struct _GCutEventLoop
{
    GObject object;
};

struct _GCutEventLoopClass
{
    GObjectClass parent_class;

    void     (*run)              (GCutEventLoop   *loop);
    gboolean (*iterate)          (GCutEventLoop   *loop,
                                  gboolean         may_block);
    void     (*quit)             (GCutEventLoop   *loop);

    guint    (*watch_io)         (GCutEventLoop   *loop,
                                  GIOChannel      *channel,
                                  GIOCondition     condition,
                                  GIOFunc          function,
                                  gpointer         data);
    guint    (*watch_child_full) (GCutEventLoop   *loop,
                                  gint             priority,
                                  GPid             pid,
                                  GChildWatchFunc  function,
                                  gpointer         data,
                                  GDestroyNotify   notify);
    guint    (*add_timeout_full) (GCutEventLoop   *loop,
                                  gint             priority,
                                  gdouble          interval_in_seconds,
                                  GSourceFunc      function,
                                  gpointer         data,
                                  GDestroyNotify   notify);
    guint    (*add_idle_full)    (GCutEventLoop   *loop,
                                  gint             priority,
                                  GSourceFunc      function,
                                  gpointer         data,
                                  GDestroyNotify   notify);
    gboolean (*remove)           (GCutEventLoop   *loop,
                                  guint            tag);
};

GQuark               gcut_event_loop_error_quark       (void);
GType                gcut_event_loop_get_type          (void) G_GNUC_CONST;

/**
 * gcut_event_loop_run:
 * @loop: a #GCutEventLoop.
 *
 * Runs the given event loop until gcut_event_loop_quit() is
 * called on the loop.
 *
 * Since: 1.1.6
 */
void                 gcut_event_loop_run               (GCutEventLoop   *loop);

/**
 * gcut_event_loop_iterate:
 * @loop: a #GCutEventLoop.
 * @may_block: whether the call may block.
 *
 * Runs a single iteration for the given event loop. If no
 * events are ready and @may_block is %TRUE, waiting
 * for a event become ready. Otherwise, if @may_block is
 * %FALSE, events are not waited to become ready.
 *
 * Returns: %TRUE if a event was dispatched.
 *
 * Since: 1.1.6
 */
gboolean             gcut_event_loop_iterate           (GCutEventLoop   *loop,
                                                        gboolean         may_block);

/**
 * gcut_event_loop_stop:
 * @loop: a #GCutEventLoop.
 *
 * Stops the @loop from running.
 *
 * Since: 1.1.6
 */
void                 gcut_event_loop_quit              (GCutEventLoop   *loop);

/**
 * gcut_event_loop_watch_io:
 * @loop: a #GCutEventLoop
 * @channel: a #GIOChannel
 * @condition: conditions to watch for
 * @function: function to call
 * @data: data to pass to @function
 *
 * Adds the @channel into @loop with the default
 * priority. @function is called when @condition is met for
 * the given @channel.
 *
 * Returns: the event ID.
 *
 * Since: 1.1.6
 */
guint                gcut_event_loop_watch_io          (GCutEventLoop   *loop,
                                                        GIOChannel      *channel,
                                                        GIOCondition     condition,
                                                        GIOFunc          function,
                                                        gpointer         data);

/**
 * gcut_event_loop_watch_child:
 * @loop: a #GCutEventLoop
 * @pid: process ID to watch
 * @function: function to call
 * @data: data to pass to @function
 *
 * Adds the @function to be called when the child indicated
 * by @pid exists into @loop with the default priority.
 *
 * Returns: the event ID.
 *
 * Since: 1.1.6
 */
guint                gcut_event_loop_watch_child       (GCutEventLoop   *loop,
                                                        GPid             pid,
                                                        GChildWatchFunc  function,
                                                        gpointer         data);

/**
 * gcut_event_loop_watch_child_full:
 * @loop: a #GCutEventLoop
 * @priority: the priority of the event.
 * @pid: process ID to watch
 * @function: function to call
 * @data: data to pass to @function
 * @notify: function to call when the event is removed, or %NULL
 *
 * Adds the @function to be called when the child indicated
 * by @pid exists into @loop with the @priority.
 *
 * Returns: the event ID.
 *
 * Since: 1.1.6
 */
guint                gcut_event_loop_watch_child_full  (GCutEventLoop   *loop,
                                                        gint             priority,
                                                        GPid             pid,
                                                        GChildWatchFunc  function,
                                                        gpointer         data,
                                                        GDestroyNotify   notify);

/**
 * gcut_event_loop_add_timeout:
 * @loop: a #GCutEventLoop
 * @interval: the time between calls to the @function, in seconds.
 * @function: function to call
 * @data: data to pass to @function
 *
 * Adds the @function to be called at regular intervals,
 * with the default priority.
 *
 * Returns: the event ID.
 *
 * Since: 1.1.6
 */
guint                gcut_event_loop_add_timeout       (GCutEventLoop   *loop,
                                                        gdouble          interval_in_seconds,
                                                        GSourceFunc      function,
                                                        gpointer         data);

/**
 * gcut_event_loop_add_timeout_full:
 * @loop: a #GCutEventLoop
 * @priority: the priority of the event.
 * @interval: the time between calls to the @function, in seconds.
 * @function: function to call
 * @data: data to pass to @function
 * @notify: function to call when the event is removed, or %NULL
 *
 * Adds the @function to be called at regular intervals,
 * with the @priority.
 *
 * Returns: the event ID.
 *
 * Since: 1.1.6
 */
guint                gcut_event_loop_add_timeout_full  (GCutEventLoop   *loop,
                                                        gint             priority,
                                                        gdouble          interval_in_seconds,
                                                        GSourceFunc      function,
                                                        gpointer         data,
                                                        GDestroyNotify   notify);

/**
 * gcut_event_loop_add_idle:
 * @loop: a #GCutEventLoop
 * @function: function to call
 * @data: data to pass to @function
 *
 * Adds the @function to be called whenever there are no
 * higher priority events pending with the default priority.
 *
 * Returns: the event ID.
 *
 * Since: 1.1.6
 */
guint                gcut_event_loop_add_idle          (GCutEventLoop   *loop,
                                                        GSourceFunc      function,
                                                        gpointer         data);

/**
 * gcut_event_loop_add_idle_full:
 * @loop: a #GCutEventLoop
 * @priority: the priority of the event.
 * @function: function to call
 * @data: data to pass to @function
 * @notify: function to call when the event is removed, or %NULL
 *
 * Adds the @function to be called whenever there are no
 * higher priority events pending with the @priority.
 *
 * Returns: the event ID.
 *
 * Since: 1.1.6
 */
guint                gcut_event_loop_add_idle_full     (GCutEventLoop   *loop,
                                                        gint             priority,
                                                        GSourceFunc      function,
                                                        gpointer         data,
                                                        GDestroyNotify   notify);

/**
 * gcut_event_loop_remove:
 * @loop: a #GCutEventLoop
 * @tag: the ID of the source to remove
 *
 * Removes the event with the given ID, @tag.
 *
 * Returns: %TRUE if the source was found and removed.
 *
 * Since: 1.1.6
 */
gboolean             gcut_event_loop_remove            (GCutEventLoop   *loop,
                                                        guint            tag);

G_END_DECLS

#endif /* __GCUT_EVENT_LOOP_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
