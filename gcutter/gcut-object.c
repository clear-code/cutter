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
#  include "../cutter/config.h"
#endif /* HAVE_CONFIG_H */

#include <glib-object.h>

#include "gcut-object.h"
#include "gcut-enum.h"
#include "gcut-test-utils.h"

gboolean
gcut_object_equal (const GObject *object1, const GObject *object2,
                   GEqualFunc equal_func)
{
    if (object1 == object2)
        return TRUE;

    if (object1 == NULL || object2 == NULL)
        return FALSE;

    if (!equal_func)
        return FALSE;

    return equal_func(object1, object2);
}


static void
inspect_object (GString *string, gconstpointer const_object, gpointer data)
{
    GObject *object = (GObject *)const_object;
    guint i, n_properties = 0;
    GParamSpec **param_specs = NULL;

    g_string_append_printf(string, "#<%s:%p",
                           G_OBJECT_TYPE_NAME(object),
                           object);
    param_specs = g_object_class_list_properties(G_OBJECT_GET_CLASS(object),
                                                 &n_properties);
    for (i = 0; i < n_properties; i++) {
        GParamSpec *spec = param_specs[i];
        GValue value = {0,};
        gchar *value_string;

        if (i > 0)
            g_string_append(string, ",");

        g_value_init(&value, spec->value_type);
        g_object_get_property(object, spec->name, &value);
        if (G_TYPE_IS_ENUM(spec->value_type)) {
            value_string = gcut_enum_inspect(spec->value_type,
                                             g_value_get_enum(&value));
        } else if (G_TYPE_IS_FLAGS(spec->value_type)) {
            value_string = gcut_flags_inspect(spec->value_type,
                                              g_value_get_flags(&value));
        } else if (G_TYPE_IS_OBJECT(spec->value_type)) {
            /* FIXME: circular check */
            value_string = gcut_object_inspect(g_value_get_object(&value));
        } else {
            value_string = g_strdup_value_contents(&value);
        }
        g_string_append_printf(string, " %s=<%s>", spec->name, value_string);
        g_free(value_string);
        g_value_unset(&value);
    }
    g_free(param_specs);
    g_string_append(string, ">");
}

gchar *
gcut_object_inspect (const GObject *object)
{
    return gcut_object_inspect_custom(object, inspect_object, NULL);
}

gchar *
gcut_object_inspect_custom (const GObject *object, GCutInspectFunc inspect_func,
                            gpointer user_data)
{
    GString *string;

    if (!object)
        return g_strdup("(null)");

    string = g_string_new(NULL);
    inspect_func(string, object, user_data);
    return g_string_free(string, FALSE);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/

