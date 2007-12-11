/* -*- c-file-style: "gnu" -*- */

#include <cut-main.h>

#include <stdlib.h>
#include <gtk/gtk.h>

int
main (int argc, char *argv[])
{
  gtk_init (&argc, &argv);
  cut_init (&argc, &argv);

  exit (cut_run () ? 0 : 1);
}
