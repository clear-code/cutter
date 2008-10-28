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

#ifndef __GCUT_EGG_H__
#define __GCUT_EGG_H__

#include <glib-object.h>

G_BEGIN_DECLS

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

    void (*output_received) (GCutEgg   *egg,
                             const gchar *chunk,
                             gsize        size);
    void (*error_received)  (GCutEgg   *egg,
                             const gchar *chunk,
                             gsize        size);
    void (*reaped)          (GCutEgg   *egg,
                             gint         status);
    void (*error)           (GCutEgg   *egg,
                             GError      *error);
};

typedef enum
{
    GCUT_EGG_ERROR_COMMAND_LINE,
    GCUT_EGG_ERROR_IO_ERROR,
    GCUT_EGG_ERROR_ALREADY_RUNNING
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

void          gcut_egg_set_flags     (GCutEgg      *egg,
                                      GSpawnFlags   flags);
GSpawnFlags   gcut_egg_get_flags     (GCutEgg      *egg);

gboolean      gcut_egg_hatch         (GCutEgg      *egg,
                                      GError      **error);
void          gcut_egg_close         (GCutEgg      *egg);

gboolean      gcut_egg_write         (GCutEgg      *egg,
                                      const gchar  *chunk,
                                      gsize         size,
                                      GError      **error);

GPid          gcut_egg_get_pid       (GCutEgg      *egg);
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
