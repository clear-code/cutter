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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <gmodule.h>

#include "gcut-data.h"
#include "gcut-hash-table.h"
#include "gcut-inspect.h"

#define GCUT_DATA_GET_PRIVATE(obj)                                     \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), GCUT_TYPE_DATA, GCutDataPrivate))

typedef union _FieldValue
{
    gpointer pointer;
    gint integer;
    guint unsigned_integer;
} FieldValue;

typedef struct _Field
{
    gchar *name;
    GType type;
    FieldValue value;
    GDestroyNotify free_function;
} Field;

typedef struct _GCutDataPrivate	GCutDataPrivate;
struct _GCutDataPrivate
{
    GHashTable *fields;
};

static Field *
field_new (const gchar *name, GType type)
{
    Field *field;

    field = g_slice_new0(Field);
    field->name = g_strdup(name);
    field->type = type;

    return field;
}

static void
field_free (Field *field)
{
    if (field->name)
        g_free(field->name);
    if (field->free_function && field->value.pointer)
        field->free_function(field->value.pointer);

    g_slice_free(Field, field);
}

static void
field_inspect (GString *string, gconstpointer data, gpointer user_data)
{
    const Field *field = data;

    switch (field->type) {
      case G_TYPE_STRING:
        gcut_string_inspect(string, field->value.pointer, user_data);
        break;
      default:
        g_string_append_printf(string,
                               "[unsupported type: <%s>]",
                               g_type_name(field->type));
        break;
    }
}

static gboolean
field_equal (gconstpointer data1, gconstpointer data2)
{
    const Field *field1 = data1;
    const Field *field2 = data2;
    gboolean result = FALSE;

    if (field1 == field2)
        return TRUE;

    if (field1 == NULL || field2 == NULL)
        return FALSE;

    if (field1->type != field2->type)
        return FALSE;

    switch (field1->type) {
      case G_TYPE_STRING:
        result = g_str_equal(field1->value.pointer, field2->value.pointer);
        break;
      default:
        g_warning("[unsupported type: <%s>]", g_type_name(field1->type));
        result = FALSE;
        break;
    }

    return result;
}

G_DEFINE_TYPE(GCutData, gcut_data, G_TYPE_OBJECT)

static void dispose        (GObject         *object);

static void
gcut_data_class_init (GCutDataClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    g_type_class_add_private(gobject_class, sizeof(GCutDataPrivate));
}

static void
gcut_data_init (GCutData *data)
{
    GCutDataPrivate *priv = GCUT_DATA_GET_PRIVATE(data);

    priv->fields = g_hash_table_new_full(g_str_hash, g_str_equal,
                                         g_free,
                                         (GDestroyNotify)field_free);
}

static void
dispose (GObject *object)
{
    GCutDataPrivate *priv;

    priv = GCUT_DATA_GET_PRIVATE(object);
    if (priv->fields) {
        g_hash_table_unref(priv->fields);
        priv->fields = NULL;
    }

    G_OBJECT_CLASS(gcut_data_parent_class)->dispose(object);
}

GQuark
gcut_data_error_quark (void)
{
    return g_quark_from_static_string("gcut-data-error-quark");
}

GCutData *
gcut_data_new (const gchar *first_field_name, ...)
{
    GCutData *data;
    va_list args;

    va_start(args, first_field_name);
    data = gcut_data_new_va_list(first_field_name, args);
    va_end(args);

    return data;
}

GCutData *
gcut_data_new_va_list (const gchar *first_field_name, va_list args)
{
    GCutDataPrivate *priv;
    GCutData *data;
    const gchar *name;

    data = g_object_new(GCUT_TYPE_DATA, NULL);
    priv = GCUT_DATA_GET_PRIVATE(data);
    name = first_field_name;
    while (name) {
        Field *field;

        field = field_new(name, va_arg(args, GType));
        switch (field->type) {
          case G_TYPE_STRING:
            field->value.pointer = g_strdup(va_arg(args, const gchar *));
            field->free_function = g_free;
            break;
          default:
            g_warning("unsupported type: <%s>", g_type_name(field->type));
            field_free(field);
            field = NULL;
            break;
        }
        if (!field)
            break;

        g_hash_table_insert(priv->fields, g_strdup(name), field);

        name = va_arg(args, const gchar *);
    }

    return data;
}

gchar *
gcut_data_inspect (GCutData *data)
{
    GCutDataPrivate *priv;
    GString *string;
    gchar *inspected_fields;

    priv = GCUT_DATA_GET_PRIVATE(data);
    string = g_string_new(NULL);
    g_string_append_printf(string, "#<GCutData:0x%p ", data);
    inspected_fields = gcut_hash_table_inspect(priv->fields,
                                               gcut_string_inspect,
                                               field_inspect,
                                               NULL);
    g_string_append(string, inspected_fields);
    g_free(inspected_fields);
    g_string_append(string, ">");

    return g_string_free(string, FALSE);
}

gboolean
gcut_data_equal (GCutData *data1, GCutData *data2)
{
    GCutDataPrivate *priv1;
    GCutDataPrivate *priv2;

    if (data1 == data2)
        return TRUE;

    if (data1 == NULL || data2 == NULL)
        return FALSE;

    priv1 = GCUT_DATA_GET_PRIVATE(data1);
    priv2 = GCUT_DATA_GET_PRIVATE(data2);
    return gcut_hash_table_equal(priv1->fields,
                                 priv2->fields,
                                 field_equal);
}

const gchar *
gcut_data_get_string_with_error (GCutData *data, const gchar *field_name,
                                 GError **error)
{
    GCutDataPrivate *priv;
    Field *field;

    priv = GCUT_DATA_GET_PRIVATE(data);
    field = g_hash_table_lookup(priv->fields, field_name);
    if (!field) {
        g_set_error(error,
                    GCUT_DATA_ERROR,
                    GCUT_DATA_ERROR_NOT_EXIST,
                    "requested field doesn't exist: <%s>", field_name);
        return NULL;
    }

    return field->value.pointer;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
