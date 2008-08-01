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

#include <glib.h>

#include "cut-analyzer.h"
#include "cut-file-stream-reader.h"

#define BUFFER_SIZE 4096

#define CUT_ANALYZER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_ANALYZER, CutAnalyzerPrivate))

typedef struct _CutAnalyzerPrivate	CutAnalyzerPrivate;
struct _CutAnalyzerPrivate
{
    GList *run_contexts;
};

G_DEFINE_TYPE(CutAnalyzer, cut_analyzer, G_TYPE_OBJECT)

static void     dispose          (GObject         *object);

static void
cut_analyzer_class_init (CutAnalyzerClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    g_type_class_add_private(gobject_class, sizeof(CutAnalyzerPrivate));
}

static void
cut_analyzer_init (CutAnalyzer *analyzer)
{
    CutAnalyzerPrivate *priv = CUT_ANALYZER_GET_PRIVATE(analyzer);

    priv->run_contexts = FALSE;
}

static void
dispose (GObject *object)
{
    CutAnalyzerPrivate *priv = CUT_ANALYZER_GET_PRIVATE(object);

    if (priv->run_contexts) {
        g_list_foreach(priv->run_contexts, (GFunc)g_object_unref, NULL);
        g_list_free(priv->run_contexts);
        priv->run_contexts = NULL;
    }

    G_OBJECT_CLASS(cut_analyzer_parent_class)->dispose(object);
}

CutAnalyzer *
cut_analyzer_new (void)
{
    return g_object_new(CUT_TYPE_ANALYZER, NULL);
}

static void
error_cb (CutRunContext *context, GError *error, gpointer user_data)
{
    GError **user_error = user_data;

    *user_error = g_error_copy(error);
}

gboolean
cut_analyzer_analyze (CutAnalyzer *analyzer, const gchar *log_directory,
                      GError **error)
{
    CutAnalyzerPrivate *priv;
    GDir *log_dir;
    const gchar *name;
    GList *names = NULL;
    GList *node;
    gboolean success =TRUE;

    priv = CUT_ANALYZER_GET_PRIVATE(analyzer);

    log_dir = g_dir_open(log_directory, 0, error);
    if (!log_dir)
        return FALSE;

    while ((name = g_dir_read_name(log_dir))) {
        if (g_regex_match_simple("^\\d{4}(?:-\\d{2}){5}\\.xml$", name, 0, 0)) {
            gchar *file_name;

            file_name = g_build_filename(log_directory, name, NULL);
            names = g_list_prepend(names, file_name);
        }
    }
    g_dir_close(log_dir);

    names = g_list_reverse(g_list_sort(names, (GCompareFunc)g_utf8_collate));
    for (node = names; node && success; node = g_list_next(node)) {
        gchar *file_name = node->data;
        CutRunContext *reader;
        GError *local_error = NULL;

        reader = cut_file_stream_reader_new(file_name);
        g_signal_connect(reader, "error", G_CALLBACK(error_cb), &local_error);
        priv->run_contexts = g_list_append(priv->run_contexts, reader);
        cut_run_context_start(reader);
        if (local_error) {
            success = FALSE;
            g_propagate_error(error, local_error);
        }
    }

    if (names) {
        g_list_foreach(names, (GFunc)g_free, NULL);
        g_list_free(names);
    }

    return success;
}

const GList *
cut_analyzer_get_run_contexts (CutAnalyzer *analyzer)
{
    return CUT_ANALYZER_GET_PRIVATE(analyzer)->run_contexts;
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
