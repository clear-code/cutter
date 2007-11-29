/* -*- c-file-style: "gnu" -*- */
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

#include "cutter.h"

#include <string.h>
#include <glib.h>

void test_length (void);
void test_utf8 (void);
void test_misc (void);

gchar *longline = "asdasdas dsaf asfd as fdasdf asfd asdf as dfas dfasdf a"
"asd fasdf asdf asdf asd fasfd as fdasfd asdf as fdççççççççças ffsd asfd as fdASASASAs As"
"Asfdsf sdfg sdfg dsfg dfg sdfgsdfgsdfgsdfg sdfgsdfg sdfg sdfg sdf gsdfg sdfg sd"
"asd fasdf asdf asdf asd fasfd as fdaèèèèèèè òòòòòòòòòòòòsfd asdf as fdas ffsd asfd as fdASASASAs D"
"Asfdsf sdfg sdfg dsfg dfg sdfgsdfgsdfgsdfg sdfgsdfg sdfgùùùùùùùùùùùùùù sdfg sdf gsdfg sdfg sd"
"asd fasdf asdf asdf asd fasfd as fdasfd asd@@@@@@@f as fdas ffsd asfd as fdASASASAs D "
"Asfdsf sdfg sdfg dsfg dfg sdfgsdfgsdfgsdfg sdfgsdf€€€€€€€€€€€€€€€€€€g sdfg sdfg sdf gsdfg sdfg sd"
"asd fasdf asdf asdf asd fasfd as fdasfd asdf as fdas ffsd asfd as fdASASASAs D"
"Asfdsf sdfg sdfg dsfg dfg sdfgsdfgsdfgsdfg sdfgsdfg sdfg sdfg sdf gsdfg sdfg sd\n\nlalala\n";

void
test_utf8 (void)
{
  gint num_chars;
  gchar **p;
  gint i, j;
  
  cut_assert (g_utf8_validate (longline, -1, NULL));
  
  num_chars = g_utf8_strlen (longline, -1);
  
  p = (gchar **) g_malloc (num_chars * sizeof (gchar *));
  
  p[0] = longline;
  for (i = 1; i < num_chars; i++)
    p[i] = g_utf8_next_char (p[i-1]);
  
  for (i = 0; i < num_chars; i++)
    for (j = 0; j < num_chars; j++) 
      {
	cut_assert_equal_int (p[j], g_utf8_offset_to_pointer (p[i], j - i));
	cut_assert_equal_int (j - i, g_utf8_pointer_to_offset (p[i], p[j]));
      }
  
  g_free (p);
}

void
test_length (void)
{
  cut_assert_equal_int (4, g_utf8_strlen ("1234", -1));
  cut_assert_equal_int (0, g_utf8_strlen ("1234", 0));
  cut_assert_equal_int (1, g_utf8_strlen ("1234", 1));
  cut_assert_equal_int (2, g_utf8_strlen ("1234", 2));
  cut_assert_equal_int (3, g_utf8_strlen ("1234", 3));
  cut_assert_equal_int (4, g_utf8_strlen ("1234", 4));
  cut_assert_equal_int (4, g_utf8_strlen ("1234", 5));

  cut_assert_equal_int (762, g_utf8_strlen (longline, -1));
  cut_assert_equal_int (762, g_utf8_strlen (longline, strlen (longline)));
  cut_assert_equal_int (762, g_utf8_strlen (longline, 1024));

  cut_assert_equal_int (0, g_utf8_strlen (NULL, 0));

  cut_assert_equal_int (3, g_utf8_strlen ("a\340\250\201c", -1));
  cut_assert_equal_int (1, g_utf8_strlen ("a\340\250\201c", 1));
  cut_assert_equal_int (1, g_utf8_strlen ("a\340\250\201c", 2));
  cut_assert_equal_int (1, g_utf8_strlen ("a\340\250\201c", 3));
  cut_assert_equal_int (2, g_utf8_strlen ("a\340\250\201c", 4));
  cut_assert_equal_int (3, g_utf8_strlen ("a\340\250\201c", 5));
}

void
test_misc (void)
{
  char *s;
  s = g_utf8_strreverse ("1234", -1);
  cut_assert_equal_string ("4321", s);
  g_free (s);
  s = g_utf8_strreverse ("1234", 3);
  cut_assert_equal_string ("321", s);
  g_free (s);
}

