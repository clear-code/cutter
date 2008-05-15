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

static GList *modules = NULL;
static gchar *module_dir = NULL;

static void dispose        (GObject         *object);

static const gchar *
_cut_streamer_module_dir (void)
{
    const gchar *dir;

    if (module_dir)
        return module_dir;

    dir = g_getenv("CUT_STREAMER_MODULE_DIR");
    if (dir)
        return dir;

    return STREAMER_MODULEDIR;
}

static CutModule *
cut_streamer_load_module (const gchar *name)
{
    CutModule *module;

    module = cut_module_find(modules, name);
    if (module)
        return module;

    module = cut_module_load_module(_cut_streamer_module_dir(), name);
    if (module) {
        if (g_type_module_use(G_TYPE_MODULE(module))) {
            modules = g_list_prepend(modules, module);
            g_type_module_unuse(G_TYPE_MODULE(module));
        }
    }

    return module;
}

G_DEFINE_ABSTRACT_TYPE (CutStreamer, cut_streamer, G_TYPE_OBJECT)

static void
cut_streamer_class_init (CutStreamerClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
}

static void
cut_streamer_init (CutStreamer *streamer)
{
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(cut_streamer_parent_class)->dispose(object);
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
