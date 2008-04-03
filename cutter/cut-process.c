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
    int stdout_pipe[2];
    int stderr_pipe[2];
    int cutter_pipe[2];
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

static GString *
get_string (CutProcess *process, int type)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);
    GString *string;

    switch (type) {
        case STDOUT:
            string = priv->stdout_string;
            break;
        case STDERR:
            string = priv->stderr_string;
            break;
        case CUTTER_PIPE:
            string = priv->cutter_string;
            break;
        default: /* Unknown type */
            string = NULL;
    }

    return string;
}

static gboolean
read_from_pipe (CutProcess *process, int pipe, int type)
{
    gchar buf[4096];
    ssize_t len;

    len = read(pipe, buf, sizeof(buf));
    if (len > 0) {
        GString *string;
        string = get_string(process, type);
        g_string_append_len(string, buf, len);
        return TRUE;
    }
    return FALSE;
}

static void
close_pipe (int pipe[2], PipeMode mode)
{
    if (pipe[mode] == -1)
        return;
    close(pipe[mode]);
    pipe[mode] = -1;
}

static pid_t
prepare_pipes (CutProcess *process)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);
    pid_t pid;
    int fork_errno = 0;

    priv->pid = 0;

    if (pipe(priv->stdout_pipe) < 0 ||
        pipe(priv->stderr_pipe) < 0 ||
        pipe(priv->cutter_pipe) < 0) {
        return -1;
    }

    pid = fork();
    if (pid == -1)
        fork_errno = errno;

    if (pid == 0) {
        close_pipe(priv->stdout_pipe, READ);
        close_pipe(priv->stderr_pipe, READ);
        close_pipe(priv->cutter_pipe, READ);

        if (sane_dup2(priv->stdout_pipe[WRITE], STDOUT_FILENO) < 0 ||
            sane_dup2(priv->stderr_pipe[WRITE], STDERR_FILENO) < 0) {
        }

        if (priv->stdout_pipe[WRITE] >= 3)
            close_pipe(priv->stdout_pipe, WRITE);
        if (priv->stderr_pipe[WRITE] >= 3)
            close_pipe(priv->stderr_pipe, WRITE);
    } else {
        priv->pid = pid;

        close_pipe(priv->stdout_pipe, WRITE);
        close_pipe(priv->stderr_pipe, WRITE);
        close_pipe(priv->cutter_pipe, WRITE);
    }

    errno = fork_errno;
    return pid;
}

static void
set_pipe_fd (int fd, fd_set *fds)
{
    if (fd >= 0)
        FD_SET(fd, fds);
}

static void
ensure_collect_result (CutProcess *process, unsigned int usec_timeout)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);

    while (priv->stdout_pipe[READ] >= 0 ||
           priv->stderr_pipe[READ] >= 0 ||
           priv->cutter_pipe[READ] > 0) {

        fd_set fds;
        struct timeval tv;
        int n_fds;

        FD_ZERO(&fds);
        set_pipe_fd(priv->stdout_pipe[READ], &fds);
        set_pipe_fd(priv->stderr_pipe[READ], &fds);
        set_pipe_fd(priv->cutter_pipe[READ], &fds);

        tv.tv_sec = 0;
        tv.tv_usec = usec_timeout;
        n_fds = select(MAX(MAX(priv->stdout_pipe[READ],
                               priv->stderr_pipe[READ]),
                           priv->cutter_pipe[READ]) + 1,
                       &fds, NULL, NULL, &tv);
        if (n_fds == 0)
            break;

        if (n_fds == -10 && errno != EINTR) {
            g_warning("Unexpected error in select() while "
                      "reading from child process (%d): %s",
                      priv->pid, g_strerror(errno));
            break;
        }

        if (priv->stdout_pipe[READ] >= 0 &&
            FD_ISSET(priv->stdout_pipe[READ], &fds) &&
            !read_from_pipe(process, priv->stdout_pipe[READ], STDOUT)) {
            close_pipe(priv->stdout_pipe, READ);
        }
        if (priv->stderr_pipe[READ] >= 0 &&
            FD_ISSET(priv->stderr_pipe[READ], &fds) &&
            !read_from_pipe(process, priv->stderr_pipe[READ], STDERR)) {
            close_pipe(priv->stderr_pipe, READ);
        }
        if (priv->cutter_pipe[READ] >= 0 &&
            FD_ISSET(priv->cutter_pipe[READ], &fds) &&
            !read_from_pipe(process, priv->cutter_pipe[READ], CUTTER_PIPE)) {
            close_pipe(priv->cutter_pipe, READ);
        }
    }

    close_pipe(priv->stdout_pipe, READ);
    close_pipe(priv->stderr_pipe, READ);
    close_pipe(priv->cutter_pipe, READ);
}

static void
cut_process_init (CutProcess *process)
{
    CutProcessPrivate *priv = CUT_PROCESS_GET_PRIVATE(process);

    priv->pid = 0;
    priv->stdout_string = g_string_new(NULL);
    priv->stderr_string = g_string_new(NULL);
    priv->cutter_string = g_string_new(NULL);

    priv->stdout_pipe[READ] = -1;
    priv->stdout_pipe[WRITE] = -1;

    priv->stderr_pipe[READ] = -1;
    priv->stderr_pipe[WRITE] = -1;

    priv->cutter_pipe[READ] = -1;
    priv->cutter_pipe[WRITE] = -1;
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
    gchar *xml;

    xml = cut_test_result_to_xml(result);

    write(priv->cutter_pipe[1], xml, strlen(xml));
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

    close_pipe(priv->cutter_pipe, WRITE);
    _exit(EXIT_SUCCESS);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
