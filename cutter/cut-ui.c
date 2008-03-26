/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <glib.h>

#include "cut-ui.h"
#include "cut-module.h"

static GList *uis = NULL;
static gchar *module_dir = NULL;

void
cut_ui_init (void)
{
}

static void
cut_ui_set_default_module_dir (const gchar *dir)
{
    if (module_dir)
        g_free(module_dir);
    module_dir = NULL;

    if (dir)
        module_dir = g_strdup(dir);
}

static void
cut_ui_unload (void)
{
    g_list_foreach(uis, (GFunc)cut_module_unload, NULL);
    g_list_free(uis);
    uis = NULL;
}

void
cut_ui_quit (void)
{
    cut_ui_unload();
    cut_ui_set_default_module_dir(NULL);
}

static const gchar *
_cut_ui_module_dir (void)
{
    const gchar *dir;

    if (module_dir)
        return module_dir;

    dir = g_getenv("CUT_UI_MODULE_DIR");
    if (dir)
        return dir;

    return UI_MODULEDIR;
}

static CutModule *
cut_ui_load_module (const gchar *name)
{
    CutModule *module;

    module = cut_module_find(uis, name);
    if (module)
        return module;

    module = cut_module_load_module(_cut_ui_module_dir(), name);
    if (module) {
        if (g_type_module_use(G_TYPE_MODULE(module))) {
            uis = g_list_prepend(uis, module);
            g_type_module_unuse(G_TYPE_MODULE(module));
        }
    }

    return module;
}

GObject *
cut_ui_new (const gchar *name, const gchar *first_property, ...)
{
    CutModule *module;
    GObject *ui;
    va_list var_args;

    module = cut_ui_load_module(name);
    g_return_val_if_fail(module != NULL, NULL);

    va_start(var_args, first_property);
    ui = cut_module_instantiate(module, first_property, var_args);
    va_end(var_args);

    return ui;
}

GType
cut_ui_get_type (void)
{
    static GType ui_type = 0;

    if (!ui_type) {
        const GTypeInfo ui_info = {
            sizeof(CutUIClass), /* class_size */
            NULL,           	/* base_init */
            NULL,			    /* base_finalize */
        };

        ui_type = g_type_register_static(G_TYPE_INTERFACE, "CutUI",
                                         &ui_info, 0);
    }

    return ui_type;
}

gboolean
cut_ui_run (CutUI *ui, CutRunner *runner)
{
    if (CUT_UI_GET_CLASS(ui)->run)
        return CUT_UI_GET_CLASS(ui)->run(ui, runner);
    return FALSE;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
