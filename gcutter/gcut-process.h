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

#ifndef __GCUT_PROCESS_H__
#define __GCUT_PROCESS_H__

#include <glib-object.h>
#include <cutter/cut-test-result.h>

G_BEGIN_DECLS

#define GCUT_TYPE_PROCESS            (gcut_process_get_type ())
#define GCUT_PROCESS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_PROCESS, GCutProcess))
#define GCUT_PROCESS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_PROCESS, GCutProcessClass))
#define GCUT_IS_PROCESS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_PROCESS))
#define GCUT_IS_PROCESS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_PROCESS))
#define GCUT_PROCESS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_PROCESS, GCutProcessClass))

typedef struct _GCutProcess      GCutProcess;
typedef struct _GCutProcessClass GCutProcessClass;

struct _GCutProcess
{
    GObject object;
};

struct _GCutProcessClass
{
    GObjectClass parent_class;

    void (*output_received) (GCutProcess *process,
                             const gchar *chunk,
                             gsize        size);
    void (*error_received)  (GCutProcess *process,
                             const gchar *chunk,
                             gsize        size);
    void (*reaped)          (GCutProcess *process,
                             gint         status);
    void (*error)           (GCutProcess *process,
                             GError      *error);
};

typedef enum
{
    GCUT_PROCESS_ERROR_COMMAND_LINE,
    GCUT_PROCESS_ERROR_IO_ERROR,
    GCUT_PROCESS_ERROR_ALREADY_RUNNING,
    GCUT_PROCESS_ERROR_NOT_RUNNING,
    GCUT_PROCESS_ERROR_INVALID_OBJECT,
    GCUT_PROCESS_ERROR_TIMEOUT
} GCutProcessError;

GQuark       gcut_process_error_quark (void);

GType        gcut_process_get_type    (void) G_GNUC_CONST;

GCutProcess *gcut_process_new         (void);

/**
 * gcut_process_get_pid:
 * @process: a #GCutProcess
 *
 * Gets the process ID of running external process. If
 * external process isn't running, 0 is returned.
 *
 * Returns: the process ID of running external process if
 * external process is running, otherwise 0.
 *
 * Since: 1.1.6
 */
GPid         gcut_process_get_pid (GCutProcess *process);

/**
 * gcut_process_wait:
 * @process: a #GCutProcess
 * @timeout: the timeout period in milliseconds
 * @error: return location for an error, or %NULL
 *
 * Waits running external process is finished while @timeout
 * milliseconds. If external process isn't finished while
 * @timeout milliseconds, %GCUT_PROCESS_ERROR_TIMEOUT error is
 * set and -1 is returned. If external process isn't
 * running, %GCUT_PROCESS_ERROR_NOT_RUNNING error is set and -1
 * is returned.
 *
 * Returns: an exit status of external process on success,
 * otherwise -1.
 *
 * Since: 1.1.6
 */
gint         gcut_process_wait    (GCutProcess *porcess,
                                   guint        timeout,
                                   GError     **error);
/**
 * gcut_process_kill:
 * @process: a #GCutProcess
 * @signal_number: the signal number to be sent to external process
 * @error: return location for an error, or %NULL
 *
 * Sends @signal_number signal to external process.
 *
 * Since: 1.1.6
 */
void         gcut_process_kill    (GCutProcess *process,
                                   gint         signal_number,
                                   GError     **error);

/**
 * gcut_process_write:
 * @process: a #GCutProcess
 * @chunk: the data to be wrote
 * @size: the size of @chunk
 * @error: return location for an error, or %NULL
 *
 * Writes @chunk to external process's standard input.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 1.1.6
 */
gboolean      gcut_process_write          (GCutProcess  *process,
                                           const gchar  *chunk,
                                           gsize         size,
                                           GError      **error);
/**
 * gcut_process_get_stdout:
 * @process: a #GCutProcess
 *
 * Since: 1.1.6
 */
GString     *gcut_process_get_output_string
                                  (GCutProcess *process);

/**
 * gcut_process_get_stderr:
 * @process: a #GCutProcess
 *
 * Since: 1.1.6
 */
GString     *gcut_process_get_error_string
                                  (GCutProcess *process);

G_END_DECLS

#endif /* __GCUT_PROCESS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
