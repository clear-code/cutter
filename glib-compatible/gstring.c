/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

#include <string.h>

#include <glib.h>
#include "gstring.h"

#define MY_MAXSIZE ((gsize)-1)

static inline gsize
nearest_power (gsize base, gsize num)    
{
  if (num > MY_MAXSIZE / 2)
    {
      return MY_MAXSIZE;
    }
  else
    {
      gsize n = base;

      while (n < num)
	n <<= 1;
      
      return n;
    }
}

static void
g_string_maybe_expand (GString* string,
		       gsize    len) 
{
  if (string->len + len >= string->allocated_len)
    {
      string->allocated_len = nearest_power (1, string->len + len + 1);
      string->str = g_realloc (string->str, string->allocated_len);
    }
}

/**
 * g_string_overwrite:
 * @string: a #GString
 * @pos: the position at which to start overwriting
 * @val: the string that will overwrite the @string starting at @pos
 * 
 * Overwrites part of a string, lengthening it if necessary.
 * 
 * Return value: @string
 *
 * Since: 2.14
 **/
GString *
g_string_overwrite (GString     *string,
		    gsize        pos,
		    const gchar *val)
{
  g_return_val_if_fail (val != NULL, string);
  return g_string_overwrite_len (string, pos, val, strlen (val));
}

/**
 * g_string_overwrite_len:
 * @string: a #GString
 * @pos: the position at which to start overwriting
 * @val: the string that will overwrite the @string starting at @pos
 * @len: the number of bytes to write from @val
 * 
 * Overwrites part of a string, lengthening it if necessary. 
 * This function will work with embedded nuls.
 * 
 * Return value: @string
 *
 * Since: 2.14
 **/
GString *
g_string_overwrite_len (GString     *string,
			gsize        pos,
			const gchar *val,
			gssize       len)
{
  gsize end;

  g_return_val_if_fail (string != NULL, NULL);

  if (!len)
    return string;

  g_return_val_if_fail (val != NULL, string);
  g_return_val_if_fail (pos <= string->len, string);

  if (len < 0)
    len = strlen (val);

  end = pos + len;

  if (end > string->len)
    g_string_maybe_expand (string, end - string->len);

  memcpy (string->str + pos, val, len);

  if (end > string->len)
    {
      string->str[end] = '\0';
      string->len = end;
    }

  return string;
}
