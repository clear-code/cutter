/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 *  Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include <setjmp.h>
#define __USE_GNU
#include <signal.h>
#undef __USE_GNU

#include "cut-test-suite.h"

#include "cut-test.h"
#include "cut-test-case.h"
#include "cut-context.h"
#include "cut-utils.h"

#define CUT_TEST_SUITE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CUT_TYPE_TEST_SUITE, CutTestSuitePrivate))

typedef struct _CutTestSuitePrivate	CutTestSuitePrivate;
struct _CutTestSuitePrivate
{
};

enum
{
    PROP_0
};

enum
{
    START_TEST_CASE_SIGNAL,
    COMPLETE_TEST_CASE_SIGNAL,
    LAST_SIGNAL
};

static jmp_buf jump_buffer;
static gchar *stack_trace = NULL;

static gint cut_test_suite_signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE (CutTestSuite, cut_test_suite, CUT_TYPE_TEST_CONTAINER)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void
cut_test_suite_class_init (CutTestSuiteClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

	cut_test_suite_signals[START_TEST_CASE_SIGNAL]
        = g_signal_new("start-test-case",
                G_TYPE_FROM_CLASS(klass),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET(CutTestSuiteClass, start_test_case),
                NULL, NULL,
                g_cclosure_marshal_VOID__OBJECT,
                G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);

	cut_test_suite_signals[COMPLETE_TEST_CASE_SIGNAL]
        = g_signal_new("complete-test-case",
                G_TYPE_FROM_CLASS(klass),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET(CutTestSuiteClass, complete_test_case),
                NULL, NULL,
                g_cclosure_marshal_VOID__OBJECT,
                G_TYPE_NONE, 1, CUT_TYPE_TEST_CASE);
}

static void
cut_test_suite_init (CutTestSuite *test_suite)
{
}

