/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@clear-code.com>
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

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-differ.h"
#include "cut-string-diff-writer.h"
#include "cut-utils.h"

#define CUT_DIFFER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_DIFFER, CutDifferPrivate))

typedef struct _CutDifferPrivate         CutDifferPrivate;
struct _CutDifferPrivate
{
    gchar **from;
    gchar **to;
    CutSequenceMatcher *matcher;
};

enum {
    PROP_0,
    PROP_FROM,
    PROP_TO
};

G_DEFINE_TYPE(CutDiffer, cut_differ, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);

static gdouble get_best_ratio    (CutDiffer *differ);
static gdouble get_cut_off_ratio (CutDiffer *differ);

static void
cut_differ_class_init (CutDifferClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;
    gobject_class->set_property = set_property;

    klass->get_best_ratio = get_best_ratio;
    klass->get_cut_off_ratio = get_cut_off_ratio;

    spec = g_param_spec_string("from",
                               "Original text",
                               "The original text",
                               NULL,
                               G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_FROM, spec);

    spec = g_param_spec_string("to",
                               "Modified text",
                               "The modified text",
                               NULL,
                               G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_TO, spec);

    g_type_class_add_private(gobject_class, sizeof(CutDifferPrivate));
}

static void
cut_differ_init (CutDiffer *differ)
{
    CutDifferPrivate *priv;

    priv = CUT_DIFFER_GET_PRIVATE(differ);
    priv->from = NULL;
    priv->to = NULL;
    priv->matcher = NULL;
}

static void
dispose (GObject *object)
{
    CutDifferPrivate *priv;

    priv = CUT_DIFFER_GET_PRIVATE(object);

    if (priv->matcher) {
        g_object_unref(priv->matcher);
        priv->matcher = NULL;
    }

    if (priv->from) {
        g_strfreev(priv->from);
        priv->from = NULL;
    }

    if (priv->to) {
        g_strfreev(priv->to);
        priv->to = NULL;
    }

    G_OBJECT_CLASS(cut_differ_parent_class)->dispose(object);
}

static gchar **
split_to_lines (const gchar *string)
{
    return g_regex_split_simple("\r?\n", string, 0, 0);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutDifferPrivate *priv = CUT_DIFFER_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_FROM:
        priv->from = split_to_lines(g_value_get_string(value));
        break;
    case PROP_TO:
        priv->to = split_to_lines(g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static gdouble
get_best_ratio (CutDiffer *differ)
{
    return 0.74;
}

static gdouble
get_cut_off_ratio (CutDiffer *differ)
{
    return 0.75;
}

void
cut_differ_diff (CutDiffer *differ, CutDiffWriter *writer)
{
    CUT_DIFFER_GET_CLASS(differ)->diff(differ, writer);
}

gdouble
cut_differ_get_best_ratio (CutDiffer *differ)
{
    return CUT_DIFFER_GET_CLASS(differ)->get_best_ratio(differ);
}

gdouble
cut_differ_get_cut_off_ratio (CutDiffer *differ)
{
    return CUT_DIFFER_GET_CLASS(differ)->get_cut_off_ratio(differ);
}

gchar **
cut_differ_get_from (CutDiffer *differ)
{
    return CUT_DIFFER_GET_PRIVATE(differ)->from;
}

gchar **
cut_differ_get_to (CutDiffer *differ)
{
    return CUT_DIFFER_GET_PRIVATE(differ)->to;
}

CutSequenceMatcher *
cut_differ_get_sequence_matcher (CutDiffer *differ)
{
    CutDifferPrivate *priv;

    priv = CUT_DIFFER_GET_PRIVATE(differ);
    if (!priv->matcher) {
        gchar **from, **to;

        from = cut_differ_get_from(differ);
        to = cut_differ_get_to(differ);
        priv->matcher = cut_sequence_matcher_string_new(from, to);
    }

    return priv->matcher;
}

gboolean
cut_differ_need_diff (CutDiffer *differ)
{
    CutSequenceMatcher *matcher;
    const GList *operations;
    gboolean have_equal = FALSE;
    gboolean have_insert_or_delete = FALSE;
    gdouble best_ratio;
    gchar **from, **to;

    best_ratio = cut_differ_get_best_ratio(differ);
    from = cut_differ_get_from(differ);
    to = cut_differ_get_to(differ);
    matcher = cut_differ_get_sequence_matcher(differ);
    for (operations = cut_sequence_matcher_get_operations(matcher);
         operations;
         operations = g_list_next(operations)) {
        CutSequenceMatchOperation *operation = operations->data;
        gint from_begin, from_end, from_index;
        gint to_begin, to_end, to_index;

        switch (operation->type) {
        case CUT_SEQUENCE_MATCH_OPERATION_EQUAL:
            have_equal = TRUE;
            if (have_insert_or_delete)
                return TRUE;
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
            from_begin = operation->from_begin;
            from_end = operation->from_end;
            to_begin = operation->to_begin;
            to_end = operation->to_end;
            for (from_index = from_begin; from_index < from_end; from_index++) {
                for (to_index = to_begin; to_index < to_end; to_index++) {
                    CutSequenceMatcher *char_matcher;
                    const gchar *from_line, *to_line;
                    gdouble ratio;

                    from_line = from[from_index];
                    to_line = to[to_index];
                    char_matcher = cut_sequence_matcher_char_new_full(
                        from_line, to_line,
                        cut_differ_util_is_space_character, NULL);
                    ratio = cut_sequence_matcher_get_ratio(char_matcher);
                    g_object_unref(char_matcher);
                    if (ratio > best_ratio)
                        return TRUE;
                }
            }
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
        case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            have_insert_or_delete = TRUE;
            if (have_equal)
                return TRUE;
            break;
        default:
            break;
        }
    }
    return FALSE;
}

gboolean
cut_differ_util_is_space_character (gpointer data, gpointer user_data)
{
    gint character;

    character = GPOINTER_TO_INT(data);
    return character == ' ' || character == '\t';
}

guint
cut_differ_util_compute_width (const gchar *string, guint begin, guint end)
{
    const gchar *last;
    guint width = 0;

    last = g_utf8_offset_to_pointer(string, end);
    for (string = g_utf8_offset_to_pointer(string, begin);
         string < last;
         string = g_utf8_next_char(string)) {
        if (g_unichar_iswide_cjk(g_utf8_get_char(string))) {
            width += 2;
        } else {
            width++;
        }
    }

    return width;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/