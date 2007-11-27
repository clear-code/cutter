#include <cutter.h>
#include <gio/gfile.h>

void test_get_path (void);

static GFile *file = NULL;

void
setup (void)
{
}

void
teardown (void)
{
  if (file)
    {
      g_object_unref (file);
      file = NULL;
    }
}

void
test_get_path (void)
{
  gchar *path_name;

  file = g_file_new_for_path ("/path/file");
  path_name = g_file_get_path (file);
  cut_assert_equal_string ("/path/file", path_name);

  g_free (path_name);
}

