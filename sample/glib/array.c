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

#include <cutter.h>

#include <stdio.h>
#include <string.h>
#include <glib.h>

void test_array (void);
void test_ptr_array (void);
void test_byte_array (void);

static GArray *garray;
static GPtrArray *gparray;
static GByteArray *gbarray;

void
setup (void)
{
}

static void
free_array (void)
{
  if (garray) {
    g_array_free (garray, TRUE);
    garray = NULL;
  }
}

static void
free_ptr_array (void)
{
  if (gparray) {
    g_ptr_array_free (gparray, TRUE);
    gparray = NULL;
  }
}

static void
free_byte_array (void)
{
  if (gbarray) {
    g_byte_array_free (gbarray, TRUE);
    gbarray = NULL;
  }
}

void
teardown (void)
{
  free_array();
  free_ptr_array();
  free_byte_array();
}

static void 
sum_up (gpointer data, 
	gpointer user_data)
{
  gint *sum = (gint *)user_data;

  *sum += GPOINTER_TO_INT (data);
}

void
test_array (void)
{
  gint i;

  /* array tests */
  garray = g_array_new (FALSE, FALSE, sizeof (gint));
  for (i = 0; i < 10000; i++)
    g_array_append_val (garray, i);

  for (i = 0; i < 10000; i++)
    cut_assert_equal_int (i, g_array_index (garray, gint, i));

  free_array();

  garray = g_array_new (FALSE, FALSE, sizeof (gint));
  for (i = 0; i < 100; i++)
    g_array_prepend_val (garray, i);

  for (i = 0; i < 100; i++)
    cut_assert_equal_int ((100 - i - 1), g_array_index (garray, gint, i));

  free_array();
}


void 
test_ptr_array (void)
{
  gint i;
  gint sum = 0;

  /* pointer arrays */
  gparray = g_ptr_array_new ();
  for (i = 0; i < 10000; i++)
    g_ptr_array_add (gparray, GINT_TO_POINTER (i));

  for (i = 0; i < 10000; i++)
    cut_assert_equal_pointer (GINT_TO_POINTER (i),
                              g_ptr_array_index (gparray, i));
  
  g_ptr_array_foreach (gparray, sum_up, &sum);
  cut_assert_equal_int (49995000, sum);

  free_ptr_array ();
}

void
test_byte_array (void)
{
  gint i;

  /* byte arrays */
  gbarray = g_byte_array_new ();
  for (i = 0; i < 10000; i++)
    g_byte_array_append (gbarray, (guint8*) "abcd", 4);

  for (i = 0; i < 10000; i++)
    {
      cut_assert_equal_int ('a', gbarray->data[4*i]);
      cut_assert_equal_int ('b', gbarray->data[4*i+1]);
      cut_assert_equal_int ('c', gbarray->data[4*i+2]);
      cut_assert_equal_int ('d', gbarray->data[4*i+3]);
    }

  free_byte_array ();
}

