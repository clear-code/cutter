/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  g新部 Hiroyuki Ikezoe  <poincare@ikezoe.net>
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

#include <glib.h>

#include "cut-streamer.h"
#include "cut-module.h"
#include "cut-listener-utils.h"

G_DEFINE_ABSTRACT_TYPE (CutStreamer, cut_streamer, G_TYPE_OBJECT)
CUT_DEFINE_LISTENER_MODULE(streamer, STREAMER)

static void
cut_streamer_class_init (CutStreamerClass *klass)
{
}

static void
cut_streamer_init (CutStreamer *streamer)
{
}

CutStreamer *
cut_streamer_new (const gchar *name,
                  const gchar *first_property, ...)
{
    CutModule *module;
    GObject *streamer;
    va_list var_args;

    module = cut_streamer_load_module(name);
    g_return_val_if_fail(module != NULL, NULL);

    va_start(var_args, first_property);
    streamer = cut_module_instantiate(module,
                                      first_property, var_args);
    va_end(var_args);

    return CUT_STREAMER(streamer);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
