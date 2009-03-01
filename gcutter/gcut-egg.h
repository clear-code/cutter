/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __GCUT_EGG_H__
#define __GCUT_EGG_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-egg
 * @title: External command
 * @short_description: Convenience API for using external
 * command.
 *
 * %GCutEgg encapsulates external command execution,
 * communication and termination. %GCutEgg reports an error
 * as %GError. It can be asserted easily by
 * gcut_assert_error().
 *
 * External command is specified to constructor like
 * gcut_egg_new(), gcut_egg_new_strings() and so
 * on. External command isn't run at the
 * time. gcut_egg_hatch() runs specified external command.
 *
 * Standard/Error outputs of external command are passed by
 * #GCutEgg::output-received/#GCutEgg::error-received signals
 * or %GIOChannel returned by
 * gcut_egg_get_output()/gcut_egg_get_error().
 * gcut_egg_write() writes a chunk to standard input of
 * external command.
 *
 * To wait external command finished, gcut_egg_wait() can be
 * used. It accepts timeout to avoid infinite waiting.
 *
 * e.g.:
 * |[
 * static GString *output_string;
 * static GCutEgg *egg;
 *
 * void
 * cut_setup (void)
 * {
 *     output_string = g_string_new(NULL);
 *     egg = NULL;
 * }
 *
 * void
 * cut_teardown (void)
 * {
 *     if (output_string)
 *         g_string_free(output_string, TRUE);
 *     if (egg)
 *         g_object_unref(egg);
 * }
 *
 * static void
 * cb_output_received (GCutEgg *egg, const gchar *chunk, gsize size,
 *                     gpointer user_data)
 * {
 *     g_string_append_len(output_string, chunk, size);
 * }
 * void
 * test_echo (void)
 * {
 *     GError *error = NULL;
 *
 *     egg = gcut_egg_new("echo", "XXX", NULL);
 *     g_signal_connect(egg, "receive-output",
 *                      G_CALLBACK(cb_output_received), NULL);
 *
 *     gcut_egg_hatch(egg, &error);
 *     gcut_assert_error(error);
 *
 *     gcut_egg_wait(egg, 1000, &error);
 *     gcut_assert_error(error);
 *     cut_assert_equal_string("XXX\n", output_string->str);
 * }
 * ]|
 */

#define GCUT_TYPE_EGG            (gcut_egg_get_type ())
#define GCUT_EGG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GCUT_TYPE_EGG, GCutEgg))
#define GCUT_EGG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GCUT_TYPE_EGG, GCutEggClass))
#define GCUT_IS_EGG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GCUT_TYPE_EGG))
#define GCUT_IS_EGG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GCUT_TYPE_EGG))
#define GCUT_EGG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GCUT_TYPE_EGG, GCutEggClass))

#define GCUT_EGG_ERROR           (gcut_egg_error_quark())

typedef struct _GCutEgg      GCutEgg;
typedef struct _GCutEggClass GCutEggClass;

struct _GCutEgg
{
    GObject object;
};

struct _GCutEggClass
{
    GObjectClass parent_class;

    void (*output_received) (GCutEgg     *egg,
                             const gchar *chunk,
                             gsize        size);
    void (*error_received)  (GCutEgg     *egg,
                             const gchar *chunk,
                             gsize        size);
    void (*reaped)          (GCutEgg     *egg,
                             gint         status);
    void (*error)           (GCutEgg     *egg,
                             GError      *error);
};

/**
 * GCutEggError:
 * @GCUT_EGG_ERROR_COMMAND_LINE: Command line related error.
 * @GCUT_EGG_ERROR_IO_ERROR: IO error.
 * @GCUT_EGG_ERROR_ALREADY_RUNNING: External command is already running.
 * @GCUT_EGG_ERROR_NOT_RUNNING: External command isn't running.
 * @GCUT_EGG_ERROR_INVALID_OBJECT: Invalid %GCutEgg object is passed.
 * @GCUT_EGG_ERROR_TIMEOUT: Timeout.
 *
 * Error codes returned by %GCutEgg related operations.
 *
 * Since: 1.0.6
 */
