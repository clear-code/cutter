/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <errno.h>
#include <locale.h>
#include <sys/types.h>
#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#else
# define WEXITSTATUS(status) (status)
#endif
#include <signal.h>

#include <gcutter.h>

void test_run (void);
void test_io (void);
void test_flags (void);
void test_env (void);
void test_kill (void);
void test_command_line (void);
void test_wait_before_run (void);
void test_wait_after_reaped (void);
void test_wait_timeout (void);
void test_output_string (void);
void test_error_string (void);
#ifdef CUT_SUPPORT_GIO
void test_output_stream (void);
void test_error_stream (void);
#endif

static GCutProcess *process;
static GError *expected_error;
static GError *actual_error;
static GString *output_string;
static GString *error_string;
static gint exit_status;
static gboolean reaped;
static gchar *current_locale;
static const gchar *cuttest_echo_path;

void
cut_setup (void)
{
    process = NULL;
    expected_error = NULL;
    actual_error = NULL;
    output_string = g_string_new(NULL);
    error_string = g_string_new(NULL);
    exit_status = 0;
    reaped = FALSE;

    current_locale = g_strdup(setlocale(LC_ALL, NULL));
    setlocale(LC_ALL, "C");

    cuttest_echo_path = cut_build_path(cut_get_test_directory(),
                                       "..",
                                       "lib",
                                       "cuttest-echo",
                                       "cuttest-echo",
                                       NULL);
}

void
cut_teardown (void)
{
    if (current_locale) {
        setlocale(LC_ALL, current_locale);
        g_free(current_locale);
    } else {
        setlocale(LC_ALL, "");
    }

    if (process)
        g_object_unref(process);

    if (expected_error)
        g_error_free(expected_error);
    if (actual_error)
        g_error_free(actual_error);

    if (output_string)
        g_string_free(output_string, TRUE);
    if (error_string)
        g_string_free(error_string, TRUE);
}

static void
cb_output_received (GCutProcess *process, const gchar *chunk, gsize size,
                    gpointer user_data)
{
    g_string_append_len(output_string, chunk, size);
}

static void
cb_error_received (GCutProcess *process, const gchar *chunk, gsize size,
                   gpointer user_data)
{
    g_string_append_len(error_string, chunk, size);
}

static void
cb_reaped (GCutProcess *process, gint status, gpointer user_data)
{
    exit_status = status;
    reaped = TRUE;
}

static void
cb_error (GCutProcess *process, GError *error, gpointer user_data)
{
    actual_error = g_error_copy(error);
}