static void
dispose (GObject *object)
{
    G_OBJECT_CLASS(cut_test_suite_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    switch (prop_id) {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

CutTestSuite *
cut_test_suite_new (void)
{
    return g_object_new(CUT_TYPE_TEST_SUITE, NULL);
}

typedef struct _RunTestInfo
{
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutContext *context;
    gchar **test_names;
} RunTestInfo;

static gpointer
run (gpointer data)
{
    RunTestInfo *info = data;
    CutTestSuite *test_suite;
    CutTestCase *test_case;
    CutContext *context;
    gchar **test_names;
    gboolean success;

    test_suite = info->test_suite;
    test_case = info->test_case;
    context = info->context;
    test_names = info->test_names;

    g_signal_emit_by_name(test_suite, "start-test-case", test_case);
    success = cut_test_case_run_with_filter(test_case, context,
                                            (const gchar **)test_names);
    g_signal_emit_by_name(test_suite, "complete-test-case", test_case);

    g_object_unref(test_suite);
    g_object_unref(test_case);
    g_object_unref(context);
    g_strfreev(test_names);
    g_free(info);

    return GINT_TO_POINTER(success);
}

static void
run_with_thread (CutTestSuite *test_suite, CutTestCase *test_case,
                 CutContext *context, const gchar **test_names,
                 gboolean try_thread, GList **threads, gboolean *success)
{
    RunTestInfo *info;
    gboolean need_no_thread_run = TRUE;

    info = g_new0(RunTestInfo, 1);
    info->test_suite = g_object_ref(test_suite);
    info->test_case = g_object_ref(test_case);
    info->context = g_object_ref(context);
    info->test_names = g_strdupv((gchar **)test_names);

    if (try_thread) {
        GThread *thread;
        GError *error = NULL;

        thread = g_thread_create(run, info, TRUE, &error);
        if (error) {
            g_warning("%s(%d)", error->message, error->code);
            g_error_free(error);
        } else {
            need_no_thread_run = FALSE;
            *threads = g_list_append(*threads, thread);
        }
    }

    if (need_no_thread_run) {
        if (!run(info))
            *success = FALSE;
    }
}

static void
read_stack_trace (int in_fd)
{
    GRegex *regex;
    GString *buffer;
    gchar window[4096];
    gssize size;

    buffer = g_string_new("");
    while ((size = read(in_fd, window, sizeof(window))) > 0) {
        g_string_append_len(buffer, window, size);
    }

    regex = g_regex_new("^.+?<signal handler called>\n",
                        G_REGEX_MULTILINE | G_REGEX_DOTALL,
                        G_REGEX_MATCH_NEWLINE_ANY, NULL);
    if (regex) {
        stack_trace = g_regex_replace(regex, buffer->str, buffer->len, 0,
                                      "", 0, NULL);
        g_regex_unref(regex);
        g_string_free(buffer, TRUE);
    } else {
        stack_trace = g_string_free(buffer, FALSE);
    }
}

static void
collect_stack_trace (void)
{
    int fds[2];
    FILE *original_stdout, *pseudo_stdout;

    if (pipe(fds) == -1) {
        perror("unable to open pipe for collecting stack trace");
        return;
    }

    original_stdout = stdout;
    pseudo_stdout = fdopen(fds[1], "w");
    if (!pseudo_stdout) {
        perror("unable to open FILE for pipe");
        close(fds[0]);
        close(fds[1]);
        return;
    }
    stdout = pseudo_stdout;
    g_on_error_stack_trace(g_get_prgname());
    stdout = original_stdout;
    fclose(pseudo_stdout);

    read_stack_trace(fds[0]);

    close(fds[0]);
    close(fds[1]);
}

static void
ill_be_back_handler (int signum)
{
    g_free(stack_trace);
    stack_trace = NULL;
    collect_stack_trace();
    longjmp(jump_buffer, signum);
}

static gboolean
cut_test_suite_run_test_cases (CutTestSuite *test_suite, CutContext *context,
                               const GList *tests, const gchar **test_names)
{
    const GList *list;
    GList *node, *threads = NULL;
    gboolean try_thread;
    gboolean all_success = TRUE;
    gint signum;
    sighandler_t previous_segv_handler, previous_int_handler;

    previous_segv_handler = signal(SIGSEGV, ill_be_back_handler);
    previous_int_handler = signal(SIGINT, ill_be_back_handler);
    signum = setjmp(jump_buffer);
    switch (signum) {
      case 0:
        cut_context_start_test_suite(context, test_suite);
        g_signal_emit_by_name(CUT_TEST(test_suite), "start");

        try_thread = cut_context_get_multi_thread(context);
        for (list = tests; list; list = g_list_next(list)) {
            if (!list->data)
                continue;
            if (CUT_IS_TEST_CASE(list->data)) {
                run_with_thread(test_suite, list->data, context, test_names,
                                try_thread, &threads, &all_success);
            } else {
                g_warning("This object is not test case!");
            }
        }

        for (node = threads; node; node = g_list_next(node)) {
            GThread *thread = node->data;

            if (!GPOINTER_TO_INT(g_thread_join(thread)))
                all_success = FALSE;
        }

        if (all_success)
            g_signal_emit_by_name(CUT_TEST(test_suite), "success");
        break;
      case SIGSEGV:
        all_success = FALSE;
        g_signal_emit_by_name(CUT_TEST(test_suite), "crashed", stack_trace);
        g_free(stack_trace);
        break;
      default:
        break;
    }
    signal(SIGINT, previous_int_handler);
    signal(SIGSEGV, previous_segv_handler);

    g_signal_emit_by_name(CUT_TEST(test_suite), "complete");

    return all_success;
}

gboolean
cut_test_suite_run (CutTestSuite *suite, CutContext *context)
{
    const gchar **test_case_names;
    const gchar **test_names;

    test_case_names = cut_context_get_target_test_case_names(context);
    test_names = cut_context_get_target_test_names(context);
    return cut_test_suite_run_with_filter(suite, context,
                                          test_case_names, test_names);
}

gboolean
cut_test_suite_run_test_case (CutTestSuite *suite, CutContext *context,
                              const gchar *test_case_name)
{
    const gchar *test_case_names[] = {NULL, NULL};

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_case_names[0] = test_case_name;
    return cut_test_suite_run_with_filter(suite, context,
                                          test_case_names, NULL);
}

gboolean
cut_test_suite_run_test (CutTestSuite *suite, CutContext *context,
                         const gchar *test_name)
{
    const GList *test_cases;
    const gchar *test_names[] = {NULL, NULL};

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_names[0] = test_name;
    test_cases = cut_test_container_get_children(CUT_TEST_CONTAINER(suite));
    return cut_test_suite_run_test_cases(suite, context, test_cases,
                                         test_names);
}

gboolean
cut_test_suite_run_test_in_test_case (CutTestSuite *suite,
                                      CutContext   *context,
                                      const gchar  *test_name,
                                      const gchar  *test_case_name)
{
    const gchar *test_names[] = {NULL, NULL};
    const gchar *test_case_names[] = {NULL, NULL};

    g_return_val_if_fail(CUT_IS_TEST_SUITE(suite), FALSE);

    test_names[0] = test_name;
    test_case_names[0] = test_case_name;

    return cut_test_suite_run_with_filter(suite, context, test_case_names,
                                          test_names);
}

gboolean
cut_test_suite_run_with_filter (CutTestSuite *test_suite,
                                CutContext   *context,
                                const gchar **test_case_names,
                                const gchar **test_names)
{
    CutTestContainer *container;
    GList *filtered_test_cases = NULL;
    gboolean success = TRUE;

    container = CUT_TEST_CONTAINER(test_suite);
    if (test_case_names) {
        filtered_test_cases =
            cut_test_container_filter_children(container, test_case_names);
    } else {
        filtered_test_cases =
            g_list_copy((GList *)cut_test_container_get_children(container));
    }

    success = cut_test_suite_run_test_cases(test_suite, context,
                                            filtered_test_cases, test_names);

    if (filtered_test_cases)
        g_list_free(filtered_test_cases);

    return success;
}


void
cut_test_suite_add_test_case (CutTestSuite *suite, CutTestCase *test_case)
{
    g_return_if_fail(CUT_IS_TEST_CASE(test_case));
    g_return_if_fail(CUT_IS_TEST_SUITE(suite));

    cut_test_container_add_test(CUT_TEST_CONTAINER(suite), CUT_TEST(test_case));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