typedef enum
{
    GCUT_EGG_ERROR_COMMAND_LINE,
    GCUT_EGG_ERROR_IO_ERROR,
    GCUT_EGG_ERROR_ALREADY_RUNNING,
    GCUT_EGG_ERROR_NOT_RUNNING,
    GCUT_EGG_ERROR_INVALID_OBJECT,
    GCUT_EGG_ERROR_TIMEOUT
} GCutEggError;

GQuark        gcut_egg_error_quark   (void);

GType         gcut_egg_get_type      (void) G_GNUC_CONST;

/**
 * gcut_egg_new:
 * @command: the external command name to be ran
 * @...: the arguments for @command
 *
 * Creates a new %GCutEgg object that runs @command.
 *
 * Returns: a new %GCutEgg.
 *
 * Since: 1.0.6
 */
GCutEgg      *gcut_egg_new           (const gchar  *command,
                                      ...) G_GNUC_NULL_TERMINATED;

/**
 * gcut_egg_new_va_list:
 * @command: the external command name to be ran
 * @args: arguments for @command
 *
 * Creates a new %GCutEgg object that runs @command.
 *
 * Returns: a new %GCutEgg.
 *
 * Since: 1.0.6
 */
GCutEgg      *gcut_egg_new_va_list   (const gchar  *command,
                                      va_list       args);

/**
 * gcut_egg_new_argv:
 * @argc: the number of elements of @argv
 * @argv: the external command name to be ran and arguments
 * of it.
 *
 * Creates a new %GCutEgg object that runs @command.
 *
 * Returns: a new %GCutEgg.
 *
 * Since: 1.0.6
 */
GCutEgg      *gcut_egg_new_argv      (gint          argc,
                                      gchar       **argv);

/**
 * gcut_egg_new_strings:
 * @command: the external command name to be ran and
 * arguments of it. %NULL-terminated.
 *
 * Creates a new %GCutEgg object that runs @command.
 *
 * Returns: a new %GCutEgg.
 *
 * Since: 1.0.6
 */
GCutEgg      *gcut_egg_new_strings   (const gchar **command);

/**
 * gcut_egg_new_array:
 * @command: the external command name to be ran and
 * arguments of it. The %GArray should be zero-terminated.
 *
 * Creates a new %GCutEgg object that runs @command.
 *
 * Returns: a new %GCutEgg.
 *
 * Since: 1.0.6
 */
GCutEgg      *gcut_egg_new_array     (GArray       *command);

/**
 * gcut_egg_set_flags:
 * @egg: a %GCutEgg
 * @flags: the flags to be passed to g_spawn_async_with_pipes().
 *
 * Sets @flags for spawning.
 *
 * Since: 1.0.6
 */
void          gcut_egg_set_flags     (GCutEgg      *egg,
                                      GSpawnFlags   flags);

/**
 * gcut_egg_get_flags:
 * @egg: a %GCutEgg
 *
 * Gets @flags for spawning.
 *
 * Returns: the flags for spawning.
 *
 * Since: 1.0.6
 */
GSpawnFlags   gcut_egg_get_flags     (GCutEgg      *egg);

/**
 * gcut_egg_set_env:
 * @egg: a %GCutEgg
 * @name: the first environment name.
 * @...: the value of @name, followed by name and value
 * pairs. %NULL-terminated.
 *
 * Sets environment variable for external command.
 *
 * Since: 1.0.6
 */
void          gcut_egg_set_env       (GCutEgg      *egg,
                                      const gchar  *name,
                                      ...) G_GNUC_NULL_TERMINATED;

/**
 * gcut_egg_get_env:
 * @egg: a %GCutEgg
 *
 * Gets environment variable for external command.
 *
 * Returns: a newly-allocated %NULL-terminated environment
 * variables. ("NAME1=VALUE1", "NAME2=VALUE2",
 * ..., %NULL) It should be freed by g_strfreev() when no longer
 * needed.
 *
 * Since: 1.0.6
 */
gchar       **gcut_egg_get_env       (GCutEgg      *egg);

/**
 * gcut_egg_hatch:
 * @egg: a %GCutEgg
 * @error: return location for an error, or %NULL
 *
 * Hatches a new external process.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 1.0.6
 */
gboolean      gcut_egg_hatch         (GCutEgg      *egg,
                                      GError      **error);

