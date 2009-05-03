/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <signal.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "cut-main.h"
#include "cut-test-result.h"
#include "cut-backtrace-entry.h"
#include "cut-crash-backtrace.h"

static gboolean cut_crash_backtrace_show_on_the_moment = TRUE;


void
cut_crash_backtrace_set_show_on_the_moment (gboolean show_on_the_moment)
{
    cut_crash_backtrace_show_on_the_moment = show_on_the_moment;
}

gboolean
cut_crash_backtrace_get_show_on_the_moment (void)
{
    return cut_crash_backtrace_show_on_the_moment;
}

#ifdef G_OS_WIN32

CutCrashBacktrace *
cut_crash_backtrace_new (jmp_buf *jump_buffer)
{
    return NULL;
}

void
cut_crash_backtrace_free (CutCrashBacktrace *crash_backtrace)
{
}

void
cut_crash_backtrace_emit (CutTestSuite    *test_suite,
                          CutTestCase     *test_case,
                          CutTest         *test,
                          CutTestIterator *test_iterator,
                          CutTestData     *test_data,
                          CutTestContext  *test_context)
{
}

#else

static CutCrashBacktrace *current_crash_backtrace = NULL;
static gchar *crash_backtrace = NULL;
static gchar crash_backtrace_read_buffer[4096];

struct _CutCrashBacktrace
{
    CutCrashBacktrace *previous;
    jmp_buf *jump_buffer;
    struct sigaction i_will_be_back_action;
    struct sigaction previous_segv_action;
    struct sigaction previous_abort_action;
    struct sigaction previous_int_action;
    gboolean set_segv_action;
    gboolean set_abort_action;
    gboolean set_int_action;
};

static void
read_backtrace (int in_fd)
{
    GRegex *regex;
    GString *buffer = NULL;
    gssize size;

    while ((size = read(in_fd,
                        crash_backtrace_read_buffer,
                        sizeof(crash_backtrace_read_buffer))) > 0) {
        if (cut_crash_backtrace_show_on_the_moment)
            write(STDERR_FILENO, crash_backtrace_read_buffer, size);

        if (!buffer)
            buffer = g_string_new("");
        g_string_append_len(buffer, crash_backtrace_read_buffer, size);
    }

    if (!buffer) {
        crash_backtrace = g_strdup("");
        return;
    }

    regex = g_regex_new("^.+?<signal handler called>\n",
                        G_REGEX_MULTILINE | G_REGEX_DOTALL,
                        G_REGEX_MATCH_NEWLINE_ANY, NULL);
    if (regex) {
        crash_backtrace = g_regex_replace(regex, buffer->str, buffer->len, 0,
                                              "", 0, NULL);
        g_regex_unref(regex);
        g_string_free(buffer, TRUE);
    } else {
        crash_backtrace = g_string_free(buffer, FALSE);
    }
}

static void
collect_backtrace (void)
{
    int fds[2];
    int original_stdout_fileno;

    if (pipe(fds) == -1) {
        perror("unable to open pipe for collecting stack trace");
        return;
    }

    original_stdout_fileno = dup(STDOUT_FILENO);
    dup2(fds[1], STDOUT_FILENO);
    g_on_error_stack_trace(cut_get_cutter_command_path());
    dup2(original_stdout_fileno, STDOUT_FILENO);
    close(original_stdout_fileno);
    close(fds[1]);

    read_backtrace(fds[0]);

    close(fds[0]);
}

static void
i_will_be_back_handler (int signum)
{
    jmp_buf *jump_buffer;
    jump_buffer = current_crash_backtrace->jump_buffer;

    cut_crash_backtrace_free(current_crash_backtrace);

    if (signum != SIGINT) {
        g_free(crash_backtrace);
        crash_backtrace = NULL;
        collect_backtrace();
    }
    longjmp(*jump_buffer, signum);
}

