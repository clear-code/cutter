#include <cutter.h>
#include <cut-utils.h>

#ifdef HAVE_CONFIG_H
#  include <cutter/config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>

#ifdef HAVE_GTK
#  include <gtk/gtk.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#  include <sys/types.h>
#  include <sys/wait.h>
#else
#  define WIFEXITED(status) TRUE
#  define WEXITSTATUS(status) (status)
#endif

#ifdef G_OS_WIN32
#define LINE_FEED_CODE "\r\n"
#else
#define LINE_FEED_CODE "\n"
#endif

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
static gchar *lang = NULL;

static const gchar *help_message;

#define cut_assert_exit_status(status) do                       \
{                                                               \
    cut_assert_true(WIFEXITED(exit_status));                    \
    cut_assert_equal_int(status, WEXITSTATUS(exit_status));     \
} while (0)

#define cut_assert_exit_success()               \
    cut_assert_exit_status(EXIT_SUCCESS)

#define cut_assert_exit_failure()               \
    cut_assert_exit_status(EXIT_FAILURE)

void
setup (void)
{
    stdout_string = NULL;
    stderr_string = NULL;
    exit_status = 0;
    lang = g_strdup(g_getenv("LANG"));

    help_message = cut_take_printf(
        "Usage:" LINE_FEED_CODE
        "  %s [OPTION...] TEST_DIRECTORY" LINE_FEED_CODE
        "" LINE_FEED_CODE
        "Help Options:" LINE_FEED_CODE
        "  -?, --help                                      Show help options" LINE_FEED_CODE
        "  --help-all                                      Show all help options" LINE_FEED_CODE
        "  --help-stream                                   Show stream options" LINE_FEED_CODE
        "  --help-report                                   Show report options" LINE_FEED_CODE
        "  --help-ui                                       Show UI options" LINE_FEED_CODE
        "  --help-console-ui                               Show console UI options" LINE_FEED_CODE
        "" LINE_FEED_CODE
        "Application Options:" LINE_FEED_CODE
        "  --version                                       Show version" LINE_FEED_CODE
        "  -s, --source-directory=DIRECTORY                Set directory of source code" LINE_FEED_CODE
        "  -n, --name=TEST_NAME                            Specify tests" LINE_FEED_CODE
        "  -t, --test-case=TEST_CASE_NAME                  Specify test cases" LINE_FEED_CODE
        "  -m, --multi-thread                              Run test cases with multi-thread" LINE_FEED_CODE
        "  --test-case-order=[none|name|name-desc]         Sort test case by. Default is 'none'." LINE_FEED_CODE
        "  --exclude-file=FILE_NAME                        Skip files" LINE_FEED_CODE
        "  --exclude-dir=DIRECTORY_NAME                    Skip directories" LINE_FEED_CODE
        "" LINE_FEED_CODE,
        g_get_prgname());
}

void
teardown (void)
{
    if (stdout_string)
        g_free(stdout_string);
    if (stderr_string)
        g_free(stderr_string);

    if (lang) {
        g_setenv("LANG", lang, TRUE);
        g_free(lang);
    } else {
        g_unsetenv("LANG");
    }
}

