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
#define GCUT_PROCESS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GCUT_TYPE_PROCESS, GCutProcess))
#define GCUT_PROCESS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GCUT_TYPE_PROCESS, GCutProcessClass))
#define GCUT_IS_PROCESS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GCUT_TYPE_PROCESS))
#define GCUT_IS_PROCESS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GCUT_TYPE_PROCESS))
#define GCUT_PROCESS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GCUT_TYPE_PROCESS, GCutProcessClass))

#define GCUT_PROCESS_ERROR           (gcut_process_error_quark())

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
    GCUT_PROCESS_ERROR_INVALID_SIGNAL,
    GCUT_PROCESS_ERROR_PERMISSION_DENIED,
    GCUT_PROCESS_ERROR_TIMEOUT
} GCutProcessError;

GQuark       gcut_process_error_quark (void);

GType        gcut_process_get_type    (void) G_GNUC_CONST;

/**
 * gcut_process_new:
 * @command: the external command name to be ran
 * @...: the arguments for @command
 *
 * Creates a new #GCutProcess object that runs @command.
 *
 * Returns: a new #GCutProcess.
 *
 * Since: 1.1.6
 */
GCutProcess *gcut_process_new        (const gchar  *command,
                                      ...) G_GNUC_NULL_TERMINATED;

/**
 * gcut_process_new_command_line:
 * @command: a command line
 *
 * Creates a new #GCutProcess object that runs @command.
 *
 * Returns: a new #GCutProcess.
 *
 * Since: 1.1.6
 */
GCutProcess      *gcut_process_new_command_line (const gchar  *command);

/**
 * gcut_process_new_va_list:
 * @command: the external command name to be ran
 * @args: arguments for @command
 *
 * Creates a new #GCutProcess object that runs @command.
 *
 * Returns: a new #GCutProcess.
 *
 * Since: 1.1.6
 */
GCutProcess      *gcut_process_new_va_list   (const gchar  *command,
                                              va_list       args);

/**
 * gcut_process_new_argv:
 * @argc: the number of elements of @argv
 * @argv: the external command name to be ran and arguments
 * of it.
 *
 * Creates a new #GCutProcess object that runs @command.
 *
 * Returns: a new #GCutProcess.
 *
 * Since: 1.1.6
 */
GCutProcess      *gcut_process_new_argv  (gint          argc,
                                          gchar       **argv);

/**
 * gcut_process_new_strings:
 * @command: the external command name to be ran and
 * arguments of it. %NULL-terminated.
 *
 * Creates a new #GCutProcess object that runs @command.
 *
 * Returns: a new #GCutProcess.
 *
 * Since: 1.1.6
 */
GCutProcess      *gcut_process_new_strings   (const gchar **command);

/**
 * gcut_process_new_array:
 * @command: the external command name to be ran and
 * arguments of it. The #GArray should be zero-terminated.
 *
 * Creates a new #GCutProcess object that runs @command.
 *
 * Returns: a new #GCutProcess.
 *
 * Since: 1.1.6
 */
GCutProcess      *gcut_process_new_array     (GArray       *command);

/**
 * gcut_process_set_flags:
 * @process: a #GCutProcess
 * @flags: the flags to be passed to g_spawn_async_with_pipes().
 *
 * Sets @flags for spawning.
 *
 * Since: 1.1.6
 */
void          gcut_process_set_flags (GCutProcess  *process,
                                      GSpawnFlags   flags);

/**
 * gcut_process_get_flags:
 * @process: a #GCutProcess
 *
 * Gets @flags for spawning.
 *
 * Returns: the flags for spawning.
 *
 * Since: 1.1.6
 */
GSpawnFlags   gcut_process_get_flags (GCutProcess      *process);

/**
 * gcut_process_set_env:
 * @process: a #GCutProcess
 * @name: the first environment name.
 * @...: the value of @name, followed by name and value
 * pairs. %NULL-terminated.
 *
 * Sets environment variable for external command.
 *
 * Since: 1.1.6
 */
void          gcut_process_set_env   (GCutProcess  *process,
                                      const gchar  *name,
                                      ...) G_GNUC_NULL_TERMINATED;

/**
 * gcut_process_get_env:
 * @process: a #GCutProcess
 *
 * Gets environment variable for external command.
 *
 * Returns: a newly-allocated %NULL-terminated environment
 * variables. ("NAME1=VALUE1", "NAME2=VALUE2",
 * ..., %NULL) It should be freed by g_strfreev() when no longer
 * needed.
 *
 * Since: 1.1.6
 */
gchar       **gcut_process_get_env  (GCutProcess  *process);

/**
 * gcut_process_run:
 * @process: a #GCutProcess
 * @error: return location for an error, or %NULL
 *
 * Hatches a new external process.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 1.1.6
 */
gboolean     gcut_process_run (GCutProcess *process,
                               GError     **error);

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
gboolean     gcut_process_kill    (GCutProcess *process,
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
 * gcut_process_get_output_string:
 * @process: a #GCutProcess
 *
 * Since: 1.1.6
 */
GString     *gcut_process_get_output_string
                                  (GCutProcess *process);

/**
 * gcut_process_get_error_string:
 * @process: a #GCutProcess
 *
 * Since: 1.1.6
 */
GString     *gcut_process_get_error_string
                                  (GCutProcess *process);

#if GLIB_CHECK_VERSION(2,16,0)
#include <gio/gio.h>
/**
 * gcut_process_get_input_stream:
 * @process: a #GCutProcess
 *
 * Gets a #GInputStream connected with standard input of
 * external process.
 *
 * Returns: a #GInputStream if external process is running,
 * otherwise %NULL.
 *
 * Since: 1.1.6
 */
GInputStream *gcut_process_get_input_stream (GCutProcess *process);

/**
 * gcut_process_get_output_stream:
 * @process: a #GCutProcess
 *
 * Gets a #GOutputStream connected with standard output of
 * external process.
 *
 * Returns: a #GOutputStream if external process is running,
 * otherwise %NULL.
 *
 * Since: 1.1.6
 */
GOutputStream *gcut_process_get_output_stream (GCutProcess *process);

/**
 * gcut_process_get_error_stream:
 * @process: a #GCutProcess
 *
 * Gets a #GOutputStream connected with standard error output
 * of external process.
 *
 * Returns: a #GOutputStream if external process is running,
 * otherwise %NULL.
 *
 * Since: 1.1.6
 */
GOutputStream   *gcut_process_get_error_stream (GCutProcess *process);
#endif /* GLIB_CHECK_VERSION(2,16,0) */

G_END_DECLS

#endif /* __GCUT_PROCESS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
