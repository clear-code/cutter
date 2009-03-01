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
 * GCUT_ENUM_ERROR:
 *
 * Error domain for enum related operations. Errors in this domain
 * will be from the #GCutEnumError enumeration.
 *
 * Since: 1.0.6
 */
#define GCUT_ENUM_ERROR (gcut_enum_error_quark())

GQuark           gcut_enum_error_quark          (void);

/**
 * GCutEnumError:
 * @GCUT_ENUM_ERROR_INVALID_TYPE: Invalid type.
 * @GCUT_ENUM_ERROR_INVALID_VALUE: Invalid value.
 *
 * Error codes returned by enum related operation.
 *
 * Since: 1.0.6
 */
typedef enum
{
    GCUT_ENUM_ERROR_INVALID_TYPE,
    GCUT_ENUM_ERROR_INVALID_VALUE
} GCutEnumError;

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
 * gcut_enum_parse:
 * @enum_type: a #GEnum type.
 * @enum_value: a string to be parsed.
 * @error: return location for an error, or %NULL
 *
 * Parses @enum_value and returns a enum value of
 * @enum_type. @enum_value should be enum name or nick
 * name. If @enum_value isn't match then @error is set to a
 * #GCutEnumError.
 *
 * Returns: enum value of @enum_type corresponded to @enum_value.
 *
 * Since: 1.0.6
 */
gint             gcut_enum_parse                (GType         enum_type,
                                                 const gchar  *enum_value,
                                                 GError      **error);

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
                                                 guint flags_value);

/**
 * gcut_flags_parse:
 * @flags_type: a #GFlags type.
 * @flags_value: a string to be parsed.
 * @error: return location for an error, or %NULL
 *
 * Parses @flags_value and returns a flags value of
 * @flags_type. @flags_value should be formated as
 * "nick-or-name1|nick-or-name2|...|nick-or-nameN". If
 * @flags_value includes unknown flag then @error is set to
 * a #GCutEnumError.
 *
 * Returns: flags value of @flags_type corresponded to @flags_value.
 *
 * Since: 1.0.6
 */
guint            gcut_flags_parse               (GType         flags_type,
                                                 const gchar  *flags_value,
                                                 GError      **error);

/**
 * gcut_flags_get_all:
 * @flags_type: a #GFlags type.
 * @error: return location for an error, or %NULL
 *
 * Gets a flags value that includes all available flag
 * values.
 *
 * Returns: a flags value that includes all available flag
 * values.
 *
 * Since: 1.0.6
 */
guint            gcut_flags_get_all             (GType         flags_type,
                                                 GError      **error);

G_END_DECLS

#endif /* __GCUT_ENUM_H__ */

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
