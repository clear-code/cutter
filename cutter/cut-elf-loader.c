/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2013  Kouhei Sutou <kou@cozmixng.org>
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

#ifdef HAVE_ELF_H
#  include <elf.h>
#endif
#include <glib/gstdio.h>

#include "cut-elf-loader.h"
#include "cut-logger.h"

#define CUT_ELF_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), CUT_TYPE_ELF_LOADER, CutELFLoaderPrivate))

typedef enum {
    ARCHITECTURE_UNKNOWN,
    ARCHITECTURE_32BIT,
    ARCHITECTURE_64BIT
} ArchitectureBit;

typedef struct _CutELFLoaderPrivate	CutELFLoaderPrivate;
struct _CutELFLoaderPrivate
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

G_DEFINE_TYPE(CutELFLoader, cut_elf_loader, G_TYPE_OBJECT)

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
cut_elf_loader_class_init (CutELFLoaderClass *klass)
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

    g_type_class_add_private(gobject_class, sizeof(CutELFLoaderPrivate));
}

static void
cut_elf_loader_init (CutELFLoader *loader)
{
    CutELFLoaderPrivate *priv = CUT_ELF_LOADER_GET_PRIVATE(loader);

    priv->so_filename = NULL;
    priv->content = NULL;
    priv->length = 0;
    priv->bit = ARCHITECTURE_UNKNOWN;
}

