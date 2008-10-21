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

#ifndef __GCUT_SPAWN_H__
#define __GCUT_SPAWN_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GCUT_SPAWN_ERROR           (gcut_spawn_error_quark())

#define GCUT_TYPE_SPAWN            (gcut_spawn_get_type ())
#define GCUT_SPAWN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GCUT_TYPE_SPAWN, GCutSpawn))
#define GCUT_SPAWN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GCUT_TYPE_SPAWN, GCutSpawnClass))
#define GCUT_IS_SPAWN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GCUT_TYPE_SPAWN))
#define GCUT_IS_SPAWN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GCUT_TYPE_SPAWN))
#define GCUT_SPAWN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GCUT_TYPE_SPAWN, GCutSpawnClass))

typedef struct _GCutSpawn      GCutSpawn;
typedef struct _GCutSpawnClass GCutSpawnClass;

struct _GCutSpawn
{
    GObject object;
};

struct _GCutSpawnClass
{
    GObjectClass parent_class;

    void (*output_received) (GCutSpawn   *spawn,
                             const gchar *chunk,
                             gsize        size);
    void (*error_received)  (GCutSpawn   *spawn,
                             const gchar *chunk,
                             gsize        size);
    void (*reaped)          (GCutSpawn   *spawn,
                             gint         status);
    void (*error)           (GCutSpawn   *spawn,
                             GError      *error);
};

typedef enum
{
    GCUT_SPAWN_ERROR_COMMAND_LINE,
    GCUT_SPAWN_ERROR_IO_ERROR,
    GCUT_SPAWN_ERROR_ALREADY_RUNNING
} GCutSpawnError;

GQuark        gcut_spawn_error_quark (void);

GType         gcut_spawn_get_type    (void) G_GNUC_CONST;

GCutSpawn    *gcut_spawn_new         (const gchar  *command,
                                      ...) G_GNUC_NULL_TERMINATED;
GCutSpawn    *gcut_spawn_new_va_list (const gchar  *command,
                                      va_list       args);
GCutSpawn    *gcut_spawn_new_argv    (gint          argc,
                                      gchar       **argv);
GCutSpawn    *gcut_spawn_new_strings (const gchar **command);

void          gcut_spawn_set_flags   (GCutSpawn    *spawn,
                                      GSpawnFlags   flags);
GSpawnFlags   gcut_spawn_get_flags   (GCutSpawn    *spawn);

gboolean      gcut_spawn_run         (GCutSpawn    *spawn,
                                      GError      **error);
void          gcut_spawn_close       (GCutSpawn    *spawn);

gboolean      gcut_spawn_write       (GCutSpawn    *spawn,
                                      const gchar  *chunk,
                                      gsize         size,
                                      GError      **error);

GPid          gcut_spawn_get_pid     (GCutSpawn    *spawn);
GIOChannel   *gcut_spawn_get_input   (GCutSpawn    *spawn);
GIOChannel   *gcut_spawn_get_output  (GCutSpawn    *spawn);
GIOChannel   *gcut_spawn_get_error   (GCutSpawn    *spawn);

G_END_DECLS

#endif /* __GCUT_SPAWN_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
