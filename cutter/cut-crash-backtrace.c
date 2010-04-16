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
#include <string.h>

#include "cut-main.h"
#include "cut-test-result.h"
#include "cut-backtrace-entry.h"
#include "cut-utils.h"
#include "cut-crash-backtrace.h"

static gboolean cut_crash_backtrace_show_on_the_moment = TRUE;
static gboolean cut_crash_backtrace_signal_received = FALSE;


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

void
cut_crash_backtrace_reset_signal_received (void)
{
    cut_crash_backtrace_signal_received = FALSE;
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
static gchar crash_backtrace[40960];
static gchar crash_backtrace_read_buffer[4096];

struct _CutCrashBacktrace
{
    CutCrashBacktrace *previous;
    jmp_buf *jump_buffer;
    struct sigaction i_will_be_back_action;
    struct sigaction previous_segv_action;
    struct sigaction previous_abort_action;
    struct sigaction previous_terminate_action;
    struct sigaction previous_interrupt_action;
    gboolean set_segv_action;
    gboolean set_abort_action;
    gboolean set_terminate_action;
    gboolean set_interrupt_action;
};

static void
read_backtrace (int in_fd)
{
    gssize i, read_size;
    gchar *internal_backtrace;

    crash_backtrace[0] = '\0';
    i = 0;
    while ((read_size = read(in_fd,
                             crash_backtrace_read_buffer,
                             sizeof(crash_backtrace_read_buffer))) > 0) {
        if (cut_crash_backtrace_show_on_the_moment)
            write(STDERR_FILENO, crash_backtrace_read_buffer, read_size);

        if (i + read_size > sizeof(crash_backtrace))
            break;
        memcpy(crash_backtrace + i, crash_backtrace_read_buffer, read_size);
        i += read_size;
    }
    crash_backtrace[i] = '\0';
    if (i == 0)
        return;

#define INTERNAL_BACKTRACE_MARK "<signal handler called>\n"
    internal_backtrace = strstr(crash_backtrace, INTERNAL_BACKTRACE_MARK);
    if (internal_backtrace) {
        internal_backtrace += strlen(INTERNAL_BACKTRACE_MARK);
        crash_backtrace[internal_backtrace - crash_backtrace] = '\0';
    }
#undef INTERNAL_BACKTRACE_MARK
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

    if (cut_crash_backtrace_signal_received) {
        g_print("signal received on crash\n");
        exit(EXIT_FAILURE);
    }
    cut_crash_backtrace_signal_received = TRUE;

    jump_buffer = current_crash_backtrace->jump_buffer;

    cut_crash_backtrace_free(current_crash_backtrace);

    if (signum != SIGINT) {
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
    struct sigaction *previous_terminate_action;
    struct sigaction *previous_interrupt_action;

    crash_backtrace = g_new0(CutCrashBacktrace, 1);
    crash_backtrace->jump_buffer = jump_buffer;
    crash_backtrace->set_segv_action = TRUE;
    crash_backtrace->set_abort_action = TRUE;
    crash_backtrace->set_terminate_action = TRUE;
    crash_backtrace->set_interrupt_action = TRUE;

    i_will_be_back_action = &(crash_backtrace->i_will_be_back_action);
    previous_segv_action = &(crash_backtrace->previous_segv_action);
    previous_abort_action = &(crash_backtrace->previous_abort_action);
    previous_terminate_action = &(crash_backtrace->previous_terminate_action);
    previous_interrupt_action = &(crash_backtrace->previous_interrupt_action);

    i_will_be_back_action->sa_handler = i_will_be_back_handler;
    sigemptyset(&(i_will_be_back_action->sa_mask));
    i_will_be_back_action->sa_flags = 0;
    if (sigaction(SIGSEGV, i_will_be_back_action, previous_segv_action) == -1)
        crash_backtrace->set_segv_action = FALSE;
    if (sigaction(SIGABRT, i_will_be_back_action, previous_abort_action) == -1)
        crash_backtrace->set_abort_action = FALSE;
    if (sigaction(SIGTERM, i_will_be_back_action,
                  previous_terminate_action) == -1)
        crash_backtrace->set_terminate_action = FALSE;
    if (sigaction(SIGINT, i_will_be_back_action,
                  previous_interrupt_action) == -1)
        crash_backtrace->set_interrupt_action = FALSE;

    crash_backtrace->previous = current_crash_backtrace;
    current_crash_backtrace = crash_backtrace;

    return crash_backtrace;
}

void
cut_crash_backtrace_free (CutCrashBacktrace *crash_backtrace)
{
    if (crash_backtrace->set_interrupt_action)
        sigaction(SIGINT, &(crash_backtrace->previous_interrupt_action), NULL);
    if (crash_backtrace->set_terminate_action)
        sigaction(SIGTERM, &(crash_backtrace->previous_terminate_action), NULL);
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
    GList *parsed_backtrace = NULL;
    CutTest *target;

    parsed_backtrace = cut_utils_parse_gdb_backtrace(crash_backtrace);
    crash_backtrace[0] = '\0';

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
