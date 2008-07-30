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
#include "cut-runner.h"
#include "cut-stream-parser.h"

#define BUFFER_SIZE 4096

#define CUT_ANALYZER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_ANALYZER, CutAnalyzerPrivate))

typedef struct _CutAnalyzerPrivate	CutAnalyzerPrivate;
struct _CutAnalyzerPrivate
{
    CutStreamParser *parser;
    gboolean error_emitted;
};

static CutRunnerIface *parent_runner_iface;

static void runner_init (CutRunnerIface *iface);

G_DEFINE_TYPE_WITH_CODE(CutAnalyzer, cut_analyzer, CUT_TYPE_RUN_CONTEXT,
                        G_IMPLEMENT_INTERFACE(CUT_TYPE_RUNNER, runner_init))

static void     dispose          (GObject         *object);

static void     runner_run_async (CutRunner *runner);

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

    priv->error_emitted = FALSE;
}

static void
runner_init (CutRunnerIface *iface)
{
    parent_runner_iface = g_type_interface_peek_parent(iface);
    iface->run_async = runner_run_async;
    iface->run = NULL;
}

static void
dispose_parser (CutAnalyzerPrivate *priv)
{
    if (priv->parser) {
        CutStreamParser *parser;

        parser = priv->parser;
        priv->parser = NULL;
        g_object_unref(parser);
    }
}

static void
dispose (GObject *object)
{
    CutAnalyzerPrivate *priv = CUT_ANALYZER_GET_PRIVATE(object);

    dispose_parser(priv);

    G_OBJECT_CLASS(cut_analyzer_parent_class)->dispose(object);
}

CutRunContext *
cut_analyzer_new (void)
{
    return g_object_new(CUT_TYPE_ANALYZER, NULL);
}

CutRunContext *
cut_analyzer_new_from_run_context (CutRunContext *run_context)
{
    return g_object_new(CUT_TYPE_ANALYZER,
                        "log-directory", cut_run_context_get_log_directory(run_context),
                        "use-multi-thread", cut_run_context_get_multi_thread(run_context),
                        "target-test-case-names", cut_run_context_get_target_test_case_names(run_context),
                        "target-test-names", cut_run_context_get_target_test_names(run_context),
                        "test-case-order", cut_run_context_get_test_case_order(run_context),
                        "source-directory", cut_run_context_get_source_directory(run_context),
                        "command-line-args", cut_run_context_get_command_line_args(run_context),
                        NULL);
}

#define emit_error(analyzer, error, format, ...) do                     \
{                                                                       \
    CutAnalyzerPrivate *_priv;                                          \
    CutRunContext *_run_context;                                        \
                                                                        \
    _priv = CUT_ANALYZER_GET_PRIVATE(analyzer);                         \
    _run_context = CUT_RUN_CONTEXT(analyzer);                           \
    cut_run_context_emit_error(_run_context, "StreamError", error,      \
                               format, ## __VA_ARGS__);                 \
    _priv->error_emitted = TRUE;                                        \
    emit_complete_signal(analyzer, FALSE);                              \
} while (0)

static void
emit_complete_signal (CutAnalyzer *analyzer, gboolean success)
{
    g_signal_emit_by_name(analyzer, "complete-run", success);
}

static void
read_stream (CutAnalyzer *analyzer, GIOChannel *channel)
{
    CutAnalyzerPrivate *priv;
    gboolean eof = FALSE;

    priv = CUT_ANALYZER_GET_PRIVATE(analyzer);

    while (!priv->error_emitted) {
        GIOStatus status;
        gchar stream[BUFFER_SIZE + 1];
        gsize length = 0;
        GError *error = NULL;

        status = g_io_channel_read_chars(channel, stream, BUFFER_SIZE, &length,
                                         &error);
        if (status == G_IO_STATUS_EOF)
            eof = TRUE;

        if (error) {
            emit_error(analyzer, error, "failed to read stream");
            break;
        }

        if (length <= 0)
            break;

        cut_stream_parser_parse(priv->parser, stream, length, &error);
        if (error) {
            emit_error(analyzer, error, "failed to parse stream");
            break;
        }

        if (eof) {
            cut_stream_parser_end_parse(priv->parser, &error);
            if (error) {
                emit_error(analyzer, error, "failed to end parse stream");
                break;
            }
        } else {
            g_main_context_iteration(NULL, FALSE);
        }
    }
}

static void
run_async (CutAnalyzer *analyzer)
{
    CutAnalyzerPrivate *priv;
    GDir *log_dir;
    const gchar *name, *log_directory;
    GList *names = NULL;
    GList *node;
    GError *error = NULL;

    priv = CUT_ANALYZER_GET_PRIVATE(analyzer);

    log_directory = cut_run_context_get_log_directory(CUT_RUN_CONTEXT(analyzer));
    log_dir = g_dir_open(log_directory, 0, &error);
    if (error) {
        emit_error(analyzer, error, "failed to get child PID");
        return;
    }

    while ((name = g_dir_read_name(log_dir))) {
        if (g_regex_match_simple("^\\d{4}(?:-\\d{2}){5}\\.xml$", name, 0, 0)) {
            gchar *file_name;

            file_name = g_build_filename(log_directory, name, NULL);
            names = g_list_prepend(names, file_name);
        }
    }
    g_dir_close(log_dir);

    names = g_list_sort(names, (GCompareFunc)g_utf8_collate);
    for (node = names; node && !priv->error_emitted; node = g_list_next(node)) {
        gchar *file_name = node->data;
        GIOChannel *channel;

        g_print("%s\n", file_name);
        channel = g_io_channel_new_file(file_name, "r", &error);
        if (error) {
            emit_error(analyzer, error, "failed to open log file");
            break;
        }
        priv->parser = cut_stream_parser_new(CUT_RUN_CONTEXT(analyzer));
        read_stream(analyzer, channel);
        g_io_channel_unref(channel);
        dispose_parser(priv);
    }
    g_list_foreach(names, (GFunc)g_free, NULL);
    g_list_free(names);
}

static void
runner_run_async (CutRunner *runner)
{
    CutAnalyzer *analyzer;
    CutAnalyzerPrivate *priv;

    analyzer = CUT_ANALYZER(runner);
    run_async(analyzer);

    priv = CUT_ANALYZER_GET_PRIVATE(analyzer);
    emit_complete_signal(analyzer, !priv->error_emitted);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