CutCrashBacktrace *
cut_crash_backtrace_new (jmp_buf *jump_buffer)
{
    CutCrashBacktrace *crash_backtrace;
    struct sigaction *i_will_be_back_action;
    struct sigaction *previous_segv_action;
    struct sigaction *previous_abort_action;
    struct sigaction *previous_int_action;

    crash_backtrace = g_new0(CutCrashBacktrace, 1);
    crash_backtrace->jump_buffer = jump_buffer;
    crash_backtrace->set_segv_action = TRUE;
    crash_backtrace->set_abort_action = TRUE;
    crash_backtrace->set_int_action = TRUE;

    i_will_be_back_action = &(crash_backtrace->i_will_be_back_action);
    previous_segv_action = &(crash_backtrace->previous_segv_action);
    previous_abort_action = &(crash_backtrace->previous_abort_action);
    previous_int_action = &(crash_backtrace->previous_int_action);

    i_will_be_back_action->sa_handler = i_will_be_back_handler;
    sigemptyset(&(i_will_be_back_action->sa_mask));
    i_will_be_back_action->sa_flags = 0;
    if (sigaction(SIGSEGV, i_will_be_back_action, previous_segv_action) == -1)
        crash_backtrace->set_segv_action = FALSE;
    if (sigaction(SIGABRT, i_will_be_back_action, previous_abort_action) == -1)
        crash_backtrace->set_abort_action = FALSE;
    if (sigaction(SIGINT, i_will_be_back_action, previous_int_action) == -1)
        crash_backtrace->set_int_action = FALSE;

    crash_backtrace->previous = current_crash_backtrace;
    current_crash_backtrace = crash_backtrace;

    return crash_backtrace;
}

void
cut_crash_backtrace_free (CutCrashBacktrace *crash_backtrace)
{
    if (crash_backtrace->set_int_action)
        sigaction(SIGINT, &(crash_backtrace->previous_int_action), NULL);
    if (crash_backtrace->set_abort_action)
        sigaction(SIGABRT, &(crash_backtrace->previous_abort_action), NULL);
    if (crash_backtrace->set_segv_action)
        sigaction(SIGSEGV, &(crash_backtrace->previous_segv_action), NULL);

    current_crash_backtrace = crash_backtrace->previous;

    g_free(crash_backtrace);
}

void
cut_crash_backtrace_emit (CutTestSuite    *test_suite,
                          CutTestCase     *test_case,
                          CutTest         *test,
                          CutTestIterator *test_iterator,
                          CutTestData     *test_data,
                          CutTestContext  *test_context)
{
    CutTestResult *result;
    /* GRegex *regex; */
    GList *parsed_backtrace = NULL;
    CutBacktraceEntry *entry;
    CutTest *target;

/*     regex = g_regex_new("^#\\d +0x\\d+ in ([a-zA-Z_]+) (.+)\n", */
/*                         G_REGEX_MULTILINE | G_REGEX_DOTALL, */
/*                         G_REGEX_MATCH_NEWLINE_ANY, NULL); */
/*     if (regex) { */
/*         parsed_backtrace */
/*         backtrace = g_regex_replace(regex, buffer->str, buffer->len, 0, */
/*                                     "", 0, NULL); */
/*         g_regex_unref(regex); */
/*         g_string_free(buffer, TRUE); */
/*     } */
    entry = cut_backtrace_entry_new(crash_backtrace, 0, "unknown", NULL);
    parsed_backtrace = g_list_append(parsed_backtrace, entry);

    g_free(crash_backtrace);
    crash_backtrace = NULL;

    result = cut_test_result_new(CUT_TEST_RESULT_CRASH,
                                 test, test_iterator, test_case, test_suite,
                                 test_data, NULL, NULL, parsed_backtrace);
    g_list_foreach(parsed_backtrace, (GFunc)g_object_unref, NULL);
    g_list_free(parsed_backtrace);

    if (test) {
        target = test;
    } else if (test_iterator) {
        target = CUT_TEST(test_iterator);
    } else if (test_case) {
        target = CUT_TEST(test_case);
    } else if (test_suite) {
        target = CUT_TEST(test_suite);
    } else {
        target = NULL;
    }
    cut_test_emit_result_signal(target, test_context, result);
    g_object_unref(result);
}

#endif


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
