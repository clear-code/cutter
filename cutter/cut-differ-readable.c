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

#include "cut-sequence-matcher.h"
#include "cut-differ-readable.h"
#include "cut-utils.h"

G_DEFINE_TYPE(CutDifferReadable, cut_differ_readable, CUT_TYPE_DIFFER)

static void diff        (CutDiffer *differ, GArray *result);

static void
cut_differ_readable_class_init (CutDifferReadableClass *klass)
{
    CutDifferClass *differ_class;

    differ_class = CUT_DIFFER_CLASS(klass);
    differ_class->diff = diff;
}

static void
cut_differ_readable_init (CutDifferReadable *differ)
{
}

CutDiffer *
cut_differ_readable_new (const gchar *from, const gchar *to)
{
    return g_object_new(CUT_TYPE_DIFFER_READABLE,
                        "from", from,
                        "to", to,
                        NULL);
}

static void
tag_equal (GArray *result, gchar **lines, gint begin, gint end)
{
    cut_differ_util_append_with_tag(result, "  ", lines, begin, end);
}

static void
tag_inserted (GArray *result, gchar **lines, gint begin, gint end)
{
    cut_differ_util_append_with_tag(result, "+ ", lines, begin, end);
}

static void
tag_deleted (GArray *result, gchar **lines, gint begin, gint end)
{
    cut_differ_util_append_with_tag(result, "- ", lines, begin, end);
}

static void
tag_difference (GArray *result, gchar **lines, gint begin, gint end)
{
    cut_differ_util_append_with_tag(result, "? ", lines, begin, end);
}

static void
format_diff_point (GArray *result,
                   gchar *from_line, gchar *to_line,
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
    matcher =
        cut_sequence_matcher_char_new_full(from_line, to_line,
                                           cut_differ_util_is_space_character,
                                           NULL);
    for (operations = cut_sequence_matcher_get_operations(matcher);
         operations;
         operations = g_list_next(operations)) {
        CutSequenceMatchOperation *operation = operations->data;
        guint from_width, to_width;

        from_width = cut_differ_util_compute_width(from_line,
                                                   operation->from_begin,
                                                   operation->from_end);
        to_width = cut_differ_util_compute_width(to_line,
                                                 operation->to_begin,
                                                 operation->to_end);
        switch (operation->type) {
        case CUT_SEQUENCE_MATCH_OPERATION_EQUAL:
            cut_differ_util_append_n_character(from_tags, ' ', from_width);
            cut_differ_util_append_n_character(to_tags, ' ', to_width);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
            cut_differ_util_append_n_character(to_tags, '+', to_width);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            cut_differ_util_append_n_character(from_tags, '-', from_width);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
            cut_differ_util_append_n_character(from_tags, '^', from_width);
            cut_differ_util_append_n_character(to_tags, '^', to_width);
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

static void diff_lines (GArray *result,
                        gchar **from, gint from_begin, gint from_end,
                        gchar **to, gint to_begin, gint to_end);

static void
tag_diff_lines (GArray *result,
                gchar **from, gint from_begin, gint from_end,
                gchar **to, gint to_begin, gint to_end)
{
    if (from_begin < from_end) {
        if (to_begin < to_end) {
            diff_lines(result, from, from_begin, from_end, to, to_begin, to_end);
        } else {
            tag_deleted(result, from, from_begin, from_end);
        }
    } else {
        tag_inserted(result, to, to_begin, to_end);
    }
}

static void
diff_lines (GArray *result,
            gchar **from, gint from_begin, gint from_end,
            gchar **to, gint to_begin, gint to_end)
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
            const gchar *from_line, *to_line;
            gdouble ratio;

            from_line = from[from_index];
            to_line = to[to_index];
            if (strcmp(from_line, to_line) == 0) {
                if (from_equal_index < 0)
                    from_equal_index = from_index;
                if (to_equal_index < 0)
                    to_equal_index = to_index;
                continue;
            }

            matcher = cut_sequence_matcher_char_new_full(
                from_line, to_line,
                cut_differ_util_is_space_character, NULL);
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
                tag_inserted(result, to, to_begin, to_end);
                tag_deleted(result, from, from_begin, from_end);
            } else {
                tag_deleted(result, from, from_begin, from_end);
                tag_inserted(result, to, to_begin, to_end);
            }
            return;
        }
        from_best_index = from_equal_index;
        to_best_index = to_equal_index;
        best_ratio = 1.0;
    }

    tag_diff_lines(result,
                   from, from_begin, from_best_index,
                   to, to_begin, to_best_index);
    diff_line(result, from[from_best_index], to[to_best_index]);
    tag_diff_lines(result,
                   from, from_best_index + 1, from_end,
                   to, to_best_index + 1, to_end);
}

static void
diff (CutDiffer *differ, GArray *result)
{
    CutSequenceMatcher *matcher;
    const GList *operations;
    gchar **from, **to;

    from = cut_differ_get_from(differ);
    to = cut_differ_get_to(differ);
    matcher = cut_sequence_matcher_string_new(from, to);
    for (operations = cut_sequence_matcher_get_operations(matcher);
         operations;
         operations = g_list_next(operations)) {
        CutSequenceMatchOperation *operation = operations->data;

        switch (operation->type) {
        case CUT_SEQUENCE_MATCH_OPERATION_EQUAL:
            tag_equal(result, from,
                      operation->from_begin, operation->from_end);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
            tag_inserted(result, to,
                         operation->to_begin, operation->to_end);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            tag_deleted(result, from,
                        operation->from_begin, operation->from_end);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
            diff_lines(result,
                       from, operation->from_begin, operation->from_end,
                       to, operation->to_begin, operation->to_end);
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

    differ = cut_differ_readable_new(from, to);
    result = cut_differ_diff(differ);
    g_object_unref(differ);
    return result;
}

gchar *
cut_diff_folded_readable (const gchar *from, const gchar *to)
{
    gchar *folded_from, *folded_to;
    gchar *folded_diff;

    folded_from = cut_utils_fold(from);
    folded_to = cut_utils_fold(to);
    folded_diff = cut_diff_readable(folded_from, folded_to);
    g_free(folded_from);
    g_free(folded_to);

    return folded_diff;
}

gboolean
cut_diff_is_interested (const gchar *diff)
{
    if (!diff)
        return FALSE;

    if (!g_regex_match_simple("^[-+]", diff, G_REGEX_MULTILINE, 0))
        return FALSE;

    if (g_regex_match_simple("^[ ?]", diff, G_REGEX_MULTILINE, 0))
        return TRUE;

    if (g_regex_match_simple("(?:.*\n){2,}", diff, G_REGEX_MULTILINE, 0))
        return TRUE;

    return FALSE;
}

gboolean
cut_diff_need_fold (const gchar *diff)
{
    if (!diff)
        return FALSE;

    if (g_regex_match_simple("^[-+].{79}", diff, G_REGEX_MULTILINE, 0))
        return TRUE;

    return FALSE;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
