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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include "gcut-glib-event-loop.h"

#define GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(obj)                   \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                         \
                                 GCUT_TYPE_GLIB_EVENT_LOOP,     \
                                 GCutGLibEventLoopPrivate))

G_DEFINE_TYPE(GCutGLibEventLoop, gcut_glib_event_loop,
              GCUT_TYPE_EVENT_LOOP)

typedef struct _GCutGLibEventLoopPrivate	GCutGLibEventLoopPrivate;
struct _GCutGLibEventLoopPrivate
{
    GMainContext *context;
    GMainLoop *loop;
};

enum
{
    PROP_0,
    PROP_CONTEXT,
    PROP_LAST
};

static GObject *constructor      (GType                  type,
                                  guint                  n_props,
                                  GObjectConstructParam *props);

static void     dispose          (GObject         *object);
static void     set_property     (GObject         *object,
                                  guint            prop_id,
                                  const GValue    *value,
                                  GParamSpec      *pspec);
static void     get_property     (GObject         *object,
                                  guint            prop_id,
                                  GValue          *value,
                                  GParamSpec      *pspec);

static void     run              (GCutEventLoop   *loop);
static gboolean iterate          (GCutEventLoop   *loop,
                                  gboolean         may_block);
static void     quit             (GCutEventLoop   *loop);

static guint    watch_io         (GCutEventLoop   *loop,
                                  GIOChannel      *channel,
                                  GIOCondition     condition,
                                  GIOFunc          function,
                                  gpointer         data);

static guint    watch_child_full (GCutEventLoop   *loop,
                                  gint             priority,
                                  GPid             pid,
                                  GChildWatchFunc  function,
                                  gpointer         data,
                                  GDestroyNotify   notify);

static guint    add_timeout_full (GCutEventLoop   *loop,
                                  gint             priority,
                                  gdouble          interval_in_seconds,
                                  GSourceFunc      function,
                                  gpointer         data,
                                  GDestroyNotify   notify);

static guint    add_idle_full    (GCutEventLoop   *loop,
                                  gint             priority,
                                  GSourceFunc      function,
                                  gpointer         data,
                                  GDestroyNotify   notify);

static gboolean remove           (GCutEventLoop   *loop,
                                  guint            tag);

static void
gcut_glib_event_loop_class_init (GCutGLibEventLoopClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->constructor  = constructor;
    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    klass->parent_class.run = run;
    klass->parent_class.iterate = iterate;
    klass->parent_class.quit = quit;
    klass->parent_class.watch_io = watch_io;
    klass->parent_class.watch_child_full = watch_child_full;
    klass->parent_class.add_timeout_full = add_timeout_full;
    klass->parent_class.add_idle_full = add_idle_full;
    klass->parent_class.remove = remove;

    spec = g_param_spec_pointer("context",
                                "Context",
                                "Use the GMainContext for the event loop",
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_CONTEXT, spec);

    g_type_class_add_private(gobject_class, sizeof(GCutGLibEventLoopPrivate));
}

static GObject *
constructor (GType type, guint n_props, GObjectConstructParam *props)
{
    GObject *object;
    GObjectClass *klass;
    GCutGLibEventLoop *loop;
    GCutGLibEventLoopPrivate *priv;

    klass = G_OBJECT_CLASS(gcut_glib_event_loop_parent_class);
    object = klass->constructor(type, n_props, props);

    loop = GCUT_GLIB_EVENT_LOOP(object);
    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(loop);
    priv->loop = g_main_loop_new(priv->context, FALSE);

    return object;
}

static void
gcut_glib_event_loop_init (GCutGLibEventLoop *loop)
{
    GCutGLibEventLoopPrivate *priv;

    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(loop);
    priv->context = NULL;
    priv->loop = NULL;
}

