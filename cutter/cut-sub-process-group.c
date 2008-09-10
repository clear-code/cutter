/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
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
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "cut-main.h"
#include "cut-sub-process-group.h"
#include "cut-sub-process.h"
#include "cut-test-context.h"

#define CUT_SUB_PROCESS_GROUP_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_SUB_PROCESS_GROUP, CutSubProcessGroupPrivate))

typedef struct _CutSubProcessGroupPrivate         CutSubProcessGroupPrivate;
struct _CutSubProcessGroupPrivate
{
    GList *sub_processes;
    CutTestContext *test_context;
    gint n_uncompleted_processes;
    gboolean all_success;
};

enum {
    PROP_0,
    PROP_TEST_CONTEXT
};

G_DEFINE_TYPE(CutSubProcessGroup, cut_sub_process_group, G_TYPE_OBJECT)

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
cut_sub_process_group_class_init (CutSubProcessGroupClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_object("test-context",
                               "Test context",
                               "The test context",
                               CUT_TYPE_TEST_CONTEXT,
                               G_PARAM_READABLE | G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class, PROP_TEST_CONTEXT, spec);

    g_type_class_add_private(gobject_class, sizeof(CutSubProcessGroupPrivate));
}

static void
cut_sub_process_group_init (CutSubProcessGroup *sub_process_group)
{
    CutSubProcessGroupPrivate *priv;

    priv = CUT_SUB_PROCESS_GROUP_GET_PRIVATE(sub_process_group);
    priv->sub_processes = NULL;
    priv->test_context = NULL;
    priv->n_uncompleted_processes = 0;
    priv->all_success = TRUE;
}

static void
dispose (GObject *object)
{
    CutSubProcessGroupPrivate *priv;

    priv = CUT_SUB_PROCESS_GROUP_GET_PRIVATE(object);

    if (priv->sub_processes) {
        g_list_foreach(priv->sub_processes, (GFunc)g_object_unref, NULL);
        g_list_free(priv->sub_processes);
        priv->sub_processes = NULL;
    }

    if (priv->test_context) {
        g_object_unref(priv->test_context);
        priv->test_context = NULL;
    }

    G_OBJECT_CLASS(cut_sub_process_group_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutSubProcessGroup *sub_process_group;

    sub_process_group = CUT_SUB_PROCESS_GROUP(object);
    switch (prop_id) {
      case PROP_TEST_CONTEXT:
        cut_sub_process_group_set_test_context(sub_process_group,
                                               g_value_get_object(value));
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
    CutSubProcessGroupPrivate *priv = CUT_SUB_PROCESS_GROUP_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_TEST_CONTEXT:
        g_value_set_object(value, priv->test_context);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutSubProcessGroup *
cut_sub_process_group_new (CutTestContext *test_context)
{
    return g_object_new(CUT_TYPE_SUB_PROCESS_GROUP,
                        "test-context", test_context,
                        NULL);
}

CutTestContext *
cut_sub_process_group_get_test_context (CutSubProcessGroup *sub_process_group)
{
    return CUT_SUB_PROCESS_GROUP_GET_PRIVATE(sub_process_group)->test_context;
}

void
cut_sub_process_group_set_test_context (CutSubProcessGroup *sub_process_group,
                                        CutTestContext *test_context)
{
    CutSubProcessGroupPrivate *priv;

    priv = CUT_SUB_PROCESS_GROUP_GET_PRIVATE(sub_process_group);
    if (priv->test_context)
        g_object_unref(priv->test_context);
    if (test_context)
        g_object_ref(test_context);
    priv->test_context = test_context;
}

static void
cb_complete_run (CutRunContext *pipeline, gboolean success, gpointer user_data)
{
    CutSubProcessGroup *group = user_data;
    CutSubProcessGroupPrivate *priv;

    priv = CUT_SUB_PROCESS_GROUP_GET_PRIVATE(group);
    priv->n_uncompleted_processes--;
    if (!success)
        priv->all_success = FALSE;

    g_signal_handlers_disconnect_by_func(pipeline,
                                         (GFunc)cb_complete_run,
                                         user_data);
}

void
cut_sub_process_group_add (CutSubProcessGroup *sub_process_group,
                           CutSubProcess *sub_process)
{
    CutSubProcessGroupPrivate *priv;
    CutRunContext *pipeline;

    priv = CUT_SUB_PROCESS_GROUP_GET_PRIVATE(sub_process_group);

    g_object_ref(sub_process);
    priv->n_uncompleted_processes++;

    pipeline = cut_sub_process_get_pipeline(sub_process);
    g_signal_connect(pipeline, "complete-run",
                     G_CALLBACK(cb_complete_run), sub_process_group);

    priv->sub_processes = g_list_prepend(priv->sub_processes, sub_process);
}


gboolean
cut_sub_process_group_run (CutSubProcessGroup *sub_process_group)
{
    CutSubProcessGroupPrivate *priv;
    GList *node;

    priv = CUT_SUB_PROCESS_GROUP_GET_PRIVATE(sub_process_group);
    for (node = priv->sub_processes; node; node = g_list_next(node)) {
        CutSubProcess *sub_process = node->data;

        if (!cut_sub_process_run(sub_process))
            priv->all_success = FALSE;
    }
    return priv->all_success;
}

void
cut_sub_process_group_run_async (CutSubProcessGroup *sub_process_group)
{
    CutSubProcessGroupPrivate *priv;

    priv = CUT_SUB_PROCESS_GROUP_GET_PRIVATE(sub_process_group);
    g_list_foreach(priv->sub_processes, (GFunc)cut_sub_process_run_async, NULL);
}

gboolean
cut_sub_process_group_wait (CutSubProcessGroup *sub_process_group)
{
    CutSubProcessGroupPrivate *priv;

    priv = CUT_SUB_PROCESS_GROUP_GET_PRIVATE(sub_process_group);
    while (priv->n_uncompleted_processes > 0)
        cut_run_iteration();
    return priv->all_success;
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
