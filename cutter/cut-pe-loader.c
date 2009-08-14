/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@cozmixng.org>
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

#ifdef HAVE_WINDOWS_H
#  include <windows.h>
#endif
#include <glib/gstdio.h>

#include "cut-pe-loader.h"

#define CUT_PE_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_PE_LOADER, CutPELoaderPrivate))

typedef struct _CutPELoaderPrivate	CutPELoaderPrivate;
struct _CutPELoaderPrivate
{
    gchar *so_filename;
    gchar *content;
    gsize length;
#ifdef HAVE_WINDOWS_H
    IMAGE_NT_HEADERS *nt_headers;
#endif
};

enum
{
    PROP_0,
    PROP_SO_FILENAME
};

G_DEFINE_TYPE(CutPELoader, cut_pe_loader, G_TYPE_OBJECT)

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
cut_pe_loader_class_init (CutPELoaderClass *klass)
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

    g_type_class_add_private(gobject_class, sizeof(CutPELoaderPrivate));
}

static void
cut_pe_loader_init (CutPELoader *loader)
{
    CutPELoaderPrivate *priv = CUT_PE_LOADER_GET_PRIVATE(loader);

    priv->so_filename = NULL;
    priv->content = NULL;
    priv->length = 0;
#ifdef HAVE_WINDOWS_H
    priv->nt_headers = NULL;
#endif
}

static void
dispose (GObject *object)
{
    CutPELoaderPrivate *priv = CUT_PE_LOADER_GET_PRIVATE(object);

    if (priv->so_filename) {
        g_free(priv->so_filename);
        priv->so_filename = NULL;
    }

    if (priv->content) {
        g_free(priv->content);
        priv->content = NULL;
    }

    G_OBJECT_CLASS(cut_pe_loader_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutPELoaderPrivate *priv = CUT_PE_LOADER_GET_PRIVATE(object);

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
    CutPELoaderPrivate *priv = CUT_PE_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_SO_FILENAME:
        g_value_set_string(value, priv->so_filename);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutPELoader *
cut_pe_loader_new (const gchar *so_filename)
{
    return g_object_new(CUT_TYPE_PE_LOADER,
                        "so-filename", so_filename,
                        NULL);
}

gboolean
cut_pe_loader_is_dll (CutPELoader *loader)
{
#ifdef HAVE_WINDOWS_H
    CutPELoaderPrivate *priv;
    GError *error = NULL;
    IMAGE_DOS_HEADER *dos_header;

    priv = CUT_PE_LOADER_GET_PRIVATE(loader);
    if (!g_file_get_contents(priv->so_filename, &priv->content, &priv->length,
                             &error)) {
        g_warning("can't read shared library file: %s", error->message);
        g_error_free(error);
        return FALSE;
    }

    if (priv->length < sizeof(*dos_header))
        return FALSE;

    dos_header = (IMAGE_DOS_HEADER *)priv->content;
    if (priv->length < dos_header->e_lfanew)
        return FALSE;

    priv->nt_headers =
        (IMAGE_NT_HEADERS *)(priv->content + dos_header->e_lfanew);
    if (priv->nt_headers->Signature != IMAGE_NT_SIGNATURE)
        return FALSE;

    return priv->nt_headers->FileHeader.Characteristics & IMAGE_FILE_DLL;
#else
    return FALSE;
#endif
}

gboolean
cut_pe_loader_support_attribute (CutPELoader *loader)
{
#ifdef HAVE_WINDOWS_H
    return TRUE;
#else
    return FALSE;
#endif
}

GList *
cut_pe_loader_collect_symbols (CutPELoader *loader)
{
#ifdef HAVE_WINDOWS_H
    CutPELoaderPrivate *priv;
    GList *symbols = NULL;
    WORD i;
    IMAGE_SECTION_HEADER *first_section;
    IMAGE_SECTION_HEADER *text_section = NULL;
    IMAGE_SECTION_HEADER *edata_section = NULL;
    IMAGE_EXPORT_DIRECTORY *directory;
    const gchar *base_address;
    ULONG *name_addresses;
    ULONG *function_addresses;

    priv = CUT_PE_LOADER_GET_PRIVATE(loader);
    first_section = IMAGE_FIRST_SECTION(priv->nt_headers);
    for (i = 0; i < priv->nt_headers->FileHeader.NumberOfSections; i++) {
        const gchar *section_name;

        section_name = (const gchar *)((first_section + i)->Name);
        if (g_str_equal(".text", section_name)) {
            text_section = first_section + i;
        } else if (g_str_equal(".edata", section_name)) {
            edata_section = first_section + i;
        }
    }

    if (!text_section)
        return NULL;
    if (!edata_section)
        return NULL;

    directory = (IMAGE_EXPORT_DIRECTORY *)(priv->content +
                                           edata_section->PointerToRawData);
    base_address =
        priv->content +
        edata_section->PointerToRawData -
        edata_section->VirtualAddress;
    name_addresses = (ULONG *)(base_address + directory->AddressOfNames);
    function_addresses =
        (ULONG *)(base_address + export_directory->AddressOfFunctions);
    min_text_section_address = text_section->VirtualAddress;
    max_text_section_address =
        min_text_section_address + text_section->SizeOfRawData;
    for (i = 0; i < directory->NumberOfNames; i++) {
        const gchar *name;
        DWORD function_address;

        name = base_address + name_addresses[i];
        function_address = function_addresses[i];
        if (min_text_section_address < function_address &&
            function_address < max_text_section_address)
            symbols = g_list_prepend(symbols, g_strdup(name));
    }

    return symbols;
#else
    return NULL;
#endif
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