static void
dispose (GObject *object)
{
    GCutGLibEventLoopPrivate *priv;

    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(object);

    if (priv->loop) {
        g_main_loop_unref(priv->loop);
        priv->loop = NULL;
    }

    if (priv->context) {
        g_main_context_unref(priv->context);
        priv->context = NULL;
    }

    G_OBJECT_CLASS(gcut_glib_event_loop_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    GCutGLibEventLoopPrivate *priv;

    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_CONTEXT:
        if (priv->context)
            g_main_context_unref(priv->context);
        priv->context = g_value_get_pointer(value);
        if (priv->context)
            g_main_context_ref(priv->context);
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
    GCutGLibEventLoop *loop;
    GCutGLibEventLoopPrivate *priv;

    loop = GCUT_GLIB_EVENT_LOOP(object);
    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(loop);
    switch (prop_id) {
    case PROP_CONTEXT:
        g_value_set_pointer(value, priv->context);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

GCutEventLoop *
gcut_glib_event_loop_new (GMainContext *context)
{
    return g_object_new(GCUT_TYPE_GLIB_EVENT_LOOP,
                        "context", context,
                        NULL);
}

static void
run (GCutEventLoop *loop)
{
    GCutGLibEventLoopPrivate *priv;

    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(loop);
    g_main_loop_run(priv->loop);
}

static gboolean
iterate (GCutEventLoop *loop, gboolean may_block)
{
    GCutGLibEventLoopPrivate *priv;

    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(loop);
    return g_main_context_iteration(g_main_loop_get_context(priv->loop),
                                    may_block);
}

static void
quit (GCutEventLoop *loop)
{
    GCutGLibEventLoopPrivate *priv;

    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(loop);
    g_main_loop_quit(priv->loop);
}

static guint
watch_io (GCutEventLoop *loop,
          GIOChannel      *channel,
          GIOCondition     condition,
          GIOFunc          function,
          gpointer         data)
{
    GCutGLibEventLoopPrivate *priv;
    guint watch_tag;
    GSource *watch_source;

    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(loop);
    watch_source = g_io_create_watch(channel, condition);
    g_source_set_callback(watch_source, (GSourceFunc)function, data, NULL);
    watch_tag = g_source_attach(watch_source,
                                g_main_loop_get_context(priv->loop));
    g_source_unref(watch_source);

    return watch_tag;
}

static guint
watch_child_full (GCutEventLoop *loop,
                  gint             priority,
                  GPid             pid,
                  GChildWatchFunc  function,
                  gpointer         data,
                  GDestroyNotify   notify)
{
    GCutGLibEventLoopPrivate *priv;
    guint tag;
    GSource *source;

    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(loop);
    source = g_child_watch_source_new(pid);
    if (priority != G_PRIORITY_DEFAULT)
        g_source_set_priority(source, priority);
    g_source_set_callback(source, (GSourceFunc)function, data, notify);
    tag = g_source_attach(source, g_main_loop_get_context(priv->loop));
    g_source_unref(source);

    return tag;
}

static guint
add_timeout_full (GCutEventLoop *loop,
                  gint             priority,
                  gdouble          interval_in_seconds,
                  GSourceFunc      function,
                  gpointer         data,
                  GDestroyNotify   notify)
{
    GCutGLibEventLoopPrivate *priv;
    guint tag;
    GSource *source;

    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(loop);
    source = g_timeout_source_new(interval_in_seconds * 1000);
    if (priority != G_PRIORITY_DEFAULT)
        g_source_set_priority(source, priority);
    g_source_set_callback(source, function, data, notify);
    tag = g_source_attach(source, g_main_loop_get_context(priv->loop));
    g_source_unref(source);

    return tag;
}

static guint
add_idle_full (GCutEventLoop *loop,
               gint             priority,
               GSourceFunc      function,
               gpointer         data,
               GDestroyNotify   notify)
{
    GCutGLibEventLoopPrivate *priv;
    GSource *source;
    guint tag;

    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(loop);
    source = g_idle_source_new();
    if (priority != G_PRIORITY_DEFAULT_IDLE)
        g_source_set_priority(source, priority);
    g_source_set_callback(source, function, data, notify);
    tag = g_source_attach(source, g_main_loop_get_context(priv->loop));
    g_source_unref(source);

    return tag;
}

static gboolean
remove (GCutEventLoop *loop, guint tag)
{
    GCutGLibEventLoopPrivate *priv;
    GSource *source;
    GMainContext *context;

    priv = GCUT_GLIB_EVENT_LOOP_GET_PRIVATE(loop);
    context = g_main_loop_get_context(priv->loop);
    source = g_main_context_find_source_by_id(context, tag);
    if (source)
        g_source_destroy(source);

    return source != NULL;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
