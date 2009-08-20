/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CUTTEST_SOUP_H__
#define __CUTTEST_SOUP_H__

#include <glib.h>
#include <libsoup/soup.h>

G_BEGIN_DECLS

SoupServer  *cuttest_soup_server_take_new (GMainContext *context);
const gchar *cuttest_soup_server_build_uri(SoupServer   *server,
                                           const gchar  *path);

G_END_DECLS

#endif


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
