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

#define CUT_TYPE_DIFFER            (cut_differ_get_type ())
#define CUT_DIFFER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_DIFFER, CutDiffer))
#define CUT_DIFFER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_DIFFER, CutDifferClass))
#define CUT_IS_DIFFER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_DIFFER))
#define CUT_IS_DIFFER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_DIFFER))
#define CUT_DIFFER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_DIFFER, CutDifferClass))

typedef struct _CutDiffer         CutDiffer;
typedef struct _CutDifferClass    CutDifferClass;

struct _CutDiffer
{
    GObject object;
    gchar **from;
    gchar **to;
    GArray *result;
};

struct _CutDifferClass
{
    GObjectClass parent_class;
};

GType cut_differ_get_type       (void) G_GNUC_CONST;
G_DEFINE_TYPE(CutDiffer, cut_differ, G_TYPE_OBJECT)

static void dispose        (GObject         *object);

static void
cut_differ_class_init (CutDifferClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;
}

static void
cut_differ_init (CutDiffer *differ)
{
    differ->from = NULL;
    differ->to = NULL;
    differ->result = g_array_new(TRUE, FALSE, sizeof(gchar *));
}

static void
dispose (GObject *object)
{
    CutDiffer *differ;

    differ = CUT_DIFFER(object);

    if (differ->from) {
        g_strfreev(differ->from);
        differ->from = NULL;
    }

    if (differ->to) {
        g_strfreev(differ->to);
        differ->to = NULL;
    }

    if (differ->result) {
        gint i, len;

        for (i = 0, len = differ->result->len; i < len; i++) {
            g_free(g_array_index(differ->result, gchar *, i));
        }
        g_array_free(differ->result, TRUE);
        differ->result = NULL;
    }

    G_OBJECT_CLASS(cut_differ_parent_class)->dispose(object);
}

static gchar **
split_to_lines (const gchar *string)
{
    return g_regex_split_simple("\r?\n", string, 0, 0);
}

static void
tag (CutDiffer *differ, const gchar *tag, gchar **lines, gint begin, gint end)
{
    gint i;
    for (i = begin; i < end; i++) {
        gchar *line;

        line = g_strconcat(tag, lines[i], NULL);
        g_array_append_val(differ->result, line);
    }
}

static void
tag_equal (CutDiffer *differ, gchar **lines, gint begin, gint end)
{
    tag(differ, "  ", lines, begin, end);
}

static void
tag_inserted (CutDiffer *differ, gchar **lines, gint begin, gint end)
{
    tag(differ, "+ ", lines, begin, end);
}

static void
tag_deleted (CutDiffer *differ, gchar **lines, gint begin, gint end)
{
    tag(differ, "- ", lines, begin, end);
}

static void
tag_difference (CutDiffer *differ, gchar **lines, gint begin, gint end)
{
    tag(differ, "? ", lines, begin, end);
}

static gboolean
is_space_character (gpointer data, gpointer user_data)
{
    gchar character;

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
format_diff_point (CutDiffer *differ, gchar *from_line, gchar *to_line,
                   gchar *from_tags, gchar *to_tags)
{
    gchar *lines[] = {NULL, NULL};

    from_tags = g_strchomp(from_tags);
    to_tags = g_strchomp(to_tags);

    lines[0] = from_line;
    tag_deleted(differ, lines, 0, 1);

    if (from_tags[0]) {
        lines[0] = from_tags;
        tag_difference(differ, lines, 0, 1);
    }

    lines[0] = to_line;
    tag_inserted(differ, lines, 0, 1);

    if (to_tags[0]) {
        lines[0] = to_tags;
        tag_difference(differ, lines, 0, 1);
    }
}

static void
diff_line (CutDiffer *differ, gchar *from_line, gchar *to_line)
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

    format_diff_point(differ, from_line, to_line, from_tags->str, to_tags->str);

    g_string_free(from_tags, TRUE);
    g_string_free(to_tags, TRUE);
    g_object_unref(matcher);
}

static void diff_lines (CutDiffer *differ,
                        gint from_begin, gint from_end,
                        gint to_begin, gint to_end);

static void
tag_diff_lines (CutDiffer *differ,
                gint from_begin, gint from_end,
                gint to_begin, gint to_end)
{
    if (from_begin < from_end) {
        if (to_begin < to_end) {
            diff_lines(differ, from_begin, from_end, to_begin, to_end);
        } else {
            tag_deleted(differ, differ->from, from_begin, from_end);
        }
    } else {
        tag_inserted(differ, differ->to, to_begin, to_end);
    }
}

static void
diff_lines (CutDiffer *differ,
            gint from_begin, gint from_end,
            gint to_begin, gint to_end)
{
    gdouble best_ratio, cut_off;
    gint from_equal_index, to_equal_index;
    gint from_best_index, to_best_index;
    gint to_index, from_index;

    best_ratio = 0.74;
    cut_off = 0.75;

    from_equal_index = to_equal_index = -1;
    from_best_index = to_best_index = -1;

    for (to_index = to_begin; to_index < to_end; to_index++) {
        for (from_index = from_begin; from_index < from_end; from_index++) {
            CutSequenceMatcher *matcher;
            gchar *from_line, *to_line;
            gdouble ratio;

            from_line = differ->from[from_index];
            to_line = differ->to[to_index];
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
                tag_inserted(differ, differ->to, to_begin, to_end);
                tag_deleted(differ, differ->from, from_begin, from_end);
            } else {
                tag_deleted(differ, differ->from, from_begin, from_end);
                tag_inserted(differ, differ->to, to_begin, to_end);
            }
            return;
        }
        from_best_index = from_equal_index;
        to_best_index = to_equal_index;
        best_ratio = 1.0;
    }

    tag_diff_lines(differ,
                   from_begin, from_best_index,
                   to_begin, to_best_index);
    diff_line(differ, differ->from[from_best_index], differ->to[to_best_index]);
    tag_diff_lines(differ,
                   from_best_index + 1, from_end,
                   to_best_index + 1, to_end);
}

static void
readable_diff (CutDiffer *differ)
{
    CutSequenceMatcher *matcher;
    const GList *operations;

    matcher = cut_sequence_matcher_string_new(differ->from, differ->to);
    for (operations = cut_sequence_matcher_get_operations(matcher);
         operations;
         operations = g_list_next(operations)) {
        CutSequenceMatchOperation *operation = operations->data;

        switch (operation->type) {
          case CUT_SEQUENCE_MATCH_OPERATION_EQUAL:
            tag_equal(differ, differ->from,
                      operation->from_begin, operation->from_end);
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
            tag_inserted(differ, differ->to,
                         operation->to_begin, operation->to_end);
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            tag_deleted(differ, differ->from,
                        operation->from_begin, operation->from_end);
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
            diff_lines(differ,
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
    gchar *result;

    differ = g_object_new(CUT_TYPE_DIFFER, NULL);
    differ->from = split_to_lines(from);
    differ->to = split_to_lines(to);
    readable_diff(differ);
    result = g_strjoinv("\n", (gchar **)(differ->result->data));
    g_object_unref(differ);
    return result;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
