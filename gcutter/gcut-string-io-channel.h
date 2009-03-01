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

#ifndef __GCUT_STRING_IO_CHANNEL_H__
#define __GCUT_STRING_IO_CHANNEL_H__

#include <glib-object.h>

G_BEGIN_DECLS

/* TODO: 1.0.7: write document */

GIOChannel    *gcut_string_io_channel_new        (const gchar *initial);
GString       *gcut_string_io_channel_get_string (GIOChannel  *channel);
void           gcut_string_io_channel_clear      (GIOChannel  *channel);
gsize          gcut_string_io_channel_get_buffer_limit
                                                 (GIOChannel  *channel);
void           gcut_string_io_channel_set_buffer_limit
                                                 (GIOChannel  *channel,
                                                  gsize        limit);
gsize          gcut_string_io_channel_get_limit  (GIOChannel  *channel);
void           gcut_string_io_channel_set_limit  (GIOChannel  *channel,
                                                  gsize        limit);
gboolean       gcut_string_io_channel_get_read_fail
                                                 (GIOChannel  *channel);
void           gcut_string_io_channel_set_read_fail
                                                 (GIOChannel  *channel,
                                                  gboolean     read_fail);
gboolean       gcut_string_io_channel_get_pipe_mode
                                                 (GIOChannel  *channel);
void           gcut_string_io_channel_set_pipe_mode
                                                 (GIOChannel  *channel,
                                                  gboolean     pipe_mode);

G_END_DECLS

#endif /* __GCUT_STRING_IO_CHANNEL_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
