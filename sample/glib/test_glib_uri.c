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


#include "cut.h"

void test_g_filename_to_uri (void);
void test_g_filename_to_uri_error (void);
void test_g_filename_from_uri (void);
void test_g_filename_from_uri_error (void);
void test_roundtrip (void);
void test_uri_list (void);

typedef struct
{
  char *filename;
  char *hostname;
  char *expected_result;
}  ToUriTest;

ToUriTest
to_uri_tests[] = {
  { "/etc", NULL, "file:///etc"},
  { "/etc", "", "file:///etc"},
  { "/etc", "otherhost", "file://otherhost/etc"},
#ifdef G_OS_WIN32
  { "/etc", "localhost", "file:///etc"},
  { "c:\\windows", NULL, "file:///c:/windows"},
  { "c:\\windows", "localhost", "file:///c:/windows"},
  { "c:\\windows", "otherhost", "file://otherhost/c:/windows"},
  { "\\\\server\\share\\dir", NULL, "file:////server/share/dir"},
  { "\\\\server\\share\\dir", "localhost", "file:////server/share/dir"},
#else
  { "/etc", "localhost", "file://localhost/etc"},
#endif
#ifndef G_PLATFORM_WIN32
  { "/etc/\xE5\xE4\xF6", NULL, "file:///etc/%E5%E4%F6" },
  { "/etc/\xC3\xB6\xC3\xA4\xC3\xA5", NULL, "file:///etc/%C3%B6%C3%A4%C3%A5"},
#endif
  { "/etc/file with #%", NULL, "file:///etc/file%20with%20%23%25"},
  { "/0123456789", NULL, "file:///0123456789"},
  { "/ABCDEFGHIJKLMNOPQRSTUVWXYZ", NULL, "file:///ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
  { "/abcdefghijklmnopqrstuvwxyz", NULL, "file:///abcdefghijklmnopqrstuvwxyz"},
  { "/-_.!~*'()", NULL, "file:///-_.!~*'()"},
#ifdef G_OS_WIN32
  /* As '\\' is a path separator on Win32, it gets turned into '/' in the URI */
  { "/\"#%<>[\\]^`{|}\x7F", NULL, "file:///%22%23%25%3C%3E%5B/%5D%5E%60%7B%7C%7D%7F"},
#else
  /* On Unix, '\\' is a normal character in the file name */
  { "/\"#%<>[\\]^`{|}\x7F", NULL, "file:///%22%23%25%3C%3E%5B%5C%5D%5E%60%7B%7C%7D%7F"},
#endif
  { "/;@+$,", NULL, "file:///%3B@+$,"},
  /* This and some of the following are of course as such illegal file names on Windows,
   * and would not occur in real life.
   */
  { "/:", NULL, "file:///:"},
  { "/?&=", NULL, "file:///%3F&="}, 
  { "/", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "file://ABCDEFGHIJKLMNOPQRSTUVWXYZ/"},
  { "/", "abcdefghijklmnopqrstuvwxyz", "file://abcdefghijklmnopqrstuvwxyz/"},
};


typedef struct
{
  char *filename;
  char *hostname;
  GConvertError expected_error; /* If failed */
}  ToUriErrorTest;

ToUriErrorTest
to_uri_error_tests[] = {
#ifndef G_OS_WIN32
  { "c:\\windows", NULL, G_CONVERT_ERROR_NOT_ABSOLUTE_PATH}, /* it's important to get this error on Unix */
  { "c:\\windows", "localhost", G_CONVERT_ERROR_NOT_ABSOLUTE_PATH},
  { "c:\\windows", "otherhost", G_CONVERT_ERROR_NOT_ABSOLUTE_PATH},
#endif
  { "etc", "localhost", G_CONVERT_ERROR_NOT_ABSOLUTE_PATH},
  { "/etc", "\xC3\xB6\xC3\xA4\xC3\xA5", G_CONVERT_ERROR_ILLEGAL_SEQUENCE},
  { "/etc", "\xE5\xE4\xF6", G_CONVERT_ERROR_ILLEGAL_SEQUENCE},
  { "", NULL, G_CONVERT_ERROR_NOT_ABSOLUTE_PATH},
  { "", "", G_CONVERT_ERROR_NOT_ABSOLUTE_PATH},
  { "", "localhost", G_CONVERT_ERROR_NOT_ABSOLUTE_PATH},
  { "", "otherhost", G_CONVERT_ERROR_NOT_ABSOLUTE_PATH},
  /* This and some of the following are of course as such illegal file names on Windows,
   * and would not occur in real life.
   */
  { "/", "0123456789-", G_CONVERT_ERROR_ILLEGAL_SEQUENCE},
  { "/", "_.!~*'()", G_CONVERT_ERROR_ILLEGAL_SEQUENCE},
  { "/", "\"#%<>[\\]^`{|}\x7F", G_CONVERT_ERROR_ILLEGAL_SEQUENCE},
  { "/", ";?&=+$,", G_CONVERT_ERROR_ILLEGAL_SEQUENCE},
  { "/", "/", G_CONVERT_ERROR_ILLEGAL_SEQUENCE},
  { "/", "@:", G_CONVERT_ERROR_ILLEGAL_SEQUENCE},
  { "/", "\x80\xFF", G_CONVERT_ERROR_ILLEGAL_SEQUENCE},
  { "/", "\xC3\x80\xC3\xBF", G_CONVERT_ERROR_ILLEGAL_SEQUENCE},
};


typedef struct
{
  char *uri;
  char *expected_filename;
  char *expected_hostname;
}  FromUriTest;

FromUriTest
from_uri_tests[] = {
  { "file:///etc", "/etc"},
  { "file:/etc", "/etc"},
#ifdef G_OS_WIN32
  /* On Win32 we don't return "localhost" hostames, just in case
   * it isn't recognized anyway.
   */
  { "file://localhost/etc", "/etc", NULL},
  { "file://localhost/etc/%23%25%20file", "/etc/#% file", NULL},
  { "file://localhost/\xE5\xE4\xF6", "/\xe5\xe4\xf6", NULL},
  { "file://localhost/%E5%E4%F6", "/\xe5\xe4\xf6", NULL},
#else
  { "file://localhost/etc", "/etc", "localhost"},
  { "file://localhost/etc/%23%25%20file", "/etc/#% file", "localhost"},
  { "file://localhost/\xE5\xE4\xF6", "/\xe5\xe4\xf6", "localhost"},
  { "file://localhost/%E5%E4%F6", "/\xe5\xe4\xf6", "localhost"},
#endif
  { "file://otherhost/etc", "/etc", "otherhost"},
  { "file://otherhost/etc/%23%25%20file", "/etc/#% file", "otherhost"},
  { "file:////etc/%C3%B6%C3%C3%C3%A5", "//etc/\xc3\xb6\xc3\xc3\xc3\xa5", NULL},
  { "file:////etc", "//etc"},
  { "file://///etc", "///etc"},
#ifdef G_OS_WIN32
  /* URIs with backslashes come from some nonstandard application, but accept them anyhow */
  { "file:///c:\\foo", "c:\\foo"},
  { "file:///c:/foo\\bar", "c:\\foo\\bar"},
  /* Accept also the old Netscape drive-letter-and-vertical bar convention */
  { "file:///c|/foo", "c:\\foo"},
  { "file:////server/share/dir", "\\\\server\\share\\dir"},
  { "file://localhost//server/share/foo", "\\\\server\\share\\foo"},
  { "file://otherhost//server/share/foo", "\\\\server\\share\\foo", "otherhost"},
#else
  { "file:///c:\\foo", "/c:\\foo"},
  { "file:///c:/foo", "/c:/foo"},
  { "file:////c:/foo", "//c:/foo"},
#endif
  { "file://ABCDEFGHIJKLMNOPQRSTUVWXYZ/", "/", "ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
  { "file://abcdefghijklmnopqrstuvwxyz/", "/", "abcdefghijklmnopqrstuvwxyz"},
};


typedef struct
{
  char *uri;
  GConvertError expected_error; /* If failed */
}  FromUriErrorTest;

FromUriErrorTest
from_uri_error_tests[] = {
  { "file://%C3%B6%C3%A4%C3%A5/etc", G_CONVERT_ERROR_BAD_URI},
  { "file://\xE5\xE4\xF6/etc", G_CONVERT_ERROR_BAD_URI},
  { "file://%E5%E4%F6/etc", G_CONVERT_ERROR_BAD_URI},
  { "file:///some/file#bad", G_CONVERT_ERROR_BAD_URI},
  { "file://some", G_CONVERT_ERROR_BAD_URI},
  { "", G_CONVERT_ERROR_BAD_URI},
  { "file:test", G_CONVERT_ERROR_BAD_URI},
  { "http://www.yahoo.com/", G_CONVERT_ERROR_BAD_URI},
  { "file://0123456789/", G_CONVERT_ERROR_BAD_URI},
  { "file://-_.!~*'()/", G_CONVERT_ERROR_BAD_URI},
  { "file://\"<>[\\]^`{|}\x7F/", G_CONVERT_ERROR_BAD_URI},
  { "file://;?&=+$,/", G_CONVERT_ERROR_BAD_URI},
  { "file://%C3%80%C3%BF/", G_CONVERT_ERROR_BAD_URI},
  { "file://@/", G_CONVERT_ERROR_BAD_URI},
  { "file://:/", G_CONVERT_ERROR_BAD_URI},
  { "file://#/", G_CONVERT_ERROR_BAD_URI},
  { "file://%23/", G_CONVERT_ERROR_BAD_URI},
  { "file://%2F/", G_CONVERT_ERROR_BAD_URI},
};


void
test_g_filename_to_uri (void)
{
  int i;
  gchar *res;
  
  for (i = 0; i < G_N_ELEMENTS (to_uri_tests); i++)
    {
      res = g_filename_to_uri (to_uri_tests[i].filename,
			       to_uri_tests[i].hostname,
			       NULL);
      cut_assert_equal_string(to_uri_tests[i].expected_result, res);
      g_free (res);
    }
}

void
test_g_filename_to_uri_error (void)
{
  int i;
  gchar *res;
  GError *error;
  
  for (i = 0; i < G_N_ELEMENTS (to_uri_error_tests); i++)
    {
      error = NULL;
      res = g_filename_to_uri (to_uri_error_tests[i].filename,
			       to_uri_error_tests[i].hostname,
			       &error);
      cut_assert (!res);
      cut_assert (error);
      cut_assert (error->domain == G_CONVERT_ERROR);
      cut_assert_equal_int (to_uri_error_tests[i].expected_error, error->code);
      g_error_free (error);
    }
}

void
test_g_filename_from_uri (void)
{
  int i;
  gchar *res;
  gchar *hostname;
  
  for (i = 0; i < G_N_ELEMENTS (from_uri_tests); i++)
    {
      res = g_filename_from_uri (from_uri_tests[i].uri,
			         &hostname,
			         NULL);
      cut_assert_equal_string (from_uri_tests[i].expected_filename, res);
      cut_assert_equal_string (from_uri_tests[i].expected_hostname, hostname);
      g_free (hostname);
      g_free (res);
    }
}

void
test_g_filename_from_uri_error (void)
{
  int i;
  gchar *res;
  gchar *hostname;
  GError *error;
  
  for (i = 0; i < G_N_ELEMENTS (from_uri_error_tests); i++)
    {
      error = NULL;
      res = g_filename_from_uri (from_uri_error_tests[i].uri,
			         &hostname,
			         &error);
#ifdef G_OS_WIN32
      gchar *slash, *p;

      p = from_uri_error_tests[i].expected_filename = g_strdup (from_uri_error_tests[i].expected_filename);
      while ((slash = strchr (p, '/')) != NULL)
        {
          *slash = '\\';
	  p = slash + 1;
	}
#endif
      cut_assert (!res);
      cut_assert (error);
      cut_assert (error->domain == G_CONVERT_ERROR);
      cut_assert_equal_int (from_uri_error_tests[i].expected_error, error->code);
      g_error_free (error);
      g_free (res);
      g_free (hostname);
    }
}

void
test_roundtrip (void)
{
  int i;
  gchar *uri, *hostname, *res;
  GError *error;
  
  for (i = 0; i < G_N_ELEMENTS (to_uri_tests); i++)
    {
      error = NULL;
      uri = g_filename_to_uri (to_uri_tests[i].filename,
			       to_uri_tests[i].hostname,
			       &error);
      
      cut_assert(!error);
      
      error = NULL;
      res = g_filename_from_uri (uri, &hostname, &error);
      cut_assert (!error);

      cut_assert_equal_string (to_uri_tests[i].filename, res);

      if (!to_uri_tests[i].hostname)
        to_uri_tests[i].hostname = "";
      if (!hostname)
        hostname = "";
      cut_assert_equal_string (to_uri_tests[i].hostname, hostname);
    }
}

void
test_uri_list (void)
{
  /* straight from the RFC */
  gchar *list =
    "# urn:isbn:0-201-08372-8\r\n"
    "http://www.huh.org/books/foo.html\r\n"
    "http://www.huh.org/books/foo.pdf   \r\n"
    "   ftp://ftp.foo.org/books/foo.txt\r\n";
  gchar *expected_uris[] = {
    "http://www.huh.org/books/foo.html",
    "http://www.huh.org/books/foo.pdf",
    "ftp://ftp.foo.org/books/foo.txt"
  };

  gchar **uris;
  gint j;

  uris = g_uri_list_extract_uris (list);
  
  cut_assert_equal_int (3, g_strv_length (uris));
  
  for (j = 0; j < 3; j++)
    {
      cut_assert_equal_string (expected_uris[j], uris[j]);
    }

  g_strfreev (uris);

  uris = g_uri_list_extract_uris ("# just hot air\r\n# more hot air");
  cut_assert_equal_int (0, g_strv_length (uris));
}

