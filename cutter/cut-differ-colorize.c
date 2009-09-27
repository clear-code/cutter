/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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
#include "cut-differ-colorize.h"
#include "cut-utils.h"

#define RED_COLOR "\033[01;31m"
#define RED_BACK_COLOR "\033[41m"
#define GREEN_COLOR "\033[01;32m"
#define GREEN_BACK_COLOR "\033[01;42m"
#define YELLOW_COLOR "\033[01;33m"
#define BLUE_COLOR "\033[01;34m"
#define BLUE_BACK_COLOR "\033[01;44m"
#define MAGENTA_COLOR "\033[01;35m"
#define CYAN_COLOR "\033[01;36m"
#define CYAN_BACK_COLOR "\033[01;46m"
#define WHITE_COLOR "\033[01;37m"
#define WHITE_BACK_COLOR "\033[01;47m"
#define BLACK_BACK_COLOR "\033[01;40m"
#define NORMAL_COLOR "\033[00m"

#define CRASH_COLOR RED_BACK_COLOR WHITE_COLOR

#define DIFF_LINE_EXPECTED_COLOR GREEN_COLOR
#define DIFF_LINE_ACTUAL_COLOR RED_COLOR
#define DIFF_LINE_CHANGE_COLOR CYAN_COLOR
#define DIFF_IN_LINE_EXPECTED_COLOR GREEN_BACK_COLOR WHITE_COLOR
#define DIFF_IN_LINE_ACTUAL_COLOR RED_BACK_COLOR WHITE_COLOR
#define DIFF_IN_LINE_CHANGE_COLOR CYAN_BACK_COLOR WHITE_COLOR

G_DEFINE_TYPE(CutDifferColorize, cut_differ_colorize, CUT_TYPE_DIFFER)

static void diff        (CutDiffer *differ, GArray *result);

static void
cut_differ_colorize_class_init (CutDifferColorizeClass *klass)
{
    CutDifferClass *differ_class;

    differ_class = CUT_DIFFER_CLASS(klass);
    differ_class->diff = diff;
}

static void
cut_differ_colorize_init (CutDifferColorize *differ)
{
}

CutDiffer *
cut_differ_colorize_new (const gchar *from, const gchar *to)
{
    return g_object_new(CUT_TYPE_DIFFER_COLORIZE,
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
    cut_differ_util_append_with_tag(result,
                                    GREEN_COLOR "+" NORMAL_COLOR " ",
                                    lines, begin, end);
}

static void
tag_deleted (GArray *result, gchar **lines, gint begin, gint end)
{
    cut_differ_util_append_with_tag(result,
                                    RED_COLOR "-" NORMAL_COLOR " ",
                                    lines, begin, end);
}

static void
tag_difference (GArray *result, gchar **lines, gint begin, gint end)
{
    cut_differ_util_append_with_tag(result,
                                    CYAN_COLOR "?" NORMAL_COLOR " ",
                                    lines, begin, end);
}

static void
append_segment (GString *string, const gchar *target, gint begin, gint end)
{
    const gchar *target_begin, *target_end;

    target_begin = g_utf8_offset_to_pointer(target, begin);
    target_end = g_utf8_offset_to_pointer(target, end);
    g_string_append_len(string, target_begin, target_end - target_begin);
}

static void
diff_line (GArray *result, gchar *from_line, gchar *to_line)
{
    GString *colorized_from_line, *colorized_to_line;
    CutSequenceMatcher *matcher;
    const GList *operations;

    colorized_from_line = g_string_new("");
    colorized_to_line = g_string_new("");
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
            append_segment(colorized_from_line, from_line,
                           operation->from_begin, operation->from_end);
            cut_differ_util_append_n_character(colorized_to_line,
                                               ' ', to_width);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
            g_string_append(colorized_from_line, CYAN_BACK_COLOR);
            cut_differ_util_append_n_character(colorized_from_line,
                                               ' ', to_width);
            g_string_append(colorized_from_line, NORMAL_COLOR);

            g_string_append(colorized_to_line, DIFF_IN_LINE_ACTUAL_COLOR);
            append_segment(colorized_to_line, to_line,
                           operation->to_begin, operation->to_end);
            g_string_append(colorized_to_line, NORMAL_COLOR);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            g_string_append(colorized_from_line, DIFF_IN_LINE_EXPECTED_COLOR);
            append_segment(colorized_from_line, from_line,
                           operation->from_begin, operation->from_end);
            g_string_append(colorized_from_line, NORMAL_COLOR);

            g_string_append(colorized_to_line, CYAN_BACK_COLOR);
            cut_differ_util_append_n_character(colorized_to_line,
                                               ' ', from_width);
            g_string_append(colorized_to_line, NORMAL_COLOR);
            break;
        case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
            g_string_append(colorized_from_line, DIFF_IN_LINE_EXPECTED_COLOR);
            append_segment(colorized_from_line, from_line,
                           operation->from_begin, operation->from_end);
            g_string_append(colorized_from_line, NORMAL_COLOR);
            if (from_width < to_width) {
                g_string_append(colorized_from_line, CYAN_BACK_COLOR);
                cut_differ_util_append_n_character(colorized_from_line,
                                                   ' ', from_width);
                g_string_append(colorized_from_line, NORMAL_COLOR);
            }

            g_string_append(colorized_to_line, DIFF_IN_LINE_ACTUAL_COLOR);
            append_segment(colorized_to_line, to_line,
                           operation->to_begin, operation->to_end);
            g_string_append(colorized_to_line, NORMAL_COLOR);
            if (to_width < from_width) {
                g_string_append(colorized_to_line, CYAN_BACK_COLOR);
                cut_differ_util_append_n_character(colorized_to_line,
                                                   ' ', from_width);
                g_string_append(colorized_to_line, NORMAL_COLOR);
            }
            break;
        default:
            g_error("unknown operation type: %d", operation->type);
            break;
        }
    }

    {
        gchar *lines[] = {NULL, NULL, NULL};
        lines[0] = colorized_from_line->str;
        lines[1] = colorized_to_line->str;
        tag_difference(result, lines, 0, 2);
    }

    g_string_free(colorized_from_line, TRUE);
    g_string_free(colorized_to_line, TRUE);
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
            gchar *from_line, *to_line;
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
            tag_inserted(result, from,
                         operation->from_begin, operation->from_end);
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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
