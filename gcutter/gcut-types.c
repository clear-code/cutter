/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2010  Kouhei Sutou <kou@clear-code.com>
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

#include <glib.h>
#include <gobject/gvaluecollector.h>

#include "gcut-types.h"

GType
gcut_error_get_type (void)
{
    static GType type_id = 0;
    if (!type_id)
        type_id = g_boxed_type_register_static(g_intern_static_string("GError"),
                                               (GBoxedCopyFunc)g_error_copy,
                                               (GBoxedFreeFunc)g_error_free);
    return type_id;
}

static void
value_init_size (GValue *value)
{
#if GLIB_SIZEOF_LONG == GLIB_SIZEOF_SIZE_T
    value->data[0].v_ulong = 0;
#else
    value->data[0].v_uint64 = 0;
#endif
}

static void
value_copy_size (const GValue *src_value,
                 GValue       *dest_value)
{
#if GLIB_SIZEOF_LONG == GLIB_SIZEOF_SIZE_T
    dest_value->data[0].v_ulong = src_value->data[0].v_ulong;
#else
    dest_value->data[0].v_uint64 = src_value->data[0].v_uint64;
#endif
}

static gchar*
value_collect_size (GValue      *value,
                    guint        n_collect_values,
                    GTypeCValue *collect_values,
                    guint        collect_flags)
{
#if GLIB_SIZEOF_LONG == GLIB_SIZEOF_SIZE_T
    value->data[0].v_long = (collect_values[0]).v_long;
#else
    value->data[0].v_int64 = collect_values[0].v_int64;
#endif

  return NULL;
}

static gchar*
value_lcopy_size (const GValue *value,
                  guint         n_collect_values,
                  GTypeCValue  *collect_values,
                  guint         collect_flags)
{
    gsize *size_p = collect_values[0].v_pointer;

    if (!size_p) {
        return g_strdup_printf("value location for `%s' passed as NULL",
                               G_VALUE_TYPE_NAME(value));
    }

#if GLIB_SIZEOF_LONG == GLIB_SIZEOF_SIZE_T
    *size_p = value->data[0].v_ulong;
#else
    *size_p = value->data[0].v_uint64;
#endif

    return NULL;
}

GType
gcut_size_get_type (void)
{
    static GType type_id = 0;

    if (!type_id) {
        GTypeInfo info = {
            0,				/* class_size */
            NULL,			/* base_init */
            NULL,			/* base_destroy */
            NULL,			/* class_init */
            NULL,			/* class_destroy */
            NULL,			/* class_data */
            0,				/* instance_size */
            0,				/* n_preallocs */
            NULL,			/* instance_init */
            NULL,			/* value_table */
        };
        const GTypeFundamentalInfo finfo = { G_TYPE_FLAG_DERIVABLE, };
        static const GTypeValueTable value_table = {
            value_init_size,		/* value_init */
            NULL,			/* value_free */
            value_copy_size,		/* value_copy */
            NULL,			/* value_peek_pointer */
            "l",			/* collect_format */
            value_collect_size,		/* collect_value */
            "p",			/* lcopy_format */
            value_lcopy_size,		/* lcopy_value */
        };
        info.value_table = &value_table;
        type_id = g_type_register_fundamental(g_type_fundamental_next(),
                                              g_intern_static_string("gsize"),
                                              &info,
                                              &finfo,
                                              0);
    }

    return type_id;
}

/*
vi:nowrap:ai:expandtab:sw=4
*/

