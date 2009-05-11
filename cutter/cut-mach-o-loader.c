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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#ifdef HAVE_MACH_O_LOADER_H
#  include <mach-o/loader.h>
#  include <mach-o/nlist.h>
#  include <mach-o/stab.h>
#endif
#include <glib/gstdio.h>

#include "cut-mach-o-loader.h"

#define CUT_MACH_O_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_MACH_O_LOADER, CutMachOLoaderPrivate))

typedef enum {
    ARCHITECTURE_UNKNOWN,
    ARCHITECTURE_32BIT,
    ARCHITECTURE_64BIT
} ArchitectureBit;

typedef struct _CutMachOLoaderPrivate	CutMachOLoaderPrivate;
struct _CutMachOLoaderPrivate
{
    gchar *so_filename;
    gchar *content;
    gsize length;
    ArchitectureBit bit;
};

enum
{
    PROP_0,
    PROP_SO_FILENAME
};

G_DEFINE_TYPE(CutMachOLoader, cut_mach_o_loader, G_TYPE_OBJECT)

static void dispose         (GObject               *object);
static void set_property    (GObject               *object,
                             guint                  prop_id,
                             const GValue          *value,
                             GParamSpec            *pspec);
static void get_property    (GObject               *object,
                             guint                  prop_id,
                             GValue                *value,
                             GParamSpec            *pspec);

static void
cut_mach_o_loader_class_init (CutMachOLoaderClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("so-filename",
                               ".so filename",
                               "The filename of shared object",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_SO_FILENAME, spec);

    g_type_class_add_private(gobject_class, sizeof(CutMachOLoaderPrivate));
}

static void
cut_mach_o_loader_init (CutMachOLoader *loader)
{
    CutMachOLoaderPrivate *priv = CUT_MACH_O_LOADER_GET_PRIVATE(loader);

    priv->so_filename = NULL;
    priv->content = NULL;
    priv->length = 0;
    priv->bit = ARCHITECTURE_UNKNOWN;
}

static void
dispose (GObject *object)
{
    CutMachOLoaderPrivate *priv = CUT_MACH_O_LOADER_GET_PRIVATE(object);

    if (priv->so_filename) {
        g_free(priv->so_filename);
        priv->so_filename = NULL;
    }

    if (priv->content) {
        g_free(priv->content);
        priv->content = NULL;
    }

    G_OBJECT_CLASS(cut_mach_o_loader_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutMachOLoaderPrivate *priv = CUT_MACH_O_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_SO_FILENAME:
        if (priv->so_filename)
            g_free(priv->so_filename);
        priv->so_filename = g_value_dup_string(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    CutMachOLoaderPrivate *priv = CUT_MACH_O_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_SO_FILENAME:
        g_value_set_string(value, priv->so_filename);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutMachOLoader *
cut_mach_o_loader_new (const gchar *so_filename)
{
    return g_object_new(CUT_TYPE_MACH_O_LOADER,
                        "so-filename", so_filename,
                        NULL);
}

gboolean
cut_mach_o_loader_is_mach_o (CutMachOLoader *loader)
{
#ifdef HAVE_MACH_O_LOADER_H
    CutMachOLoaderPrivate *priv;
    GError *error = NULL;
    uint32_t magic = 0;

    priv = CUT_MACH_O_LOADER_GET_PRIVATE(loader);
    if (!g_file_get_contents(priv->so_filename, &priv->content, &priv->length,
                             &error)) {
        g_warning("can't read shared library file: %s", error->message);
        g_error_free(error);
        return FALSE;
    }

    if (priv->length >= sizeof(magic))
        memcpy(&magic, priv->content, sizeof(magic));

    switch (magic) {
    case MH_MAGIC:
        priv->bit = ARCHITECTURE_32BIT;
        break;
    case MH_MAGIC_64:
        priv->bit = ARCHITECTURE_64BIT;
        g_warning("64bit mach-o isn't supported yet");
        g_free(priv->content);
        priv->content = NULL;
        break;
    default:
        g_warning("non mach-o magic: 0x%x", magic);
        g_free(priv->content);
        priv->content = NULL;
        break;
    }

    return priv->content != NULL;
#else
    return FALSE;
#endif
}

gboolean
cut_mach_o_loader_support_attribute (CutMachOLoader *loader)
{
#ifdef HAVE_MACH_O_LOADER_H
    return TRUE;
#else
    return FALSE;
#endif
}

#ifdef HAVE_MACH_O_LOADER_H
static GList *
cut_mach_o_loader_collect_symbols_32 (CutMachOLoaderPrivate *priv)
{
    gsize offset = 0;
    GPtrArray *all_symbols;
    GList *symbols = NULL;
    struct mach_header *header;
    gint i;

    header = (struct mach_header *)priv->content;
    offset += sizeof(*header);

    all_symbols = g_ptr_array_sized_new(header->ncmds);
    for (i = 0; i < header->ncmds; i++) {
        struct load_command *load;

        load = (struct load_command *)(priv->content + offset);
        switch (load->cmd) {
        case LC_SYMTAB:
        {
            struct symtab_command *table;
            struct nlist *symbol_info;
            gchar *string_table;
            gint j;

            table = (struct symtab_command *)(priv->content + offset);
            symbol_info = (struct nlist *)(priv->content + table->symoff);
            string_table = priv->content + table->stroff;
            for (j = 0; j < table->nsyms; j++) {
                uint8_t type;
                int32_t string_offset;
                gchar *name;

                type = symbol_info[j].n_type;
                string_offset = symbol_info[j].n_un.n_strx;
                name = string_table + string_offset;
                if ((string_offset == 0) ||
                    (name[0] == '\0') ||
                    (name[0] != '_')) {
                    g_ptr_array_add(all_symbols, NULL);
                } else {
                    g_ptr_array_add(all_symbols, name + 1);
                }
            }
            break;
        }
        case LC_DYSYMTAB:
        {
            struct dysymtab_command *table;
            gint j;

            table = (struct dysymtab_command *)(priv->content + offset);
            for (j = 0; j < table->nextdefsym; j++) {
                const gchar *name;

                name = g_ptr_array_index(all_symbols, table->iextdefsym + j);
                symbols = g_list_prepend(symbols, g_strdup(name));
            }
        }
        default:
            break;
        }
        offset += load->cmdsize;
    }
    g_ptr_array_free(all_symbols, TRUE);

    return symbols;
}

static GList *
cut_mach_o_loader_collect_symbols_64 (CutMachOLoaderPrivate *priv)
{
    return NULL;
}
#endif

GList *
cut_mach_o_loader_collect_symbols (CutMachOLoader *loader)
{
#ifdef HAVE_MACH_O_LOADER_H
    CutMachOLoaderPrivate *priv;
    GList *symbols = NULL;

    priv = CUT_MACH_O_LOADER_GET_PRIVATE(loader);
    switch (priv->bit) {
    case ARCHITECTURE_32BIT:
        symbols = cut_mach_o_loader_collect_symbols_32(priv);
        break;
    case ARCHITECTURE_64BIT:
        symbols = cut_mach_o_loader_collect_symbols_64(priv);
        break;
    default:
        break;
    }

    return symbols;
#else
    return NULL;
#endif
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
