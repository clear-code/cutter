#include <cutter.h>
#include <gio/gfile.h>

void test_get_path (void);
void test_get_uri (void);

static GFile *file = NULL;
static gchar *tmp_string = NULL;

static void
free_tmp_string (void)
{
  if (tmp_string)
    {
      g_free (tmp_string);
      tmp_string = NULL;
    }
}

static void
free_gfile (void)
{
  if (file)
    {
      g_object_unref (file);
      file = NULL;
    }
}

void
setup (void)
{
}

void
teardown (void)
{
}

void
test_get_path (void)
{
  file = g_file_new_for_path ("/path/file");
  tmp_string = g_file_get_path (file);
  cut_assert_equal_string ("/path/file", tmp_string);

  free_tmp_string ();
}

void
test_get_uri (void)
{
  file = g_file_new_for_path ("/path/file");
  tmp_string = g_file_get_uri (file);
  cut_assert_equal_string ("file:///path/file", tmp_string);

  free_tmp_string ();
  free_gfile ();
}