static void
dispose (GObject *object)
{
    CutELFLoaderPrivate *priv = CUT_ELF_LOADER_GET_PRIVATE(object);

    if (priv->so_filename) {
        g_free(priv->so_filename);
        priv->so_filename = NULL;
    }

    if (priv->content) {
        g_free(priv->content);
        priv->content = NULL;
    }

    G_OBJECT_CLASS(cut_elf_loader_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutELFLoaderPrivate *priv = CUT_ELF_LOADER_GET_PRIVATE(object);

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
    CutELFLoaderPrivate *priv = CUT_ELF_LOADER_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_SO_FILENAME:
        g_value_set_string(value, priv->so_filename);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutELFLoader *
cut_elf_loader_new (const gchar *so_filename)
{
    return g_object_new(CUT_TYPE_ELF_LOADER,
                        "so-filename", so_filename,
                        NULL);
}

gboolean
cut_elf_loader_is_elf (CutELFLoader *loader)
{
#ifdef HAVE_ELF_H
    CutELFLoaderPrivate *priv;
    GError *error = NULL;
    unsigned char ident[EI_NIDENT];

    priv = CUT_ELF_LOADER_GET_PRIVATE(loader);
    cut_log_trace("[loader][elf][open] <%s>", priv->so_filename);
    if (!g_file_get_contents(priv->so_filename, &priv->content, &priv->length,
                             &error)) {
        g_warning("can't read shared library file: %s", error->message);
        g_error_free(error);
        return FALSE;
    }

    if (priv->length >= sizeof(ident))
        memcpy(ident, priv->content, sizeof(ident));

    if ((ident[EI_MAG0] == ELFMAG0) &&
        (ident[EI_MAG1] == ELFMAG1) &&
        (ident[EI_MAG2] == ELFMAG2) &&
        (ident[EI_MAG3] == ELFMAG3)) {
        switch (ident[EI_CLASS]) {
        case ELFCLASS32:
            cut_log_trace("[loader][elf][bit] <32>");
            priv->bit = ARCHITECTURE_32BIT;
            break;
        case ELFCLASS64:
            cut_log_trace("[loader][elf][bit] <64>");
            priv->bit = ARCHITECTURE_64BIT;
            break;
        default:
            g_warning("unknown ELF class: 0x%x", ident[EI_CLASS]);
            g_free(priv->content);
            priv->content = NULL;
            break;
        }
    } else {
        cut_log_trace("[loader][elf][header][unknown]");
        g_free(priv->content);
        priv->content = NULL;
    }

    if (priv->content) {
        uint16_t type;

        memcpy(&type, priv->content + sizeof(ident), sizeof(type));
        cut_log_trace("[loader][elf][type] <%#x>", type);
        if (type != ET_DYN) {
            g_warning("not dynamic library: %#x", type);
            g_free(priv->content);
            priv->content = NULL;
        }
    }

    return priv->content != NULL;
#else
    return FALSE;
#endif
}

gboolean
cut_elf_loader_support_attribute (CutELFLoader *loader)
{
#ifdef HAVE_ELF_H
    return TRUE;
#else
    return FALSE;
#endif
}

#ifdef HAVE_ELF_H
static gboolean
collect_symbol_information (CutELFLoaderPrivate *priv,
                            gsize *symbol_section_offset,
                            gsize *symbol_entry_size,
                            guint *n_entries,
                            gsize *name_section_offset,
                            uint16_t *text_section_header_index)
{
    gboolean collected = FALSE;
    gsize section_offset = 0;
    Elf32_Ehdr *header_32 = NULL;
    Elf64_Ehdr *header_64 = NULL;
    uint16_t i, n_headers;
    uint16_t section_header_size;
    const gchar *section_names;
    Elf32_Shdr *dynstr_32 = NULL;
    Elf32_Shdr *dynsym_32 = NULL;
    Elf64_Shdr *dynstr_64 = NULL;
    Elf64_Shdr *dynsym_64 = NULL;
    uint16_t _text_section_header_index = 0;

    if (priv->bit == ARCHITECTURE_32BIT) {
        Elf32_Shdr *section_name_header;
        gsize section_name_header_offset;

        header_32 = (Elf32_Ehdr *)priv->content;
        section_offset = header_32->e_shoff;
        section_header_size = header_32->e_shentsize;
        n_headers = header_32->e_shnum;

        section_name_header_offset =
            header_32->e_shoff +
            (header_32->e_shstrndx * header_32->e_shentsize);
        section_name_header =
            (Elf32_Shdr *)(priv->content + section_name_header_offset);
        section_names = priv->content + section_name_header->sh_offset;
    } else {
        Elf64_Shdr *section_name_header;
        gsize section_name_header_offset;

        header_64 = (Elf64_Ehdr *)priv->content;
        section_offset = header_64->e_shoff;
        section_header_size = header_64->e_shentsize;
        n_headers = header_64->e_shnum;

        section_name_header_offset =
            header_64->e_shoff +
            (header_64->e_shstrndx * header_64->e_shentsize);
        section_name_header =
            (Elf64_Shdr *)(priv->content + section_name_header_offset);
        section_names = priv->content + section_name_header->sh_offset;
    }

    cut_log_trace("[loader][elf][collect-symbols][n-header] <%d>", n_headers);
    for (i = 0; i < n_headers; i++) {
        Elf32_Shdr *section_header_32 = NULL;
        Elf64_Shdr *section_header_64 = NULL;
        gsize offset;
        const gchar *section_name;

        offset = section_offset + (section_header_size * i);
        if (priv->bit == ARCHITECTURE_32BIT) {
            section_header_32 = (Elf32_Shdr *)(priv->content + offset);
            section_name = section_names + section_header_32->sh_name;
        } else {
            section_header_64 = (Elf64_Shdr *)(priv->content + offset);
            section_name = section_names + section_header_64->sh_name;
        }

        if (g_str_equal(section_name, ".dynstr")) {
            if (priv->bit == ARCHITECTURE_32BIT) {
                dynstr_32 = section_header_32;
            } else {
                dynstr_64 = section_header_64;
            }
        } else if (g_str_equal(section_name, ".dynsym")) {
            if (priv->bit == ARCHITECTURE_32BIT) {
                dynsym_32 = section_header_32;
            } else {
                dynsym_64 = section_header_64;
            }
        } else if (g_str_equal(section_name, ".text")) {
            _text_section_header_index = i;
        }
    }

    if (priv->bit == ARCHITECTURE_32BIT) {
        if (dynsym_32 && dynstr_32 && _text_section_header_index > 0) {
            collected = TRUE;
            *symbol_section_offset = dynsym_32->sh_offset;
            *symbol_entry_size = dynsym_32->sh_entsize;
            if (*symbol_entry_size > 0)
                *n_entries = dynsym_32->sh_size / *symbol_entry_size;
            else
                *n_entries = 0;
            *name_section_offset = dynstr_32->sh_offset;
            *text_section_header_index = _text_section_header_index;
        }
    } else {
        if (dynsym_64 && dynstr_64 && _text_section_header_index > 0) {
            collected = TRUE;
            *symbol_section_offset = dynsym_64->sh_offset;
            *symbol_entry_size = dynsym_64->sh_entsize;
            if (*symbol_entry_size > 0)
                *n_entries = dynsym_64->sh_size / *symbol_entry_size;
            else
                *n_entries = 0;
            *name_section_offset = dynstr_64->sh_offset;
            *text_section_header_index = _text_section_header_index;
        }
    }

    return collected;
}

static const gchar *
inspect_type (unsigned char info)
{
    switch (info) {
    case STT_NOTYPE:
        return "no type";
    case STT_OBJECT:
        return "object";
    case STT_FUNC:
        return "function";
    case STT_SECTION:
        return "section";
    case STT_FILE:
        return "file";
    default:
        return "unknown";
    }
}

static const gchar *
inspect_bind (unsigned char bind)
{
    switch (bind) {
    case STB_LOCAL:
        return "local";
    case STB_GLOBAL:
        return "global";
    case STB_WEAK:
        return "weak";
    default:
        return "unknown";
    }
}
#endif

GList *
cut_elf_loader_collect_symbols (CutELFLoader *loader)
{
#ifdef HAVE_ELF_H
    CutELFLoaderPrivate *priv;
    GList *symbols = NULL;
    gsize symbol_section_offset;
    gsize symbol_entry_size;
    guint i, n_entries;
    gsize name_section_offset;
    uint16_t text_section_header_index;

    priv = CUT_ELF_LOADER_GET_PRIVATE(loader);
    if (!collect_symbol_information(priv,
                                    &symbol_section_offset,
                                    &symbol_entry_size,
                                    &n_entries,
                                    &name_section_offset,
                                    &text_section_header_index))
        return NULL;

    cut_log_trace("[loader][elf][collect-symbols][n-symbols] <%d>", n_entries);
    for (i = 0; i < n_entries; i++) {
        Elf32_Sym *symbol_32;
        Elf64_Sym *symbol_64;
        uint32_t name_index;
        unsigned char info, type, bind;
        uint16_t section_header_index;
        gsize offset;
        const gchar *name;

        offset = symbol_section_offset + (i * symbol_entry_size);
        if (priv->bit == ARCHITECTURE_32BIT) {
            symbol_32 = (Elf32_Sym *)(priv->content + offset);
            name_index = symbol_32->st_name;
            info = symbol_32->st_info;
            type = ELF32_ST_TYPE(info);
            bind = ELF32_ST_BIND(info);
            section_header_index = symbol_32->st_shndx;
        } else {
            symbol_64 = (Elf64_Sym *)(priv->content + offset);
            name_index = symbol_64->st_name;
            info = symbol_64->st_info;
            type = ELF64_ST_TYPE(info);
            bind = ELF64_ST_BIND(info);
            section_header_index = symbol_64->st_shndx;
        }

        name = priv->content + name_section_offset + name_index;
        cut_log_trace("[loader][elf][collect-symbols][symbol] <%s>:<%s>:<%s>",
                      name_index > 0 ? name : "null",
                      inspect_type(type),
                      inspect_bind(bind));
        if ((type == STT_FUNC) &&
            (bind == STB_GLOBAL) &&
            (section_header_index == text_section_header_index)) {
            cut_log_trace("[loader][elf][collect-symbols][symbol][collect] <%s>",
                          name);
            symbols = g_list_prepend(symbols, g_strdup(name));
        }
    }

    return symbols;
#else
    return NULL;
#endif
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
