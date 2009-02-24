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
 * #GCutEgg::output-received/GCutEgg::error-received signals
 * or %GIOChannel returned by
 * gcut_egg_get_output()/gcut_egg_get_error().
 * gcut_egg_write() writes a chunk to standard input of
 * external command.
 *
 * To wait external command finished, gcut_egg_wait() can be
 * used. It accepts timeout to avoid infinite waiting.
 *
 * Here is an example:
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

#define GCUT_EGG_ERROR           (gcut_egg_error_quark())

#define GCUT_TYPE_EGG            (gcut_egg_get_type ())
#define GCUT_EGG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GCUT_TYPE_EGG, GCutEgg))
#define GCUT_EGG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GCUT_TYPE_EGG, GCutEggClass))
#define GCUT_IS_EGG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GCUT_TYPE_EGG))
#define GCUT_IS_EGG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GCUT_TYPE_EGG))
#define GCUT_EGG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GCUT_TYPE_EGG, GCutEggClass))

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

GCutEgg      *gcut_egg_new           (const gchar  *command,
                                      ...) G_GNUC_NULL_TERMINATED;
GCutEgg      *gcut_egg_new_va_list   (const gchar  *command,
                                      va_list       args);
GCutEgg      *gcut_egg_new_argv      (gint          argc,
                                      gchar       **argv);
GCutEgg      *gcut_egg_new_strings   (const gchar **command);
GCutEgg      *gcut_egg_new_array     (GArray       *array);

void          gcut_egg_set_flags     (GCutEgg      *egg,
                                      GSpawnFlags   flags);
GSpawnFlags   gcut_egg_get_flags     (GCutEgg      *egg);

void          gcut_egg_set_env       (GCutEgg      *egg,
                                      const gchar  *name,
                                      ...) G_GNUC_NULL_TERMINATED;
gchar       **gcut_egg_get_env       (GCutEgg      *egg);

gboolean      gcut_egg_hatch         (GCutEgg      *egg,
                                      GError      **error);
void          gcut_egg_close         (GCutEgg      *egg);

gboolean      gcut_egg_write         (GCutEgg      *egg,
                                      const gchar  *chunk,
                                      gsize         size,
                                      GError      **error);

GPid          gcut_egg_get_pid       (GCutEgg      *egg);
gint          gcut_egg_wait          (GCutEgg      *egg,
                                      guint         interval,
                                      GError      **error);
void          gcut_egg_kill          (GCutEgg      *egg,
                                      int           signal_number);

GIOChannel   *gcut_egg_get_input     (GCutEgg      *egg);
GIOChannel   *gcut_egg_get_output    (GCutEgg      *egg);
GIOChannel   *gcut_egg_get_error     (GCutEgg      *egg);

guint         gcut_egg_get_kill_wait_milliseconds
                                     (GCutEgg      *egg);
void          gcut_egg_set_kill_wait_milliseconds
                                     (GCutEgg      *egg,
                                      guint         milliseconds);

G_END_DECLS

#endif /* __GCUT_EGG_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
