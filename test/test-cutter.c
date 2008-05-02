#include "cutter.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <glib.h>

void test_help (void);
void test_help_all (void);
void test_version (void);
void test_invalid_option (void);
void test_invalid_color_option (void);
void test_invalid_order_option (void);
void test_invalid_verbose_option (void);
void test_no_option (void);

static gchar *stdout_string = NULL;
static gchar *stderr_string = NULL;
static gint exit_status = 0;

static const gchar *help_message = 
        "Usage:\n"
        "  lt-cutter [OPTION...] TEST_DIRECTORY\n"
        "\n"
        "Help Options:\n"
        "  -?, --help                                      Show help options\n"
        "  --help-all                                      Show all help options\n"
        "  --help-report                                   Show report options\n"
        "  --help-ui                                       Show UI options\n"
        "  --help-ui-console                               Show console UI options\n"
        "\n"
        "Application Options:\n"
        "  --version                                       Show version\n"
        "  -s, --source-directory=DIRECTORY                Set directory of source code\n"
        "  -n, --name=TEST_NAME                            Specify tests\n"
        "  -t, --test-case=TEST_CASE_NAME                  Specify test cases\n"
        "  -m, --multi-thread                              Run test cases with multi-thread\n"
        "  --test-case-order=[none|name|name-desc]         Sort test case by. Default is 'none'.\n"
        "\n";

void
setup (void)
{
    stdout_string = NULL;
    stderr_string = NULL;
    exit_status = 0;
}

void
teardown (void)
{
    if (stdout_string)
        g_free(stdout_string);
    if (stderr_string)
        g_free(stderr_string);
}

static gboolean
run_cutter (const gchar *options)
{
    const gchar *cutter_command;
    gchar *command;
    const gchar *envp[] = {"LANG=C", NULL};
    gint argc;
    gchar **argv;
    gboolean ret;

    cutter_command = g_getenv("CUTTER");
    cut_assert(cutter_command);

    if (options)
        command = g_strdup_printf("%s %s", cutter_command, options);
    else
        command = g_strdup(cutter_command);
    g_shell_parse_argv(command, &argc, &argv, NULL);
    g_free(command);

    ret = g_spawn_sync("./",
                       argv,
                       (gchar **)envp,
                       0,
                       NULL,
                       NULL,
                       &stdout_string,
                       &stderr_string,
                       &exit_status,
                       NULL); 
    g_strfreev(argv);

    return ret;
}

void
test_version (void)
{
    cut_assert(run_cutter("--version"));
    cut_assert_equal_int(exit_status, 0);
    cut_assert_equal_string(VERSION"\n", stdout_string);
}

void
test_help (void)
{
    cut_assert(run_cutter("--help"));
    cut_assert_equal_int(exit_status, 0);
    cut_assert_equal_string(help_message, stdout_string);
}

void
test_no_option (void)
{
    cut_assert(run_cutter(NULL));
    cut_assert_equal_int(exit_status, 256);
    cut_assert_equal_string(help_message, stdout_string);
}

void
test_help_all (void)
{
    const gchar *expected = 
        "Usage:\n"
        "  lt-cutter [OPTION...] TEST_DIRECTORY\n"
        "\n"
        "Help Options:\n"
        "  -?, --help                                      Show help options\n"
        "  --help-all                                      Show all help options\n"
        "  --help-report                                   Show report options\n"
        "  --help-ui                                       Show UI options\n"
        "  --help-gtk                                      Show GTK+ Options\n"
        "  --help-ui-console                               Show console UI options\n"
        "\n"
        "Report Options\n"
        "  --pdf-report=FILE                               Set filename of pdf report\n"
        "  --xml-report=FILE                               Set filename of xml report\n"
        "\n"
        "UI Options\n"
        "  -u, --ui=[gtk|console]                          Specify UI\n"
        "\n"
        "GTK+ Options\n"
        "  --class=CLASS                                   Program class as used by the window manager\n"
        "  --gtk-name=NAME                                 Program name as used by the window manager\n"
        "  --screen=SCREEN                                 X screen to use\n"
        "  --sync                                          Make X calls synchronous\n"
        "  --gtk-module=MODULES                            Load additional GTK+ modules\n"
        "  --g-fatal-warnings                              Make all warnings fatal\n"
        "\n"
        "Console UI Options\n"
        "  -v, --verbose=[s|silent|n|normal|v|verbose]     Set verbose level\n"
        "  -c, --color=[yes|true|no|false|auto]            Output log with colors\n"
        "\n"
        "Application Options:\n"
        "  --version                                       Show version\n"
        "  -s, --source-directory=DIRECTORY                Set directory of source code\n"
        "  -n, --name=TEST_NAME                            Specify tests\n"
        "  -t, --test-case=TEST_CASE_NAME                  Specify test cases\n"
        "  -m, --multi-thread                              Run test cases with multi-thread\n"
        "  --test-case-order=[none|name|name-desc]         Sort test case by. Default is 'none'.\n"
        "  --display=DISPLAY                               X display to use\n"
        "\n";

    cut_assert(run_cutter("--help-all"));
    cut_assert_equal_int(exit_status, 0);
    cut_assert_equal_string(expected, stdout_string);
}

void
test_invalid_option (void)
{
    cut_assert(run_cutter("--XXXX"));
    cut_assert_equal_int(exit_status, 256);
    cut_assert_equal_string("Unknown option --XXXX\n", stdout_string);
}

void
test_invalid_color_option (void)
{
    cut_assert(run_cutter("--color=XXX"));
    cut_assert_equal_int(exit_status, 256);
    cut_assert_equal_string("Invalid color value: XXX\n", stdout_string);
}

void
test_invalid_order_option (void)
{
    cut_assert(run_cutter("--test-case-order=XXX"));
    cut_assert_equal_int(exit_status, 256);
    cut_assert_equal_string("Invalid test case order value: XXX\n", stdout_string);
}

void
test_invalid_verbose_option (void)
{
    cut_assert(run_cutter("--verbose=XXX"));
    cut_assert_equal_int(exit_status, 256);
    cut_assert_equal_string("Invalid verbose level name: XXX\n", stdout_string);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