/**
 * gcut_egg_close:
 * @egg: a %GCutEgg
 *
 * Closes a hatched external process. It is closed
 * implicitly on destroy.
 *
 * Since: 1.0.6
 */
void          gcut_egg_close         (GCutEgg      *egg);

/**
 * gcut_egg_write:
 * @egg: a %GCutEgg
 * @chunk: the data to be wrote
 * @size: the size of @chunk
 * @error: return location for an error, or %NULL
 *
 * Writes @chunk to external process's standard input.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 1.0.6
 */
gboolean      gcut_egg_write         (GCutEgg      *egg,
                                      const gchar  *chunk,
                                      gsize         size,
                                      GError      **error);

/**
 * gcut_egg_get_pid:
 * @egg: a %GCutEgg
 *
 * Gets the process ID of running external process. If
 * external process isn't running, 0 is returned.
 *
 * Returns: the process ID of running external process if
 * external process is running, otherwise 0.
 *
 * Since: 1.0.6
 */
GPid          gcut_egg_get_pid       (GCutEgg      *egg);

/**
 * gcut_egg_wait:
 * @egg: a %GCutEgg
 * @timeout: the timeout period in milliseconds
 * @error: return location for an error, or %NULL
 *
 * Waits running external process is finished while @timeout
 * milliseconds. If external process isn't finished while
 * @timeout milliseconds, %GCUT_EGG_ERROR_TIMEOUT error is
 * set and -1 is returned. If external process isn't
 * running, %GCUT_EGG_ERROR_NOT_RUNNING error is set and -1
 * is returned.
 *
 * Returns: an exit status of external process on success,
 * otherwise -1.
 *
 * Since: 1.0.6
 */
gint          gcut_egg_wait          (GCutEgg      *egg,
                                      guint         timeout,
                                      GError      **error);

/**
 * gcut_egg_kill:
 * @egg: a %GCutEgg
 * @signal_number: the signal number to be sent to external process
 *
 * Sends @signal_number signal to external process.
 *
 * Since: 1.0.6
 */
void          gcut_egg_kill          (GCutEgg      *egg,
                                      int           signal_number);

/**
 * gcut_egg_get_input:
 * @egg: a %GCutEgg
 *
 * Gets a %GIOChannel connected with standard input of
 * external process.
 *
 * Returns: a %GIOChannel if external process is running,
 * otherwise %NULL.
 *
 * Since: 1.0.6
 */
GIOChannel   *gcut_egg_get_input     (GCutEgg      *egg);

/**
 * gcut_egg_get_output:
 * @egg: a %GCutEgg
 *
 * Gets a %GIOChannel connected with standard output of
 * external process.
 *
 * Returns: a %GIOChannel if external process is running,
 * otherwise %NULL.
 *
 * Since: 1.0.6
 */
GIOChannel   *gcut_egg_get_output    (GCutEgg      *egg);

/**
 * gcut_egg_get_error:
 * @egg: a %GCutEgg
 *
 * Gets a %GIOChannel connected with standard error output
 * of external process.
 *
 * Returns: a %GIOChannel if external process is running,
 * otherwise %NULL.
 *
 * Since: 1.0.6
 */
GIOChannel   *gcut_egg_get_error     (GCutEgg      *egg);

/**
 * gcut_egg_get_forced_termination_wait_time:
 * @egg: a %GCutEgg
 *
 * Gets a wait time in milliseconds for forced termination
 * on dispose.
 *
 * Returns: a timeout value for waiting forced terminated
 * external command on dispose.
 *
 * Since: 1.0.6
 */
guint         gcut_egg_get_forced_termination_wait_time
                                     (GCutEgg      *egg);

/**
 * gcut_egg_set_forced_termination_wait_time:
 * @egg: a %GCutEgg
 * @timeout: the timeout value in milliseconds
 *
 * Sets a wait time in milliseconds for forced termination
 * on dispose. If @timeout is 0, it doesn't wait
 * termination of external process. The default value is 10.
 *
 * Since: 1.0.6
 */
void          gcut_egg_set_forced_termination_wait_time
                                     (GCutEgg      *egg,
                                      guint         timeout);

G_END_DECLS

#endif /* __GCUT_EGG_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
