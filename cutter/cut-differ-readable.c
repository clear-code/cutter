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

static void diff        (CutDiffer     *differ,
                         CutDiffWriter *writer);
static void diff_line   (CutDiffer     *differ,
                         CutDiffWriter *writer,
                         gchar         *from_line,
                         gchar         *to_line);

static void
cut_differ_readable_class_init (CutDifferReadableClass *klass)
{
    CutDifferClass *differ_class;

    differ_class = CUT_DIFFER_CLASS(klass);
    differ_class->diff = diff;

    klass->diff_line = diff_line;
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
mark_equal (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, " ", lines, begin, end,
                               CUT_DIFF_WRITER_TAG_EQUAL_MARK);
}

static void
mark_inserted (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, "+", lines, begin, end,
                               CUT_DIFF_WRITER_TAG_INSERTED_MARK);
}

static void
mark_deleted (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, "-", lines, begin, end,
                               CUT_DIFF_WRITER_TAG_DELETED_MARK);
}

static void
mark_difference (CutDiffWriter *writer, gchar **lines, gint begin, gint end)
{
    cut_diff_writer_mark_lines(writer, "?", lines, begin, end,
                               CUT_DIFF_WRITER_TAG_DIFFERENCE_MARK);
}

static void
format_diff_point (CutDiffWriter *writer,
                   gchar *from_line, gchar *to_line,
                   gchar *from_tags, gchar *to_tags)
{
    gchar *lines[] = {NULL, NULL};

    from_tags = g_strchomp(from_tags);
    to_tags = g_strchomp(to_tags);

    lines[0] = from_line;
    mark_deleted(writer, lines, 0, 1);

    if (from_tags[0]) {
        lines[0] = from_tags;
        mark_difference(writer, lines, 0, 1);
    }

    lines[0] = to_line;
    mark_inserted(writer, lines, 0, 1);

    if (to_tags[0]) {
        lines[0] = to_tags;
        mark_difference(writer, lines, 0, 1);
    }
}

static void
diff_line (CutDiffer *differ, CutDiffWriter *writer,
           gchar *from_line, gchar *to_line)
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

    format_diff_point(writer, from_line, to_line, from_tags->str, to_tags->str);

    g_string_free(from_tags, TRUE);
    g_string_free(to_tags, TRUE);
    g_object_unref(matcher);
}

static void diff_lines (CutDiffer *differ, CutDiffWriter *writer,
                        gchar **from, gint from_begin, gint from_end,
                        gchar **to, gint to_begin, gint to_end);

static void
mark_diff_lines (CutDiffer *differ, CutDiffWriter *writer,
                 gchar **from, gint from_begin, gint from_end,
                 gchar **to, gint to_begin, gint to_end)
{
    if (from_begin < from_end) {
        if (to_begin < to_end) {
            diff_lines(differ, writer,
                       from, from_begin, from_end, to, to_begin, to_end);
        } else {
            mark_deleted(writer, from, from_begin, from_end);
        }
    } else {
        mark_inserted(writer, to, to_begin, to_end);
    }
}

static void
diff_lines (CutDiffer *differ, CutDiffWriter *writer,
            gchar **from, gint from_begin, gint from_end,
            gchar **to, gint to_begin, gint to_end)
{
    CutDifferReadableClass *klass;
    gdouble best_ratio, cut_off;
    gint from_equal_index, to_equal_index;
    gint from_best_index, to_best_index;
    gint to_index, from_index;

    klass = CUT_DIFFER_READABLE_GET_CLASS(differ);

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
                mark_inserted(writer, to, to_begin, to_end);
                mark_deleted(writer, from, from_begin, from_end);
            } else {
                mark_deleted(writer, from, from_begin, from_end);
                mark_inserted(writer, to, to_begin, to_end);
            }
            return;
        }
        from_best_index = from_equal_index;
        to_best_index = to_equal_index;
        best_ratio = 1.0;
    }

    mark_diff_lines(differ, writer,
                    from, from_begin, from_best_index,
                    to, to_begin, to_best_index);
    klass->diff_line(differ, writer, from[from_best_index], to[to_best_index]);
    mark_diff_lines(differ, writer,
                    from, from_best_index + 1, from_end,
                    to, to_best_index + 1, to_end);
}

static void
diff (CutDiffer *differ, CutDiffWriter *writer)
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
            mark_equal(writer, from,
                       operation->from_begin, operation->from_end);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
            mark_inserted(writer, to,
                          operation->to_begin, operation->to_end);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            mark_deleted(writer, from,
                         operation->from_begin, operation->from_end);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
            diff_lines(differ, writer,
                       from, operation->from_begin, operation->from_end,
                       to, operation->to_begin, operation->to_end);
            break;
        default:
            g_error("unknown operation type: %d", operation->type);
            break;
        }
    }
    cut_diff_writer_finish(writer);

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
cut_diff_readable_folded (const gchar *from, const gchar *to)
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
cut_diff_readable_is_interested (const gchar *diff)
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
cut_diff_readable_need_fold (const gchar *diff)
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
