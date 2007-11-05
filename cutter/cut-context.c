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

#define CUT_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_CONTEXT, CutContextPrivate))

#define RED_COLOR "\033[01;31m"
#define GREEN_COLOR "\033[01;32m"
#define YELLOW_COLOR "\033[01;33m"
#define BLUE_COLOR "\033[01;34m"
#define NORMAL_COLOR "\033[00m"

typedef struct _CutContextPrivate	CutContextPrivate;
struct _CutContextPrivate
{
    CutTest *test;
    gint verbose_level;
    gboolean use_color;
    gchar *base_dir;
};

enum
{
    PROP_0,
    PROP_VERBOSE_LEVEL
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

    spec = g_param_spec_int("verbose-level",
                            "Verbose Level",
                            "The number of representing verbosity level",
                            0, G_MAXINT32, 0,
                            G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_VERBOSE_LEVEL, spec);

    g_type_class_add_private(gobject_class, sizeof(CutContextPrivate));
}

static void
cut_context_init (CutContext *context)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    priv->test = NULL;
    priv->verbose_level = 0;
    priv->use_color = FALSE;
}

static void
dispose (GObject *object)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(object);

    if (priv->base_dir) {
        g_free(priv->base_dir);
        priv->base_dir = NULL;
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
      case PROP_VERBOSE_LEVEL:
        priv->verbose_level = g_value_get_uint(value);
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
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(object);

    switch (prop_id) {
      case PROP_VERBOSE_LEVEL:
        g_value_set_uint(value, priv->verbose_level);
        break;
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
cut_context_set_base_dir (CutContext *context, const gchar *base_dir)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    if (priv->base_dir) {
        g_free(priv->base_dir);
        priv->base_dir = NULL;
    }

    if (base_dir) {
        priv->base_dir = g_strdup(base_dir);
    }
}

void
cut_context_set_use_color (CutContext *context, gboolean use_color)
{
    CutContextPrivate *priv = CUT_CONTEXT_GET_PRIVATE(context);

    priv->use_color = use_color;
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
    const CutTestError *error;

    error = cut_test_get_error(priv->test);

    /* output log */
    switch (priv->verbose_level) {
      case 2:
        g_print("%s:%d: (%s)\n", error->filename,
                                 error->line,
                                 error->function_name);
        if (priv->use_color)
            g_print(RED_COLOR"%s"NORMAL_COLOR"\n", error->message);
        else
            g_print("%s\n", error->message);
        break;
      case 1:
        if (priv->use_color)
            g_print(RED_COLOR"F"NORMAL_COLOR);
        else
            g_print("F");
        break;
      case 0:
      default:
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
      case 2:
        g_print("(%s):.\n", test_function_name);
        break;
      case 1:
        g_print(".");
        break;
      case 0:
      default:
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
