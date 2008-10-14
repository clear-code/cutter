/* -*- c-file-style: "gnu" -*- */

#include <gcutter.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

void test_iochannel (void);

void
setup (void)
{
}

void
teardown (void)
{
}

static void
test_small_writes (void)
{
  GIOChannel *io;
  GIOStatus status = G_IO_STATUS_NORMAL;
  guint cnt;
  gchar tmp;
  GError *error = NULL;

  io = g_io_channel_new_file ("iochannel-test-outfile", "w", &error);
  
  gcut_assert_error (error, "Unable to open file: iochannel-test-outfile");

  g_io_channel_set_encoding (io, NULL, NULL);
  g_io_channel_set_buffer_size (io, 1022);

  cnt = 2 * g_io_channel_get_buffer_size (io);
  tmp = 0;
 
  while (cnt)
    {
      status = g_io_channel_write_chars (io, &tmp, 1, NULL, NULL);
      if (status == G_IO_STATUS_ERROR)
	break;
      if (status == G_IO_STATUS_NORMAL)
	cnt--;
    }

  cut_assert_equal_int (G_IO_STATUS_NORMAL, status);

  g_io_channel_unref (io);
}

void
test_iochannel (void)
{
  GIOChannel *gio_r, *gio_w ;
  GString *buffer;
  char *filename;
  char *srcdir = getenv ("srcdir");
  gint rlength = 0;
  glong wlength = 0;
  gsize length_out;
  const gchar encoding[] = "EUC-JP";
  GIOStatus status;
  GIOFlags flags;
  GError *error = NULL;

  if (!srcdir)
    srcdir = ".";
  filename = g_build_filename (srcdir, "iochannel-test-infile", NULL);

  gio_r = g_io_channel_new_file (filename, "r", &error);
  gcut_assert_error (error, "Unable to open file");

  gio_w = g_io_channel_new_file ("iochannel-test-outfile", "w", &error);
  gcut_assert_error (error, "Unable to open file: iochannel-test-outfile");

  g_io_channel_set_encoding (gio_r, encoding, &error);
  gcut_assert_error (error);

  g_io_channel_set_buffer_size (gio_r, BUFFER_SIZE);

  status = g_io_channel_set_flags (gio_r, G_IO_FLAG_NONBLOCK, &error);
  gcut_assert_error (error);
  flags = g_io_channel_get_flags (gio_r);
  buffer = g_string_sized_new (BUFFER_SIZE);

  while (TRUE)
  {
      do
        {
          status = g_io_channel_read_line_string (gio_r, buffer, NULL, &error);
          gcut_assert_error (error);
        } while (status == G_IO_STATUS_AGAIN);
      if (status != G_IO_STATUS_NORMAL)
        break;

      rlength += buffer->len;

      do
        {
          status = g_io_channel_write_chars (gio_w, buffer->str, buffer->len,
                                             &length_out, &error);
          gcut_assert_error (error);
        } while (status == G_IO_STATUS_AGAIN);
      if (status != G_IO_STATUS_NORMAL)
        break;

      wlength += length_out;

      if (length_out < buffer->len)
        g_warning ("Only wrote part of the line.");

      g_string_truncate (buffer, 0);
  }

  switch (status)
    {
      case G_IO_STATUS_EOF:
        break;
      case G_IO_STATUS_ERROR:
        gcut_assert_error(error);
        break;
      default:
        g_warning ("Abnormal exit from write loop.");
        break;
    }

  do
    {
      status = g_io_channel_flush (gio_w, &error);
      gcut_assert_error (error);
    } while (status == G_IO_STATUS_AGAIN);

  cut_assert_equal_int (G_IO_STATUS_NORMAL, status);

  g_io_channel_unref(gio_r);
  g_io_channel_unref(gio_w);

  test_small_writes ();
}
