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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "cut-sequence-matcher.h"
#include "cut-diff.h"

#define CUT_DIFFER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_DIFFER, CutDifferPrivate))

typedef struct _CutDifferPrivate         CutDifferPrivate;
struct _CutDifferPrivate
{
    gchar **from;
    gchar **to;
    GArray *result;
};

G_DEFINE_TYPE(CutDiffer, cut_differ, G_TYPE_OBJECT)

static void   dispose        (GObject         *object);

static void
cut_differ_class_init (CutDifferClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;

    g_type_class_add_private(gobject_class, sizeof(CutDifferPrivate));
}

static void
cut_differ_init (CutDiffer *differ)
{
    CutDifferPrivate *priv;

    priv = CUT_DIFFER_GET_PRIVATE(differ);
    priv->from = NULL;
    priv->to = NULL;
}

static void
dispose (GObject *object)
{
    CutDifferPrivate *priv;

    priv = CUT_DIFFER_GET_PRIVATE(object);

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

gchar *
cut_differ_diff (CutDiffer *differ)
{
    gint i, len;
    GArray *result;
    gchar *diff;

    result = g_array_new(TRUE, FALSE, sizeof(gchar *));
    CUT_DIFFER_GET_CLASS(differ)->diff(differ, result);
    diff = g_strjoinv("\n", (gchar **)result->data);

    for (i = 0, len = result->len; i < len; i++) {
        g_free(g_array_index(result, gchar *, i));
    }
    g_array_free(result, TRUE);

    return diff;
}


static void readable_diff        (CutDiffer *differ, GArray *result);

typedef struct _CutDifferReadable         CutDifferReadable;
typedef struct _CutDifferReadableClass    CutDifferReadableClass;

struct _CutDifferReadable
{
    CutDiffer object;
};

struct _CutDifferReadableClass
{
    CutDifferClass parent_class;
};

#define CUT_TYPE_DIFFER_READABLE            (cut_differ_readable_get_type ())
#define CUT_DIFFER_READABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_DIFFER_READABLE, CutDifferReadable))
#define CUT_DIFFER_READABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_DIFFER_READABLE, CutDifferReadableClass))
#define CUT_IS_DIFFER_READABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_DIFFER_READABLE))
#define CUT_IS_DIFFER_READABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_DIFFER_READABLE))
#define CUT_DIFFER_READABLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_DIFFER_READABLE, CutDifferReadableClass))

GType      cut_differ_readable_get_type  (void) G_GNUC_CONST;

G_DEFINE_TYPE(CutDifferReadable, cut_differ_readable, CUT_TYPE_DIFFER)

static void
cut_differ_readable_class_init (CutDifferReadableClass *klass)
{
    CutDifferClass *differ_class;

    differ_class = CUT_DIFFER_CLASS(klass);
    differ_class->diff = readable_diff;
}

static void
cut_differ_readable_init (CutDifferReadable *differ)
{
}

static gchar **
split_to_lines (const gchar *string)
{
    return g_regex_split_simple("\r?\n", string, 0, 0);
}

static void
tag (GArray *result, const gchar *tag, gchar **lines, gint begin, gint end)
{
    gint i;
    for (i = begin; i < end; i++) {
        gchar *line;

        line = g_strconcat(tag, lines[i], NULL);
        g_array_append_val(result, line);
    }
}

static void
tag_equal (GArray *result, gchar **lines, gint begin, gint end)
{
    tag(result, "  ", lines, begin, end);
}

static void
tag_inserted (GArray *result, gchar **lines, gint begin, gint end)
{
    tag(result, "+ ", lines, begin, end);
}

static void
tag_deleted (GArray *result, gchar **lines, gint begin, gint end)
{
    tag(result, "- ", lines, begin, end);
}

static void
tag_difference (GArray *result, gchar **lines, gint begin, gint end)
{
    tag(result, "? ", lines, begin, end);
}

static gboolean
is_space_character (gpointer data, gpointer user_data)
{
    gint character;

    character = GPOINTER_TO_INT(data);
    return character == ' ' || character == '\t';
}

static void
append_n_tag (GString *string, gchar tag, gint n)
{
    gchar *tags;

    tags = g_strnfill(n, tag);
    g_string_append(string, tags);
    g_free(tags);
}

static void
format_diff_point (GArray *result, gchar *from_line, gchar *to_line,
                   gchar *from_tags, gchar *to_tags)
{
    gchar *lines[] = {NULL, NULL};

    from_tags = g_strchomp(from_tags);
    to_tags = g_strchomp(to_tags);

    lines[0] = from_line;
    tag_deleted(result, lines, 0, 1);

    if (from_tags[0]) {
        lines[0] = from_tags;
        tag_difference(result, lines, 0, 1);
    }

    lines[0] = to_line;
    tag_inserted(result, lines, 0, 1);

    if (to_tags[0]) {
        lines[0] = to_tags;
        tag_difference(result, lines, 0, 1);
    }
}

static void
diff_line (GArray *result, gchar *from_line, gchar *to_line)
{
    GString *from_tags, *to_tags;
    CutSequenceMatcher *matcher;
    const GList *operations;

    from_tags = g_string_new("");
    to_tags = g_string_new("");
    matcher = cut_sequence_matcher_char_new_full(from_line,
                                                 to_line,
                                                 is_space_character,
                                                 NULL);
    for (operations = cut_sequence_matcher_get_operations(matcher);
         operations;
         operations = g_list_next(operations)) {
        CutSequenceMatchOperation *operation = operations->data;
        gint from_size, to_size;

        from_size = operation->from_end - operation->from_begin;
        to_size = operation->to_end - operation->to_begin;
        switch (operation->type) {
          case CUT_SEQUENCE_MATCH_OPERATION_EQUAL:
            append_n_tag(from_tags, ' ', from_size);
            append_n_tag(to_tags, ' ', to_size);
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
            append_n_tag(to_tags, '+', to_size);
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            append_n_tag(from_tags, '-', from_size);
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
            append_n_tag(from_tags, '^', from_size);
            append_n_tag(to_tags, '^', to_size);
            break;
          default:
            g_error("unknown operation type: %d", operation->type);
            break;
        }
    }

    format_diff_point(result, from_line, to_line, from_tags->str, to_tags->str);

    g_string_free(from_tags, TRUE);
    g_string_free(to_tags, TRUE);
    g_object_unref(matcher);
}

static void diff_lines (CutDiffer *differ, GArray *result,
                        gint from_begin, gint from_end,
                        gint to_begin, gint to_end);

static void
tag_diff_lines (CutDiffer *differ, GArray *result,
                gint from_begin, gint from_end,
                gint to_begin, gint to_end)
{
    CutDifferPrivate *priv;

    priv = CUT_DIFFER_GET_PRIVATE(differ);
    if (from_begin < from_end) {
        if (to_begin < to_end) {
            diff_lines(differ, result, from_begin, from_end, to_begin, to_end);
        } else {
            tag_deleted(result, priv->from, from_begin, from_end);
        }
    } else {
        tag_inserted(result, priv->to, to_begin, to_end);
    }
}

static void
diff_lines (CutDiffer *differ, GArray *result,
            gint from_begin, gint from_end,
            gint to_begin, gint to_end)
{
    CutDifferPrivate *priv;
    gdouble best_ratio, cut_off;
    gint from_equal_index, to_equal_index;
    gint from_best_index, to_best_index;
    gint to_index, from_index;

    priv = CUT_DIFFER_GET_PRIVATE(differ);

    best_ratio = 0.74;
    cut_off = 0.75;

    from_equal_index = to_equal_index = -1;
    from_best_index = to_best_index = -1;

    for (to_index = to_begin; to_index < to_end; to_index++) {
        for (from_index = from_begin; from_index < from_end; from_index++) {
            CutSequenceMatcher *matcher;
            gchar *from_line, *to_line;
            gdouble ratio;

            from_line = priv->from[from_index];
            to_line = priv->to[to_index];
            if (strcmp(from_line, to_line) == 0) {
                if (from_equal_index < 0)
                    from_equal_index = from_index;
                if (to_equal_index < 0)
                    to_equal_index = to_index;
                continue;
            }

            matcher = cut_sequence_matcher_char_new_full(from_line,
                                                         to_line,
                                                         is_space_character,
                                                         NULL);
            ratio = cut_sequence_matcher_get_ratio(matcher);
            if (ratio > best_ratio) {
                best_ratio = ratio;
                from_best_index = from_index;
                to_best_index = to_index;
            }
            g_object_unref(matcher);
        }
    }

    if (best_ratio < cut_off) {
        if (from_equal_index < 0) {
            if (to_end - to_begin < from_end - from_begin) {
                tag_inserted(result, priv->to, to_begin, to_end);
                tag_deleted(result, priv->from, from_begin, from_end);
            } else {
                tag_deleted(result, priv->from, from_begin, from_end);
                tag_inserted(result, priv->to, to_begin, to_end);
            }
            return;
        }
        from_best_index = from_equal_index;
        to_best_index = to_equal_index;
        best_ratio = 1.0;
    }

    tag_diff_lines(differ, result,
                   from_begin, from_best_index,
                   to_begin, to_best_index);
    diff_line(result, priv->from[from_best_index], priv->to[to_best_index]);
    tag_diff_lines(differ, result,
                   from_best_index + 1, from_end,
                   to_best_index + 1, to_end);
}

static void
readable_diff (CutDiffer *differ, GArray *result)
{
    CutDifferPrivate *priv;
    CutSequenceMatcher *matcher;
    const GList *operations;

    priv = CUT_DIFFER_GET_PRIVATE(differ);
    matcher = cut_sequence_matcher_string_new(priv->from, priv->to);
    for (operations = cut_sequence_matcher_get_operations(matcher);
         operations;
         operations = g_list_next(operations)) {
        CutSequenceMatchOperation *operation = operations->data;

        switch (operation->type) {
          case CUT_SEQUENCE_MATCH_OPERATION_EQUAL:
            tag_equal(result, priv->from,
                      operation->from_begin, operation->from_end);
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
            tag_inserted(result, priv->to,
                         operation->to_begin, operation->to_end);
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            tag_deleted(result, priv->from,
                        operation->from_begin, operation->from_end);
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
            diff_lines(differ, result,
                       operation->from_begin, operation->from_end,
                       operation->to_begin, operation->to_end);
            break;
          default:
            g_error("unknown operation type: %d", operation->type);
            break;
        }
    }

    g_object_unref(matcher);
}

gchar *
cut_diff_readable (const gchar *from, const gchar *to)
{
    CutDiffer *differ;
    CutDifferPrivate *priv;
    gchar *result;

    differ = g_object_new(CUT_TYPE_DIFFER_READABLE, NULL);
    priv = CUT_DIFFER_GET_PRIVATE(differ);
    priv->from = split_to_lines(from);
    priv->to = split_to_lines(to);
    result = cut_differ_diff(differ);
    g_object_unref(differ);
    return result;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
