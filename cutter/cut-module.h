/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2006 Kouhei Sutou <kou@cozmixng.org>
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

#ifndef __CUT_MODULE_H__
#define __CUT_MODULE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#ifdef G_OS_WIN32
#  define CUT_MODULE_DEFINE_ADDITIONAL_VARIABLES(name)          \
    static gchar *win32_ ## name ## _module_dir = NULL;
#  define CUT_MODULE_RETURN_DEFAULT_MODULE_DIR(name, NAME) do   \
    {                                                           \
        if (!win32_ ## name ## _module_dir)                     \
            win32_ ## name ## _module_dir =                     \
                cut_win32_build_module_dir_name(#name);         \
        return win32_ ## name ## _module_dir;                   \
    } while (0)
#else
#  define CUT_MODULE_DEFINE_ADDITIONAL_VARIABLES(name)
#  define CUT_MODULE_RETURN_DEFAULT_MODULE_DIR(name, NAME)      \
    return NAME ## _MODULE_DIR
#endif


#define CUT_MODULE_DEFINE_INTERNAL_DEFINITIONS(name, NAME)              \
static GList *name ## s = NULL;                                         \
static gchar *name ## _module_dir = NULL;                               \
CUT_MODULE_DEFINE_ADDITIONAL_VARIABLES(name)                            \
                                                                        \
static const gchar *                                                    \
_cut_ ## name ## _module_dir (void)                                     \
{                                                                       \
    const gchar *dir;                                                   \
                                                                        \
    if (name ## _module_dir)                                            \
        return name ## _module_dir;                                     \
                                                                        \
    dir = g_getenv("CUT_" #NAME "_MODULE_DIR");                         \
    if (dir)                                                            \
        return dir;                                                     \
                                                                        \
    CUT_MODULE_RETURN_DEFAULT_MODULE_DIR(name, NAME);                   \
}                                                                       \
                                                                        \
static CutModule *                                                      \
cut_ ## name ## _load_module (const gchar *name)                        \
{                                                                       \
    CutModule *module;                                                  \
                                                                        \
    module = cut_module_find(name ## s, name);                          \
    if (module)                                                         \
        return module;                                                  \
                                                                        \
    module = cut_module_load_module(_cut_ ## name ## _module_dir(),     \
                                    name);                              \
    if (module) {                                                       \
        if (g_type_module_use(G_TYPE_MODULE(module))) {                 \
            name ## s = g_list_prepend(name ## s, module);              \
            g_type_module_unuse(G_TYPE_MODULE(module));                 \
        }                                                               \
    }                                                                   \
                                                                        \
    return module;                                                      \
}


#define CUT_TYPE_MODULE            (cut_module_get_type ())
#define CUT_MODULE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_MODULE, CutModule))
#define CUT_MODULE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_MODULE, CutModuleClass))
#define CUT_IS_MODULE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_MODULE))
#define CUT_IS_MODULE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_MODULE))
#define CUT_MODULE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_MODULE, CutModuleClass))

typedef struct _CutModule CutModule;
typedef struct _CutModuleClass CutModuleClass;

struct _CutModule
{
    GTypeModule object;
};

struct _CutModuleClass
{
    GTypeModuleClass parent_class;
};

GType     cut_module_get_type (void) G_GNUC_CONST;

CutModule   *cut_module_load_module         (const gchar    *base_dir,
                                             const gchar    *name);
GList       *cut_module_load_modules        (const gchar    *base_dir);
GList       *cut_module_load_modules_unique (const gchar    *base_dir,
                                             GList          *modules);
CutModule   *cut_module_find                (GList          *modules,
                                             const gchar    *name);
GObject     *cut_module_instantiate         (CutModule      *module,
                                             const gchar    *first_property,
                                             va_list         var_args);

GList    *cut_module_collect_registered_types (GList *modules);
GList    *cut_module_collect_names            (GList *modules);
void      cut_module_unload                   (CutModule *module);

G_END_DECLS

#endif /* __CUT_MODULE_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
