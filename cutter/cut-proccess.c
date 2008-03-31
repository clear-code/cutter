/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
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
#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>

#include "cut-proccess.h"
#include "cut-experimental.h"

#define CUT_PROCCESS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_PROCCESS, CutProccessPrivate))

typedef struct _CutProccessPrivate	CutProccessPrivate;
struct _CutProccessPrivate
{
    pid_t pid;
    GString *stdout_string;
    GString *stderr_string;
};

enum
{
    STDOUT,
    STDERR,
    CUTTER_PIPE
};

G_DEFINE_TYPE (CutProccess, cut_proccess, G_TYPE_OBJECT)

static void dispose         (GObject               *object);

static void
cut_proccess_class_init (CutProccessClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    g_type_class_add_private(gobject_class, sizeof(CutProccessPrivate));
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
get_string (CutProccess *proccess, int type)
{
    CutProccessPrivate *priv = CUT_PROCCESS_GET_PRIVATE(proccess);
    GString *string;

    switch (type) {
        case STDOUT:
            string = priv->stdout_string;
            break;
        case STDERR:
            string = priv->stderr_string;
            break;
        default: /* Unknown type */
            string = NULL;
    }

    return string;
}

static gboolean
read_from_pipe (CutProccess *proccess, int pipe, int type)
{
    gchar buf[4096];
    ssize_t len;

    len = read(pipe, buf, sizeof(buf));
    if (len > 0) {
        GString *string;
        string = get_string(proccess, type);
        g_string_append_len(string, buf, len);
        return TRUE;
    }
    return FALSE;
}

static pid_t
prepare_pipes (CutProccess *proccess)
{
    CutProccessPrivate *priv = CUT_PROCCESS_GET_PRIVATE(proccess);
    pid_t pid;
    int stdout_pipe[2] = { -1, -1 };
    int stderr_pipe[2] = { -1, -1 };
    int stdtst_pipe[2] = { -1, -1 };

    priv->pid = 0;

    if (pipe(stdout_pipe) < 0 ||
        pipe(stderr_pipe) < 0 ||
        pipe(stdtst_pipe) < 0) {
        return -1;
    }

    pid = fork();

    if (pid == 0) {
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        close(stdtst_pipe[0]);

        if (sane_dup2(stdout_pipe[1], 1) < 0 ||
            sane_dup2(stderr_pipe[1], 2) < 0) {
        }

        if (stdout_pipe[1] >= 3)
            close(stdout_pipe[1]);
        if (stderr_pipe[1] >= 3)
            close(stderr_pipe[1]);
        return 0;
    } else {
        priv->pid = pid;

        close(stdout_pipe[1]);
        close(stderr_pipe[1]);
        close(stdtst_pipe[1]);

        while (stdout_pipe[0] >= 0 ||
               stderr_pipe[0] >= 0 ||
               stdtst_pipe[0] > 0) {

            if (stdout_pipe[0] >=0 && 
                !read_from_pipe(proccess, stdout_pipe[0], STDOUT)) {
                stdout_pipe[0] = -1;
            }
            if (stderr_pipe[0] >=0 &&
                !read_from_pipe(proccess, stderr_pipe[0], STDERR)) {
                stderr_pipe[0] = -1;
            }
            if (stdtst_pipe[0] >=0 &&
                !read_from_pipe(proccess, stdtst_pipe[0], CUTTER_PIPE)) {
                stdtst_pipe[0] = -1;
            }
        }

        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        close(stdtst_pipe[0]);

        return pid;
    }
}

static void
cut_proccess_init (CutProccess *proccess)
{
    CutProccessPrivate *priv = CUT_PROCCESS_GET_PRIVATE(proccess);

    priv->pid = 0;
    priv->stdout_string = g_string_new(NULL);
    priv->stderr_string = g_string_new(NULL);
}

static void
dispose (GObject *object)
{
    CutProccessPrivate *priv = CUT_PROCCESS_GET_PRIVATE(object);

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

    G_OBJECT_CLASS(cut_proccess_parent_class)->dispose(object);
}

CutProccess *
cut_proccess_new ()
{
    return g_object_new(CUT_TYPE_PROCCESS,
                        NULL);
}


int
cut_proccess_fork (CutProccess *proccess)
{
    return prepare_pipes(proccess);
}

int
cut_proccess_get_pid (CutProccess *proccess)
{
    return CUT_PROCCESS_GET_PRIVATE(proccess)->pid;
}

const gchar *
cut_proccess_get_stdout_message (CutProccess *proccess)
{
    return CUT_PROCCESS_GET_PRIVATE(proccess)->stdout_string->str;
}

const gchar *
cut_proccess_get_stderr_message (CutProccess *proccess)
{
    return CUT_PROCCESS_GET_PRIVATE(proccess)->stderr_string->str;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