static gboolean
run_cutter (const gchar *options)
{
    const gchar *cutter_command;
    gchar *command;
    gint argc;
    gchar **argv;
    gboolean ret;

    cutter_command = cut_utils_get_cutter_command_path();
    cut_assert(cutter_command);

    g_setenv("LANG", "C", TRUE);

    if (options)
        command = g_strdup_printf("\"%s\" %s", cutter_command, options);
    else
        command = g_strdup_printf("\"%s\"", cutter_command);
    g_shell_parse_argv(command, &argc, &argv, NULL);
    g_free(command);

    ret = g_spawn_sync(NULL,
                       argv,
                       NULL,
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
    cut_assert_exit_success();
    cut_assert_equal_string(VERSION LINE_FEED_CODE, stdout_string);
}

void
test_help (void)
{
    cut_assert(run_cutter("--help"));
    cut_assert_exit_success();
    cut_assert_equal_string(help_message, stdout_string);
}

void
test_no_option (void)
{
    cut_assert(run_cutter(NULL));
    cut_assert_exit_failure();
    cut_assert_equal_string(help_message, stdout_string);
}

void
test_help_all (void)
{
    const gchar *help_all_message;

    help_all_message = cut_take_printf(
        "Usage:" LINE_FEED_CODE
        "  %s [OPTION...] TEST_DIRECTORY" LINE_FEED_CODE
        "" LINE_FEED_CODE
        "Help Options:" LINE_FEED_CODE
        "  -?, --help                                      Show help options" LINE_FEED_CODE
        "  --help-all                                      Show all help options" LINE_FEED_CODE
        "  --help-stream                                   Show stream options" LINE_FEED_CODE
        "  --help-report                                   Show report options" LINE_FEED_CODE
        "  --help-ui                                       Show UI options" LINE_FEED_CODE
        "  --help-console-ui                               Show console UI options" LINE_FEED_CODE
#ifdef HAVE_GTK
        "  --help-gtk                                      Show GTK+ Options" LINE_FEED_CODE
#endif
        "" LINE_FEED_CODE
        "Stream Options" LINE_FEED_CODE
        "  --stream=[xml]                                  Specify stream" LINE_FEED_CODE
        "  --stream-fd=FILE_DESCRIPTOR                     Stream to FILE_DESCRIPTOR (default: stdout)" LINE_FEED_CODE
        "  --stream-log-dir=DIRECTORY                      Stream to a file under DIRECTORY (default: none)" LINE_FEED_CODE
        "" LINE_FEED_CODE
        "Report Options" LINE_FEED_CODE
        "  --pdf-report=FILE                               Set filename of pdf report" LINE_FEED_CODE
        "  --xml-report=FILE                               Set filename of xml report" LINE_FEED_CODE
        "" LINE_FEED_CODE
        "UI Options" LINE_FEED_CODE
#ifdef HAVE_GTK
        "  -u, --ui=[console|gtk]                          Specify UI" LINE_FEED_CODE
#else
        "  -u, --ui=[console]                              Specify UI" LINE_FEED_CODE
#endif
        "" LINE_FEED_CODE
        "Console UI Options" LINE_FEED_CODE
        "  -v, --verbose=[s|silent|n|normal|v|verbose]     Set verbose level" LINE_FEED_CODE
        "  -c, --color=[yes|true|no|false|auto]            Output log with colors" LINE_FEED_CODE
        "" LINE_FEED_CODE
#ifdef HAVE_GTK
        "GTK+ Options" LINE_FEED_CODE
        "  --class=CLASS                                   Program class as used by the window manager" LINE_FEED_CODE
        "  --gtk-name=NAME                                 Program name as used by the window manager" LINE_FEED_CODE
        "  --screen=SCREEN                                 X screen to use" LINE_FEED_CODE
#  ifdef HAVE_GTK_ENABLE_DEBUG
        "  --gdk-debug=FLAGS                               Gdk debugging flags to set" LINE_FEED_CODE
        "  --gdk-no-debug=FLAGS                            Gdk debugging flags to unset" LINE_FEED_CODE
#  endif
#  ifdef GDK_WINDOWING_X11
        "  --sync                                          Make X calls synchronous" LINE_FEED_CODE
#  elif defined(GDK_WINDOWING_WIN32)
        "  --sync                                          Don't batch GDI requests" LINE_FEED_CODE
        "  --no-wintab                                     Don't use the Wintab API for tablet support" LINE_FEED_CODE
        "  --ignore-wintab                                 Same as --no-wintab" LINE_FEED_CODE
        "  --use-wintab                                    Do use the Wintab API [default]" LINE_FEED_CODE
        "  --max-colors=COLORS                             Size of the palette in 8 bit mode" LINE_FEED_CODE
#  endif
        "  --gtk-module=MODULES                            Load additional GTK+ modules" LINE_FEED_CODE
        "  --g-fatal-warnings                              Make all warnings fatal" LINE_FEED_CODE
#  ifdef HAVE_GTK_ENABLE_DEBUG
        "  --gtk-debug=FLAGS                               GTK+ debugging flags to set" LINE_FEED_CODE
        "  --gtk-no-debug=FLAGS                            GTK+ debugging flags to unset" LINE_FEED_CODE
#  endif
        "" LINE_FEED_CODE
#endif
        "Application Options:" LINE_FEED_CODE
        "  --version                                       Show version" LINE_FEED_CODE
        "  -s, --source-directory=DIRECTORY                Set directory of source code" LINE_FEED_CODE
        "  -n, --name=TEST_NAME                            Specify tests" LINE_FEED_CODE
        "  -t, --test-case=TEST_CASE_NAME                  Specify test cases" LINE_FEED_CODE
        "  -m, --multi-thread                              Run test cases with multi-thread" LINE_FEED_CODE
        "  --test-case-order=[none|name|name-desc]         Sort test case by. Default is 'none'." LINE_FEED_CODE
        "  --exclude-file=FILE_NAME                        Skip files" LINE_FEED_CODE
        "  --exclude-dir=DIRECTORY_NAME                    Skip directories" LINE_FEED_CODE
#ifdef HAVE_GTK
        "  --display=DISPLAY                               X display to use" LINE_FEED_CODE
#endif
        "" LINE_FEED_CODE,
        g_get_prgname());

    cut_assert(run_cutter("--help-all"));
    cut_assert_exit_success();
    cut_assert_equal_string(help_all_message, stdout_string);
}

void
test_invalid_option (void)
{
    cut_assert(run_cutter("--XXXX"));
    cut_assert_exit_failure();
    cut_assert_equal_string("Unknown option --XXXX" LINE_FEED_CODE,
                            stdout_string);
}

void
test_invalid_color_option (void)
{
    cut_assert(run_cutter("--color=XXX"));
    cut_assert_exit_failure();
    cut_assert_equal_string("Invalid color value: XXX" LINE_FEED_CODE,
                            stdout_string);
}

void
test_invalid_order_option (void)
{
    cut_assert(run_cutter("--test-case-order=XXX"));
    cut_assert_exit_failure();
    cut_assert_equal_string("Invalid test case order value: XXX" LINE_FEED_CODE,
                            stdout_string);
}

void
test_invalid_verbose_option (void)
{
    cut_assert(run_cutter("--verbose=XXX"));
    cut_assert_exit_failure();
    cut_assert_equal_string("Invalid verbose level name: XXX" LINE_FEED_CODE,
                            stdout_string);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
