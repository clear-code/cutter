#include <gcutter.h>
#include "../lib/cuttest-utils.h"

void          cut_set_cutter_command_path (const char *argv0);
const char   *cut_get_cutter_command_path (void);

void test_cutter_command_path (void);

static gchar *original_cutter_command_path;

void
cut_setup (void)
{
    original_cutter_command_path = g_strdup(cut_get_cutter_command_path());
}

void
cut_teardown (void)
{
    if (original_cutter_command_path) {
        cut_set_cutter_command_path(original_cutter_command_path);
        g_free(original_cutter_command_path);
    }
}

void
test_cutter_command_path (void)
{
    const gchar *exist_command;
    const gchar *absolute_path, *relative_cutter, *absolute_cutter;

    cut_assert_not_null(cut_get_cutter_command_path());

#ifdef G_OS_WIN32
    exist_command = "notepad";
#else
    exist_command = "ls";
#endif
    cut_set_cutter_command_path(exist_command);
    cut_assert_equal_string(cut_take_string(g_find_program_in_path(exist_command)),
                            cut_get_cutter_command_path());

    absolute_path =
        cut_take_string(g_build_filename(G_DIR_SEPARATOR_S,
                                         "usr", "bin", "cutter", NULL));
    cut_set_cutter_command_path(absolute_path);
    cut_assert_equal_string(absolute_path, cut_get_cutter_command_path());


    relative_cutter =
        cut_take_string(g_build_filename("build", "cutter", NULL));
    cut_set_cutter_command_path(relative_cutter);
    absolute_cutter =
        cut_take_string(g_build_filename(cut_take_string(g_get_current_dir()),
                                         relative_cutter,
                                         NULL));
    cut_assert_equal_string(absolute_cutter, cut_get_cutter_command_path());
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
