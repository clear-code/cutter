/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2010  Kouhei Sutou <kou@clear-code.com>
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
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>

#ifdef G_OS_WIN32
#  include <io.h>
#  define pipe(phandles) _pipe(phandles, 4096, _O_BINARY)
#else
#  include <unistd.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>

#include "cut-process.h"
#include "cut-experimental.h"
#include "cut-utils.h"

#define CUT_PROCESS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_PROCESS, CutProcessPrivate))

typedef struct _CutProcessPrivate	CutProcessPrivate;
struct _CutProcessPrivate
{
#ifdef G_OS_WIN32
    void *dummy;
#else
    pid_t pid;
    gchar *stdout_string;
    gchar *stderr_string;
    GString *cutter_string;
    GIOChannel *child_io;
    GIOChannel *parent_io;
    GIOChannel *stdout_read_io;
    GIOChannel *stderr_read_io;
#endif
};

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

#ifndef G_OS_WIN32
static int
sane_dup2 (int fd1, int fd2)
{
    int ret;
    do
        ret = dup2(fd1, fd2);
    while (ret < 0 && errno == EINTR);
    return ret;
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
create_read_io_channel (int pipe)
{
    return create_io_channel(pipe, G_IO_FLAG_IS_READABLE);
}

static GIOChannel *
create_write_io_channel (int pipe)
{
    return create_io_channel(pipe, G_IO_FLAG_IS_WRITEABLE);
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
        cut_utils_close_pipe(stdout_pipe, CUT_READ);
        cut_utils_close_pipe(stderr_pipe, CUT_READ);
        cut_utils_close_pipe(cutter_pipe, CUT_READ);

        if (sane_dup2(stdout_pipe[CUT_WRITE], STDOUT_FILENO) < 0 ||
            sane_dup2(stderr_pipe[CUT_WRITE], STDERR_FILENO) < 0) {
        }

        priv->child_io = create_write_io_channel(cutter_pipe[CUT_WRITE]);

        if (stdout_pipe[CUT_WRITE] >= 3)
            cut_utils_close_pipe(stdout_pipe, CUT_WRITE);
        if (stderr_pipe[CUT_WRITE] >= 3)
            cut_utils_close_pipe(stderr_pipe, CUT_WRITE);
    } else {
        priv->pid = pid;

        cut_utils_close_pipe(stdout_pipe, CUT_WRITE);
        cut_utils_close_pipe(stderr_pipe, CUT_WRITE);
        cut_utils_close_pipe(cutter_pipe, CUT_WRITE);

        priv->parent_io = create_read_io_channel(cutter_pipe[CUT_READ]);
        priv->stdout_read_io = create_read_io_channel(stdout_pipe[CUT_READ]);
        priv->stderr_read_io = create_read_io_channel(stderr_pipe[CUT_READ]);
    }

    errno = fork_errno;
    return pid;
}

static void
ensure_collect_result (CutProcess *process, unsigned int usec_timeout)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);

    /* workaround since g_io_add_watch() does not work I expect. */
    while (read_from_child(priv->parent_io, NULL, process))
        ;
}
#endif

static void
cut_process_init (CutProcess *process)
{
#ifndef G_OS_WIN32
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);

    priv->pid = 0;
    priv->stdout_string = NULL;
    priv->stderr_string = NULL;
    priv->cutter_string = g_string_new(NULL);
    priv->child_io = NULL;
    priv->parent_io = NULL;
#endif
}

static void
dispose (GObject *object)
{
#ifndef G_OS_WIN32
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(object);

    if (priv->pid) {
        kill(priv->pid, SIGKILL);
        priv->pid = 0;
    }

    if (priv->stdout_string) {
        g_free(priv->stdout_string);
        priv->stdout_string = NULL;
    }

    if (priv->stderr_string) {
        g_free(priv->stderr_string);
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
#endif

    G_OBJECT_CLASS(cut_process_parent_class)->dispose(object);
}

CutProcess *
cut_process_new ()
{
    return g_object_new(CUT_TYPE_PROCESS, NULL);
}


int
cut_process_fork (CutProcess *process)
{
#ifdef G_OS_WIN32
    errno = ENOSYS;
    return -1;
#else
    return prepare_pipes(process);
#endif
}

int
cut_process_wait (CutProcess *process, unsigned int usec_timeout)
{
#ifdef G_OS_WIN32
    return 0;
#else
    int status;
    pid_t pid;

    pid = CUT_PROCESS_GET_PRIVATE(process)->pid;

    if (pid == 0)
        return 0;

    while (waitpid(pid, &status, 0) == -1 && errno == EINTR)
        /* do nothing */;

    ensure_collect_result(process, usec_timeout);

    return status;
#endif
}

int
cut_process_get_pid (CutProcess *process)
{
#ifdef G_OS_WIN32
    return 0;
#else
    return CUT_PROCESS_GET_PRIVATE(process)->pid;
#endif
}

#ifndef G_OS_WIN32
static gchar *
read_from_channel (GIOChannel *source)
{
    GIOStatus status;
    gsize bytes_read;
    gchar *buffer = NULL;

    status = g_io_channel_read_to_end(source, &buffer,
                                      &bytes_read,
                                      NULL);
    return buffer;
}
#endif

const gchar *
cut_process_get_stdout_message (CutProcess *process)
{
#ifdef G_OS_WIN32
    return "";
#else
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);

    if (priv->stdout_string)
        g_free(priv->stdout_string);

    priv->stdout_string = read_from_channel(priv->stdout_read_io);

    return (const gchar*)priv->stdout_string;
#endif
}

const gchar *
cut_process_get_stderr_message (CutProcess *process)
{
#ifdef G_OS_WIN32
    return "";
#else
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);

    if (priv->stderr_string)
        g_free(priv->stderr_string);

    priv->stderr_string = read_from_channel(priv->stderr_read_io);

    return (const gchar*)priv->stderr_string;
#endif
}

gboolean
cut_process_send_test_result_to_parent (CutProcess *process, CutTestResult *result)
{
#ifdef G_OS_WIN32
    return TRUE;
#else
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
#endif
}

const gchar *
cut_process_get_result_from_child (CutProcess *process)
{
#ifdef G_OS_WIN32
    return "";
#else
    return CUT_PROCESS_GET_PRIVATE(process)->cutter_string->str;
#endif
}

void
cut_process_exit (CutProcess *process)
{
#ifndef G_OS_WIN32
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);

    g_io_channel_unref(priv->child_io);
    priv->child_io = NULL;
#endif
    _exit(EXIT_SUCCESS);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
