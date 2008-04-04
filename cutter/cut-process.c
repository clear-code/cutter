/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
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
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>

#include "cut-process.h"
#include "cut-experimental.h"

#define CUT_PROCESS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_PROCESS, CutProcessPrivate))

typedef struct _CutProcessPrivate	CutProcessPrivate;
struct _CutProcessPrivate
{
    pid_t pid;
    GString *stdout_string;
    GString *stderr_string;
    GString *cutter_string;
    GIOChannel *child_io;
    GIOChannel *parent_io;
    GIOChannel *stdout_read_io;
    GIOChannel *stderr_read_io;
};

typedef enum
{
    READ,
    WRITE
} PipeMode;

enum
{
    STDOUT,
    STDERR,
    CUTTER_PIPE
};

G_DEFINE_TYPE (CutProcess, cut_process, G_TYPE_OBJECT)

static void dispose         (GObject               *object);

static void
cut_process_class_init (CutProcessClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    g_type_class_add_private(gobject_class, sizeof(CutProcessPrivate));
}

static int
sane_dup2 (int fd1, int fd2)
{
    int ret;
    do
        ret = dup2(fd1, fd2);
    while (ret < 0 && errno == EINTR);
    return ret;
}

static void
close_pipe (int *pipe, PipeMode mode)
{
    if (pipe[mode] == -1)
        return;
    close(pipe[mode]);
    pipe[mode] = -1;
}

static GIOChannel *
create_io_channel (int pipe, GIOFlags flag)
{
    GIOChannel *channel;

    channel = g_io_channel_unix_new(pipe);
    g_io_channel_set_encoding(channel, NULL, NULL);
    g_io_channel_set_flags(channel, flag, NULL);
    g_io_channel_set_close_on_unref(channel, TRUE);

    return channel;
}

static GIOChannel *
create_read_io_channel (int pipe, GIOFunc read_func, CutProcess *process)
{
    GIOChannel *channel;

    channel = create_io_channel(pipe, G_IO_FLAG_IS_READABLE);
    g_io_add_watch(channel,
                   G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP,
                   (GIOFunc)read_func, process);

    return channel;
}

static GIOChannel *
create_write_io_channel (int pipe)
{
    return create_io_channel(pipe, G_IO_FLAG_IS_WRITEABLE);
}

static gboolean
read_from_stdout (GIOChannel *source, GIOCondition *condition,
                  gpointer data)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(data);
    GIOStatus status;
    gsize bytes_read;
    gchar buffer[4096];

    status = g_io_channel_read_chars(source, buffer, 
                                     sizeof(buffer),
                                     &bytes_read,
                                     NULL);
    g_string_append_len(priv->stdout_string, buffer, bytes_read);
    if (status == G_IO_STATUS_EOF)
        return FALSE;

    return TRUE;
}

static gboolean
read_from_stderr (GIOChannel *source, GIOCondition *condition,
                  gpointer data)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(data);
    GIOStatus status;
    gsize bytes_read;
    gchar buffer[4096];

    status = g_io_channel_read_chars(source, buffer, 
                                     sizeof(buffer),
                                     &bytes_read,
                                     NULL);
    g_string_append_len(priv->stderr_string, buffer, bytes_read);
    if (status == G_IO_STATUS_EOF)
        return FALSE;

    return TRUE;
}

static gboolean
read_from_child (GIOChannel *source, GIOCondition *condition,
                 gpointer data)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(data);
    GIOStatus status;
    gsize bytes_read;
    gchar buffer[4096];

    status = g_io_channel_read_chars(source, buffer, 
                                     sizeof(buffer),
                                     &bytes_read,
                                     NULL);
    g_string_append_len(priv->cutter_string, buffer, bytes_read);
    if (status == G_IO_STATUS_EOF)
        return FALSE;

    return TRUE;
}

static pid_t
prepare_pipes (CutProcess *process)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);
    pid_t pid;
    int fork_errno = 0;
    int stdout_pipe[2];
    int stderr_pipe[2];
    int cutter_pipe[2];

    priv->pid = 0;

    if (pipe(stdout_pipe) < 0 ||
        pipe(stderr_pipe) < 0 ||
        pipe(cutter_pipe) < 0) {
        return -1;
    }

    pid = fork();
    if (pid == -1)
        fork_errno = errno;

    if (pid == 0) {
        close_pipe(stdout_pipe, READ);
        close_pipe(stderr_pipe, READ);
        close_pipe(cutter_pipe, READ);

        if (sane_dup2(stdout_pipe[WRITE], STDOUT_FILENO) < 0 ||
            sane_dup2(stderr_pipe[WRITE], STDERR_FILENO) < 0) {
        }

        priv->child_io = create_write_io_channel(cutter_pipe[WRITE]);

        if (stdout_pipe[WRITE] >= 3)
            close_pipe(stdout_pipe, WRITE);
        if (stderr_pipe[WRITE] >= 3)
            close_pipe(stderr_pipe, WRITE);
    } else {
        priv->pid = pid;

        close_pipe(stdout_pipe, WRITE);
        close_pipe(stderr_pipe, WRITE);
        close_pipe(cutter_pipe, WRITE);

        priv->parent_io = create_read_io_channel(cutter_pipe[READ],
                                                 (GIOFunc)read_from_child, process);
        priv->stdout_read_io = create_read_io_channel(stdout_pipe[READ],
                                                      (GIOFunc)read_from_stdout, process);
        priv->stderr_read_io = create_read_io_channel(stderr_pipe[READ],
                                                      (GIOFunc)read_from_stderr, process);
    }

    errno = fork_errno;
    return pid;
}

