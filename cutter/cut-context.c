/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-context.h"
#include "cut-context-private.h"
#include "cut-output.h"

#define CUT_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_CONTEXT, CutContextPrivate))

typedef struct _CutContextPrivate	CutContextPrivate;
struct _CutContextPrivate
{
    CutTest *test;
    CutOutput *output;
};

enum
{
    PROP_0
};

G_DEFINE_TYPE (CutContext, cut_context, G_TYPE_OBJECT)

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
cut_context_class_init (CutContextClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    g_type_class_add_private(gobject_class, sizeof(CutContextPrivate));
}

static void
cut_context_init (CutContext *context)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    priv->test = NULL;
    priv->output = cut_output_new();
}

static void
dispose (GObject *object)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(object);

    if (priv->output) {
        g_object_unref(priv->output);
        priv->output = NULL;
    }

    G_OBJECT_CLASS(cut_context_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(object);

    switch (prop_id) {
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
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(object);

    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutContext *
cut_context_new (void)
{
    return g_object_new(CUT_TYPE_CONTEXT, NULL);
}

void
cut_context_set_verbose_level (CutContext *context, CutVerboseLevel level)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_set_verbose_level(priv->output, level);
}

void
cut_context_set_verbose_level_by_name (CutContext *context, const gchar *name)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_set_verbose_level_by_name(priv->output, name);
}


void
cut_context_set_source_directory (CutContext *context, const gchar *directory)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_set_source_directory(priv->output, directory);
}

void
cut_context_set_use_color (CutContext *context, gboolean use_color)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_set_use_color(priv->output, use_color);
}

void
cut_context_increment_assertion_count (CutContext *context)
{
    CutTest *test = CUT_CONTEXT_GET_PRIVATE(context)->test;

    cut_test_increment_assertion_count(test);
}

void
cut_context_set_test (CutContext *context, CutTest *test)
{
    CUT_CONTEXT_GET_PRIVATE(context)->test = test;
}

CutTest *
cut_context_get_current_test (CutContext *context)
{
    return CUT_CONTEXT_GET_PRIVATE(context)->test;
}

void
cut_context_set_result (CutContext *context,
                        CutTestResultStatus status,
                        const gchar *result_message,
                        const gchar *function_name,
                        const gchar *filename,
                        guint line)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);
    CutTest *test = priv->test;

    cut_test_set_result(test, status, result_message,
                        function_name, filename,
                        line);
}

void
cut_context_output_error_log (CutContext *context)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_on_failure(priv->output, priv->test);
}

void
cut_context_output_normal_log (CutContext *context)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->output)
        cut_output_on_success(priv->output, priv->test);
}

CutContext *
cut_context_get_current (void)
{
    CutContext *current;

    current = g_private_get(cut_context_private);

    if (!current) {
        current = cut_context_new();
        g_private_set(cut_context_private, current);
    }

    return current;
}

void
cut_context_set_current (CutContext *context)
{
    g_private_set(cut_context_private, context);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
