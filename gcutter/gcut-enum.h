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

#ifndef __GCUT_ENUM_H__
#define __GCUT_ENUM_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SECTION: gcut-enum
 * @title: Assertion Utilities for GEnum and GFlags
 * @short_description: Utilities to write assertions related
 * to #GEnum or #GFlags more easily.
 *
 * To write assertions, you need to check equality and show
 * expected and actual values.
 *
 * The utilities help you to write assertions that are
 * related to #GEnum or #GFlags.
 */

/**
 * gcut_enum_equal:
 * @enum_type: a #GEnum type.
 * @enum1: a #GEnum value to be compared.
 * @enum2: a #GEnum value to be compared.
 *
 * Compares two @enum_type values, @enum1 and @enum2.
 *
 * Returns: TRUE if @enum1 == @enum2, FALSE otherwise.
 *
 * Since: 1.0.5
 */
gboolean         gcut_enum_equal                (GType enum_type,
                                                 gint  enum1,
                                                 gint  enum2);

/**
 * gcut_enum_inspect:
 * @enum_type: a #GEnum type.
 * @enum_value: a #GEnum value to be inspected.
 *
 * Inspects @enum_type value, @enum_value. The returned
 * string should be freed when no longer needed.
 *
 * Returns: inspected @enum_value as a string.
 *
 * Since: 1.0.5
 */
gchar           *gcut_enum_inspect              (GType enum_type,
                                                 gint  enum_value);

/**
 * gcut_flags_equal:
 * @flags_type: a #GFlags type.
 * @flags1: a #GFlags value to be compared.
 * @flags2: a #GFlags value to be compared.
 *
 * Compares two @flags_type values, @flags1 and @flags2.
 *
 * Returns: TRUE if @flags1 == @flags2, FALSE otherwise.
 *
 * Since: 1.0.5
 */
gboolean         gcut_flags_equal               (GType flags_type,
                                                 guint flags1,
                                                 guint flags2);

/**
 * gcut_flags_inspect:
 * @flags_type: a #GFlags type.
 * @flags_value: a #GFlags value to be inspected.
 *
 * Inspects @flags_type value, @flags_value. The returned
 * string should be freed when no longer needed.
 *
 * Returns: inspected @flags_value as a string.
 *
 * Since: 1.0.5
 */
gchar           *gcut_flags_inspect             (GType flags_type,
                                                 guint flags);


G_END_DECLS

#endif /* __GCUT_ENUM_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