static void
ensure_collect_result (CutProcess *process, unsigned int usec_timeout)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);

    /* workaround since g_io_add_watch() does not work I expect. */
    read_from_stdout(priv->stdout_read_io, NULL, process);
    read_from_stderr(priv->stderr_read_io, NULL, process);
    read_from_child(priv->parent_io, NULL, process);
}

static void
cut_process_init (CutProcess *process)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);

    priv->pid = 0;
    priv->stdout_string = g_string_new(NULL);
    priv->stderr_string = g_string_new(NULL);
    priv->cutter_string = g_string_new(NULL);
    priv->child_io = NULL;
    priv->parent_io = NULL;
}

static void
dispose (GObject *object)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(object);

    if (priv->pid) {
        kill(priv->pid, SIGKILL);
        priv->pid = 0;
    }

    if (priv->stdout_string) {
        g_string_free(priv->stdout_string, TRUE);
        priv->stdout_string = NULL;
    }

    if (priv->stderr_string) {
        g_string_free(priv->stderr_string, TRUE);
        priv->stderr_string = NULL;
    }

    if (priv->cutter_string) {
        g_string_free(priv->cutter_string, TRUE);
        priv->cutter_string = NULL;
    }

    if (priv->child_io) {
        g_io_channel_unref(priv->child_io);
        priv->child_io = NULL;
    }

    if (priv->parent_io) {
        g_io_channel_unref(priv->parent_io);
        priv->parent_io = NULL;
    }

    if (priv->stdout_read_io) {
        g_io_channel_unref(priv->stdout_read_io);
        priv->stdout_read_io = NULL;
    }

    if (priv->stderr_read_io) {
        g_io_channel_unref(priv->stderr_read_io);
        priv->stderr_read_io = NULL;
    }

    G_OBJECT_CLASS(cut_process_parent_class)->dispose(object);
}

CutProcess *
cut_process_new ()
{
    return g_object_new(CUT_TYPE_PROCESS,
                        NULL);
}


int
cut_process_fork (CutProcess *process)
{
    return prepare_pipes(process);
}

int
cut_process_wait (CutProcess *process, unsigned int usec_timeout)
{
    int status;
    pid_t pid;

    pid = CUT_PROCESS_GET_PRIVATE(process)->pid;

    if (pid == 0)
        return 0;

    while (waitpid(pid, &status, 0) == -1 && errno == EINTR)
        /* do nothing */;

    ensure_collect_result(process, usec_timeout);

    return status;
}

int
cut_process_get_pid (CutProcess *process)
{
    return CUT_PROCESS_GET_PRIVATE(process)->pid;
}

const gchar *
cut_process_get_stdout_message (CutProcess *process)
{
    return CUT_PROCESS_GET_PRIVATE(process)->stdout_string->str;
}

const gchar *
cut_process_get_stderr_message (CutProcess *process)
{
    return CUT_PROCESS_GET_PRIVATE(process)->stderr_string->str;
}

gboolean
cut_process_send_test_result_to_parent (CutProcess *process, CutTestResult *result)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);
    gchar *xml, *buffer;
    gsize bytes_written, length;
    GError *error = NULL;

    xml = cut_test_result_to_xml(result);
    if (!xml)
        return FALSE;

    length = strlen(xml);
    buffer = xml;

    while (length > 0) {
        g_io_channel_write_chars(priv->child_io,
                                 buffer, length,
                                 &bytes_written, &error);
        if (error) {
            g_error_free (error);
            return FALSE;
        }

        buffer += bytes_written;
        length -= bytes_written;
    }

    g_io_channel_flush(priv->child_io, NULL);

    g_free(xml);

    return TRUE;
}

const gchar *
cut_process_get_result_from_child (CutProcess *process)
{
    return CUT_PROCESS_GET_PRIVATE(process)->cutter_string->str;
}

void
cut_process_exit (CutProcess *process)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);

    g_io_channel_unref(priv->child_io);
    priv->child_io = NULL;
    _exit(EXIT_SUCCESS);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
