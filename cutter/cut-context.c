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

#define CUT_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_CONTEXT, CutContextPrivate))

typedef struct _CutContextPrivate	CutContextPrivate;
struct _CutContextPrivate
{
    CutTest *test;
    gint verbose_level;
};

enum
{
    PROP_0,
    PROP_ASSERTION_COUNT
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
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_uint("assertion-count",
                             "Assertion Count",
                             "The number of assertion.",
                             0, G_MAXUINT32, 0,
                             G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_ASSERTION_COUNT, spec);

    g_type_class_add_private(gobject_class, sizeof(CutContextPrivate));
}

static void
cut_context_init (CutContext *context)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    priv->test = NULL;
    priv->verbose_level = 1;
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(cut_context_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
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
cut_context_set_verbose_level (CutContext *context, gint level)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    priv->verbose_level = level;
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

void
cut_context_set_error (CutContext *context,
                       const gchar *error_message,
                       const gchar *function_name,
                       const gchar *filename,
                       guint line)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);
    CutTest *test = priv->test;

    cut_test_set_error(test, error_message,
                       function_name, filename,
                       line);
}

void
cut_context_output_error_log (CutContext *context)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    /* output log */
    switch (priv->verbose_level) {
      case 1:
        g_print("error message should be in here!");
        break;
      default:
        g_print("E");
        break;
    }
}

void
cut_context_output_normal_log (CutContext *context)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);
    const gchar *test_function_name;

    test_function_name = cut_test_get_function_name(priv->test);

    /* output log */
    switch (priv->verbose_level) {
      case 1:
        g_print("(%s):.\n", test_function_name);
        break;
      default:
        g_print(".");
        break;
    }
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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
