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

void test_hatch (void);
void test_io (void);
void test_flags (void);
void test_env (void);
void test_kill (void);
void test_wait_before_hatch (void);
void test_wait_after_reaped (void);
void test_wait_timeout (void);

static GCutEgg *egg;
static GError *expected_error;
static GError *actual_error;
static GString *output_string;
static GString *error_string;
static gint exit_status;
static gboolean reaped;
static gchar *current_locale;

void
cut_setup (void)
{
    egg = NULL;
    expected_error = NULL;
    actual_error = NULL;
    output_string = g_string_new(NULL);
    error_string = g_string_new(NULL);
    exit_status = 0;
    reaped = FALSE;

    current_locale = g_strdup(setlocale(LC_ALL, NULL));
    setlocale(LC_ALL, "C");
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

    if (egg)
        g_object_unref(egg);

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
cb_output_received (GCutEgg *egg, const gchar *chunk, gsize size,
                    gpointer user_data)
{
    g_string_append_len(output_string, chunk, size);
}

static void
cb_error_received (GCutEgg *egg, const gchar *chunk, gsize size,
                   gpointer user_data)
{
    g_string_append_len(error_string, chunk, size);
}

static void
cb_reaped (GCutEgg *egg, gint status, gpointer user_data)
{
    exit_status = status;
    reaped = TRUE;
}

static void
cb_error (GCutEgg *egg, GError *error, gpointer user_data)
{
    actual_error = g_error_copy(error);
}

static void
setup_egg (GCutEgg *egg)
{
#define CONNECT(name)                                                   \
    g_signal_connect(egg, #name, G_CALLBACK(cb_ ## name), NULL)

    CONNECT(output_received);
    CONNECT(error_received);
    CONNECT(reaped);
    CONNECT(error);
#undef CONNECT
}

static void
wait_reaped_helper (void)
{
    GError *error = NULL;
    gint returned_exit_status;

    returned_exit_status = gcut_egg_wait(egg, 1000, &error);
    gcut_assert_error(error);
    cut_assert_equal_int(returned_exit_status, exit_status);
    cut_assert_equal_int(EXIT_SUCCESS, WEXITSTATUS(exit_status));
}

#define wait_reaped()                           \
    cut_trace(wait_reaped_helper())

void
test_hatch (void)
{
    GError *error = NULL;

    egg = gcut_egg_new("echo", "XXX", NULL);
    setup_egg(egg);

    gcut_assert_equal_pid(0, gcut_egg_get_pid(egg));
    gcut_egg_hatch(egg, &error);
    gcut_assert_error(error);
    gcut_assert_not_equal_pid(0, gcut_egg_get_pid(egg));

    wait_reaped();
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

    egg = gcut_egg_new(command, NULL);
    setup_egg(egg);

    gcut_egg_hatch(egg, &error);
    gcut_assert_error(error);

    gcut_egg_write(egg, buffer, strlen(buffer), &error);
    gcut_assert_error(error);
    g_io_channel_shutdown(gcut_egg_get_input(egg), TRUE, &error);
    gcut_assert_error(error);

    wait_reaped();
    cut_assert_equal_string("XXX\n", output_string->str);
    cut_assert_equal_string("", error_string->str);
}

void
test_flags (void)
{
    GError *error = NULL;
    const gchar command[] = "echo";

    egg = gcut_egg_new(command, "XXX", NULL);
    setup_egg(egg);

    gcut_egg_set_flags(egg, G_SPAWN_SEARCH_PATH);
    gcut_egg_hatch(egg, &error);
    gcut_assert_error(error);

    wait_reaped();
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

    egg = gcut_egg_new(command, NULL);
    setup_egg(egg);

    gcut_egg_set_env(egg,
                     "name1", "value1",
                     "no-value", NULL,
                     "name2", "value2",
                     NULL);
    cut_assert_equal_string_array_with_free(env, gcut_egg_get_env(egg));

    gcut_egg_set_flags(egg, G_SPAWN_SEARCH_PATH);
    gcut_egg_hatch(egg, &error);
    gcut_assert_error(error);

    wait_reaped();
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

    egg = gcut_egg_new(command, NULL);
    setup_egg(egg);

    gcut_egg_set_flags(egg, G_SPAWN_SEARCH_PATH);
    gcut_egg_hatch(egg, &error);
    gcut_assert_error(error);

    gcut_egg_kill(egg, SIGKILL);
    wait_reaped();
}

void
test_wait_before_hatch (void)
{
    egg = gcut_egg_new("echo", "XXX", NULL);

    cut_assert_equal_int(-1, gcut_egg_wait(egg, 0, &actual_error));
    expected_error = g_error_new(GCUT_EGG_ERROR,
                                 GCUT_EGG_ERROR_NOT_RUNNING,
                                 "not running: <echo XXX>");
    gcut_assert_equal_error(expected_error, actual_error);
}

void
test_wait_after_reaped (void)
{
    GError *error = NULL;

    egg = gcut_egg_new("echo", "XXX", NULL);

    gcut_egg_hatch(egg, &error);
    gcut_assert_error(error);

    wait_reaped();

    cut_assert_equal_int(-1, gcut_egg_wait(egg, 0, &actual_error));
    expected_error = g_error_new(GCUT_EGG_ERROR,
                                 GCUT_EGG_ERROR_NOT_RUNNING,
                                 "not running: <echo XXX>");
    gcut_assert_equal_error(expected_error, actual_error);
}

void
test_wait_timeout (void)
{
    GError *error = NULL;

    egg = gcut_egg_new("sleep", "1", NULL);

    gcut_egg_hatch(egg, &error);
    gcut_assert_error(error);

    cut_assert_equal_int(-1, gcut_egg_wait(egg, 0, &actual_error));
    expected_error = g_error_new(GCUT_EGG_ERROR,
                                 GCUT_EGG_ERROR_TIMEOUT,
                                 "timeout while waiting reaped: <sleep 1>");
    gcut_assert_equal_error(expected_error, actual_error);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