static void
setup_process (GCutProcess *process)
{
#define CONNECT(name)                                                   \
    g_signal_connect(process, #name, G_CALLBACK(cb_ ## name), NULL)

    CONNECT(output_received);
    CONNECT(error_received);
    CONNECT(reaped);
    CONNECT(error);
#undef CONNECT
}

static void
wait_reaped_helper (gint expected_signal)
{
    GError *error = NULL;
    gint returned_exit_status;

    returned_exit_status = gcut_process_wait(process, 1000, &error);
    gcut_assert_error(error);
    cut_assert_equal_int(exit_status, returned_exit_status);

    if (expected_signal) {
        cut_assert_true(WIFSIGNALED(exit_status));
        cut_assert_equal_int(expected_signal, WTERMSIG(exit_status));
    } else {
        cut_assert_true(WIFEXITED(exit_status));
        cut_assert_equal_int(EXIT_SUCCESS, WEXITSTATUS(exit_status));
    }
}

#define wait_exited()                           \
    cut_trace(wait_reaped_helper(0))

#define wait_termed_by_signal(signal)           \
    cut_trace(wait_reaped_helper(signal))

void
test_run (void)
{
    GError *error = NULL;

    process = gcut_process_new(cuttest_echo_path, "XXX", NULL);
    setup_process(process);

    gcut_assert_equal_pid(0, gcut_process_get_pid(process));
    gcut_process_run(process, &error);
    gcut_assert_error(error);
    gcut_assert_not_equal_pid(0, gcut_process_get_pid(process));

    wait_exited();
    cut_assert_equal_string("XXX\n", output_string->str);
    cut_assert_equal_string("", error_string->str);
}

void
test_io (void)
{
    GError *error = NULL;
    const gchar buffer[] = "XXX\n";
    const gchar command[] =
#ifdef G_OS_WIN32
        "type"
#else
        "cat"
#endif
        ;

    process = gcut_process_new(command, NULL);
    setup_process(process);

    gcut_process_run(process, &error);
    gcut_assert_error(error);

    gcut_process_write(process, buffer, strlen(buffer), &error);
    gcut_assert_error(error);
    g_io_channel_shutdown(gcut_process_get_input_channel(process), TRUE, &error);
    gcut_assert_error(error);

    wait_exited();
    cut_assert_equal_string("XXX\n", output_string->str);
    cut_assert_equal_string("", error_string->str);
}

void
test_flags (void)
{
    GError *error = NULL;

    process = gcut_process_new("echo", "XXX", NULL);
    setup_process(process);

    gcut_process_set_flags(process, 0);
    cut_assert_false(gcut_process_get_flags(process) & G_SPAWN_SEARCH_PATH);
    cut_assert_false(gcut_process_run(process, &actual_error));
    expected_error =
        g_error_new(G_SPAWN_ERROR, G_SPAWN_ERROR_NOENT,
                    "Failed to execute child process \"%s\" (%s)",
                    "echo", g_strerror(ENOENT));
    gcut_assert_equal_error(expected_error, actual_error);

    gcut_process_set_flags(process, G_SPAWN_SEARCH_PATH);
    gcut_process_run(process, &error);
    gcut_assert_error(error);

    wait_exited();
    cut_assert_equal_string("XXX\n", output_string->str);
    cut_assert_equal_string("", error_string->str);
}

void
test_env (void)
{
    GError *error = NULL;
    const gchar command[] = "env";
    gchar *env[] = {
        "name1=value1", "name2=value2", NULL
    };

    process = gcut_process_new(command, NULL);
    setup_process(process);

    gcut_process_set_env(process,
                     "name1", "value1",
                     "no-value", NULL,
                     "name2", "value2",
                     NULL);
    cut_assert_equal_string_array_with_free(env, gcut_process_get_env(process));

    gcut_process_set_flags(process, G_SPAWN_SEARCH_PATH);
    gcut_process_run(process, &error);
    gcut_assert_error(error);

    wait_exited();
    cut_assert_equal_string("name1=value1\n"
                            "name2=value2\n",
                            output_string->str);
    cut_assert_equal_string("", error_string->str);
}


void
test_kill (void)
{
    GError *error = NULL;
    const gchar command[] = "cat";
#ifdef G_OS_WIN32
#define SIGKILL 9
#endif

    process = gcut_process_new(command, NULL);
    setup_process(process);

    gcut_process_set_flags(process, G_SPAWN_SEARCH_PATH);
    gcut_process_run(process, &error);
    gcut_assert_error(error);

    gcut_process_kill(process, SIGKILL, &error);
    gcut_assert_error(error);
    wait_termed_by_signal(SIGKILL);
}

void
test_command_line (void)
{
    GError *error = NULL;
    const gchar *command_line;
    GString expected = { "command_line_arg\n", 17, 0 };
    GString *actual;

    command_line = cut_take_printf("%s command_line_arg", cuttest_echo_path);

    process = gcut_process_new_command_line(command_line);

    gcut_process_run(process, &error);
    gcut_assert_error(error);
    wait_exited();

    actual = gcut_process_get_output_string(process);
    gcut_assert_equal_string(&expected, actual);
}

void
test_wait_before_run (void)
{
    process = gcut_process_new(cuttest_echo_path, "XXX", NULL);

    cut_assert_equal_int(-1, gcut_process_wait(process, 0, &actual_error));
    expected_error = g_error_new(GCUT_PROCESS_ERROR,
                                 GCUT_PROCESS_ERROR_NOT_RUNNING,
                                 "not running: <%s XXX>",
                                 cuttest_echo_path);
    gcut_assert_equal_error(expected_error, actual_error);
}

void
test_wait_after_reaped (void)
{
    GError *error = NULL;

    process = gcut_process_new(cuttest_echo_path, "XXX", NULL);

    gcut_process_run(process, &error);
    gcut_assert_error(error);

    wait_exited();

    cut_assert_equal_int(-1, gcut_process_wait(process, 0, &actual_error));
    expected_error = g_error_new(GCUT_PROCESS_ERROR,
                                 GCUT_PROCESS_ERROR_NOT_RUNNING,
                                 "not running: <%s XXX>",
                                 cuttest_echo_path);
    gcut_assert_equal_error(expected_error, actual_error);
}

void
test_wait_timeout (void)
{
    GError *error = NULL;

    process = gcut_process_new("sleep", "1", NULL);

    gcut_process_run(process, &error);
    gcut_assert_error(error);

    cut_assert_equal_int(-1, gcut_process_wait(process, 0, &actual_error));
    expected_error = g_error_new(GCUT_PROCESS_ERROR,
                                 GCUT_PROCESS_ERROR_TIMEOUT,
                                 "timeout while waiting reaped: <sleep 1>");
    gcut_assert_equal_error(expected_error, actual_error);
}

void
test_output_string (void)
{
    GError *error = NULL;
    GString expected = { "XXX\n", 4, 0 };
    GString *actual;

    process = gcut_process_new(cuttest_echo_path, "XXX", NULL);

    gcut_process_run(process, &error);
    gcut_assert_error(error);
    wait_exited();

    actual = gcut_process_get_output_string(process);
    gcut_assert_equal_string(&expected, actual);
}

void
test_error_string (void)
{
    GError *error = NULL;
    GString expected = { "EEE\n", 4, 0 };
    GString *actual;

    process = gcut_process_new(cuttest_echo_path, "-e", "EEE", NULL);

    gcut_process_run(process, &error);
    gcut_assert_error(error);
    wait_exited();

    actual = gcut_process_get_error_string(process);
    gcut_assert_equal_string(&expected, actual);
}

#ifdef CUT_SUPPORT_GIO
void
test_output_stream (void)
{
    GError *error = NULL;
    GInputStream *stream;
    char buffer[4096];
    gsize bytes_read;

    process = gcut_process_new(cuttest_echo_path, "XXX", NULL);

    stream = gcut_process_get_output_stream(process);
    cut_assert_not_null(stream);

    gcut_process_run(process, &error);
    gcut_assert_error(error);
    wait_exited();

    g_input_stream_read_all(stream,
                            buffer, sizeof(buffer),
                            &bytes_read,
                            NULL,
                            &error);
    gcut_assert_error(error);
    cut_assert_equal_memory("XXX\n", 4, buffer, bytes_read);
}

void
test_error_stream (void)
{
    GError *error = NULL;
    GInputStream *stream;
    char buffer[4096];
    gsize bytes_read;

    process = gcut_process_new(cuttest_echo_path, "-e", "XXX", NULL);

    stream = gcut_process_get_error_stream(process);
    cut_assert_not_null(stream);

    gcut_process_run(process, &error);
    gcut_assert_error(error);
    wait_exited();

    g_input_stream_read_all(stream,
                            buffer, sizeof(buffer),
                            &bytes_read,
                            NULL,
                            &error);
    gcut_assert_error(error);
    cut_assert_equal_memory("XXX\n", 4, buffer, bytes_read);
}
#endif

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
