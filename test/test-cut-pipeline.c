/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cutter.h>
#include <gcutter.h>
#include <cutter/cut-pipeline.h>
#include <cutter/cut-runner.h>

#include "lib/cuttest-assertions.h"

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

void data_signal (void);
void test_signal (gconstpointer data);
void data_count (void);
void test_count (gconstpointer data);

static GHashTable *pipelines;
static GMutex *fixture_mutex, *run_mutex;

#define build_test_dir(dir, ...)                \
    g_build_filename(cuttest_get_base_dir(),    \
                     "fixtures",                \
                     "pipeline",                \
                     dir,                       \
                     ## __VA_ARGS__,            \
                     NULL)

void
startup (void)
{
    pipelines = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                      NULL, g_object_unref);
    fixture_mutex = g_mutex_new();
    run_mutex = g_mutex_new();
}

void
shutdown (void)
{
    g_mutex_free(run_mutex);
    g_mutex_free(fixture_mutex);
    g_hash_table_unref(pipelines);
}

#define pipeline (g_hash_table_lookup(pipelines, g_thread_self()))

void
setup (void)
{
    g_mutex_lock(fixture_mutex);
    g_hash_table_insert(pipelines, g_thread_self(), cut_pipeline_new());
    g_mutex_unlock(fixture_mutex);
}

void
teardown (void)
{
    g_mutex_lock(fixture_mutex);
    g_hash_table_remove(pipelines, g_thread_self());
    g_mutex_unlock(fixture_mutex);
}

static void
report_error (CutRunContext *context, GError *error, gpointer user_data)
{
    gcut_assert_error(error, "Pipeline Error");
}

static gboolean
run (const gchar *test_dir)
{
    gboolean success;

    cut_run_context_set_test_directory(pipeline, test_dir);
    cut_run_context_set_source_directory(pipeline, test_dir);

    g_signal_connect(pipeline, "error", G_CALLBACK(report_error), NULL);

    g_mutex_lock(run_mutex);
    success = cut_run_context_start(pipeline);
    g_mutex_unlock(run_mutex);

    return success;
}

typedef struct _SignalTestData
{
    gchar *signal_name;
    gchar *test_dir;
    gboolean success;
} SignalTestData;

static SignalTestData *
signal_test_data_new (const gchar *result, gboolean success)
{
    SignalTestData *test_data;

    test_data = g_new(SignalTestData, 1);
    test_data->signal_name = g_strconcat(result, "-test", NULL);
    test_data->test_dir = build_test_dir(result);
    test_data->success = success;

    return test_data;
}

static void
signal_test_data_free (SignalTestData *test_data)
{
    g_free(test_data->signal_name);
    g_free(test_data->test_dir);
    g_free(test_data);
}

void
data_signal (void)
{
    cut_add_data("success",
                 signal_test_data_new("success", TRUE),
                 signal_test_data_free,
                 "failure",
                 signal_test_data_new("failure", FALSE),
                 signal_test_data_free,
                 "error",
                 signal_test_data_new("error", FALSE),
                 signal_test_data_free,
                 "pending",
                 signal_test_data_new("pending", FALSE),
                 signal_test_data_free,
                 "omission",
                 signal_test_data_new("omission", TRUE),
                 signal_test_data_free);
}

static void
cb_count_signal (CutRunContext *run_context, CutTest *test,
                 CutTestContext *test_context, CutTestResult *result,
                 gpointer data)
{
    gint *n_signal = data;
    (*n_signal)++;
}

void
test_signal (gconstpointer data)
{
    const SignalTestData *test_data = data;
    gint n_signal = 0;

    g_signal_connect(pipeline, test_data->signal_name,
                     G_CALLBACK(cb_count_signal), &n_signal);

    if (test_data->success)
        cut_assert_true(run(test_data->test_dir));
    else
        cut_assert_false(run(test_data->test_dir));

    cut_assert_equal_int(1, n_signal);

    g_signal_handlers_disconnect_by_func(pipeline,
                                         G_CALLBACK(cb_count_signal),
                                         &n_signal);
}

typedef guint (*GetCountFunction) (CutRunContext *run_context);
typedef struct _CountTestData
{
    GetCountFunction get_count;
    gchar *test_dir;
    gboolean success;
} CountTestData;

static CountTestData *
count_test_data_new (const gchar *result, GetCountFunction get_count,
                     gboolean success)
{
    CountTestData *test_data;

    test_data = g_new(CountTestData, 1);
    test_data->get_count = get_count;
    test_data->test_dir = build_test_dir(result);
    test_data->success = success;

    return test_data;
}

static void
count_test_data_free (CountTestData *test_data)
{
    g_free(test_data->test_dir);
    g_free(test_data);
}

void
data_count (void)
{
#define ADD_DATA(result, result_plural, success) do                     \
{                                                                       \
    GetCountFunction get_count;                                         \
                                                                        \
    get_count = cut_run_context_get_n_ ## result_plural;                \
    cut_add_data(#result,                                               \
                 count_test_data_new(#result, get_count, success),      \
                 count_test_data_free);                                 \
} while (0)

    ADD_DATA(success, successes, TRUE);
    ADD_DATA(failure, failures, FALSE);
    ADD_DATA(error, errors, FALSE);
    ADD_DATA(pending, pendings, FALSE);
    ADD_DATA(omission, omissions, TRUE);

    ADD_DATA(setup-error, errors, FALSE);

#undef ADD_DATA
}

void
test_count (gconstpointer data)
{
    const CountTestData *test_data = data;

    if (test_data->success)
        cut_assert_true(run(test_data->test_dir));
    else
        cut_assert_false(run(test_data->test_dir));
    cut_assert_equal_uint(1, test_data->get_count(pipeline));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
