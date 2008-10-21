/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string.h>
#include <errno.h>
#include <locale.h>

#include <gcutter.h>

void test_run (void);
void test_io (void);
void test_flags (void);

static GCutSpawn *spawn;
static GError *expected_error;
static GError *actual_error;
static GString *output_string;
static GString *error_string;
static gint exit_status;
static gboolean reaped;
static gchar *current_locale;

void
setup (void)
{
    spawn = NULL;
    expected_error = NULL;
    actual_error = NULL;
    output_string = g_string_new(NULL);
    error_string = g_string_new(NULL);
    exit_status = 0;
    reaped = FALSE;

    current_locale = g_strdup(setlocale(LC_ALL, "C"));
}

void
teardown (void)
{
    if (current_locale) {
        setlocale(LC_ALL, current_locale);
        g_free(current_locale);
    } else {
        setlocale(LC_ALL, NULL);
    }

    if (spawn)
        g_object_unref(spawn);

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
cb_output_received (GCutSpawn *spawn, const gchar *chunk, gsize size,
                    gpointer user_data)
{
    g_string_append_len(output_string, chunk, size);
}

static void
cb_error_received (GCutSpawn *spawn, const gchar *chunk, gsize size,
                   gpointer user_data)
{
    g_string_append_len(error_string, chunk, size);
}

static void
cb_reaped (GCutSpawn *spawn, gint status, gpointer user_data)
{
    exit_status = status;
    reaped = TRUE;
}

static void
cb_error (GCutSpawn *spawn, GError *error, gpointer user_data)
{
    actual_error = g_error_copy(error);
}

static void
setup_spawn (GCutSpawn *spawn)
{
#define CONNECT(name)                                                   \
    g_signal_connect(spawn, #name, G_CALLBACK(cb_ ## name), NULL)

    CONNECT(output_received);
    CONNECT(error_received);
    CONNECT(reaped);
    CONNECT(error);
#undef CONNECT
}

static gboolean
cb_timeout_reaped (gpointer user_data)
{
    GError **error = user_data;
    reaped = TRUE;
    g_set_error(error, GCUT_SPAWN_ERROR, 0, "timeout");
    return FALSE;
}

static void
wait_reaped_helper (void)
{
    GError *error = NULL;
    guint timeout_id;

    gcut_assert_error(error);
    timeout_id = g_timeout_add_seconds(1, cb_timeout_reaped, &error);
    while (!reaped)
        g_main_context_iteration(NULL, TRUE);
    g_source_remove(timeout_id);
    gcut_assert_error(error);

    cut_assert_equal_int(EXIT_SUCCESS, exit_status);
}

#define wait_reaped()                           \
    cut_trace(wait_reaped_helper())

void
test_run (void)
{
    GError *error = NULL;

    spawn = gcut_spawn_new("echo", "XXX", NULL);
    setup_spawn(spawn);

    cut_assert_equal_int(0, gcut_spawn_get_pid(spawn));
    gcut_spawn_run(spawn, &error);
    gcut_assert_error(error);
    cut_assert_operator_int(0, !=, gcut_spawn_get_pid(spawn));

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

    spawn = gcut_spawn_new(command, NULL);
    setup_spawn(spawn);

    gcut_spawn_run(spawn, &error);
    gcut_assert_error(error);

    gcut_spawn_write(spawn, buffer, strlen(buffer), &error);
    gcut_assert_error(error);
    g_io_channel_shutdown(gcut_spawn_get_input(spawn), TRUE, &error);
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

    spawn = gcut_spawn_new(command, "XXX", NULL);
    setup_spawn(spawn);

    gcut_spawn_set_flags(spawn, 0);
    cut_assert_false(gcut_spawn_get_flags(spawn) & G_SPAWN_SEARCH_PATH);
    cut_assert_false(gcut_spawn_run(spawn, &actual_error));
    expected_error =
        g_error_new(G_SPAWN_ERROR, G_SPAWN_ERROR_NOENT,
                    "Failed to execute child process \"%s\" (%s)",
                    command, g_strerror(ENOENT));
    gcut_assert_equal_error(expected_error, actual_error);

    gcut_spawn_set_flags(spawn, G_SPAWN_SEARCH_PATH);
    gcut_spawn_run(spawn, &error);
    gcut_assert_error(error);

    wait_reaped();
    cut_assert_equal_string("XXX\n", output_string->str);
    cut_assert_equal_string("", error_string->str);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
