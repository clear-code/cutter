/* floatingtest.c - test floating flag uses
 * Copyright (C) 2005 Tim Janik
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

#include <cutter.h>
#include <gtk/gtk.h>

void test_floating (void);

static gboolean destroyed = FALSE;
static void destroy (void) { destroyed = TRUE; }
     
void
setup (void)
{
}

void
teardown (void)
{
}

void
test_floating (void)
{
  GtkWidget *widget;
  widget = g_object_new (GTK_TYPE_LABEL, NULL);
  g_object_connect (widget, "signal::destroy", destroy, NULL, NULL);

  cut_assert (GTK_OBJECT_FLOATING (widget));
  cut_assert (g_object_is_floating (widget));

  GTK_OBJECT_UNSET_FLAGS (widget, GTK_FLOATING);
  cut_assert (!GTK_OBJECT_FLOATING (widget));
  cut_assert (!g_object_is_floating (widget));

  GTK_OBJECT_SET_FLAGS (widget, GTK_FLOATING);
  cut_assert (GTK_OBJECT_FLOATING (widget));
  cut_assert (g_object_is_floating (widget));

  g_object_ref_sink (widget);
  cut_assert (!GTK_OBJECT_FLOATING (widget));
  cut_assert (!g_object_is_floating (widget));

  g_object_force_floating (G_OBJECT (widget));
  cut_assert (GTK_OBJECT_FLOATING (widget));
  cut_assert (g_object_is_floating (widget));

  g_object_ref (widget);
  gtk_object_sink (GTK_OBJECT (widget));
  cut_assert (!GTK_OBJECT_FLOATING (widget));
  cut_assert (!g_object_is_floating (widget));

  cut_assert (!destroyed);
  g_object_unref (widget);
  cut_assert (destroyed);
}

