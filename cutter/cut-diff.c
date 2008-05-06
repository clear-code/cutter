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
            tag(differ, "  ", differ->from,
                operation->from_begin, operation->from_end);
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_INSERT:
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_DELETE:
            break;
          case CUT_SEQUENCE_MATCH_OPERATION_REPLACE:
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
