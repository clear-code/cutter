/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2012-2013  Kouhei Sutou <kou@clear-code.com>
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
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <gcutter.h>

#include <cutter/cut-logger.h>
#include <cutter/cut-enum-types.h>

#define CUT_LOG_DOMAIN "logger-test"

void data_level_from_string (void);
void test_level_from_string (gconstpointer data);
void test_error (void);
void test_critical (void);
void test_message (void);
void test_warning (void);
void test_debug (void);
void test_info (void);
void test_console_output (void);
void test_target_level (void);
void test_interesting_level (void);

static CutLogger *logger;

static const gchar *logged_domain;
static CutLogLevelFlags logged_level;
static const gchar *logged_message;
static const gchar *logged_file;
static guint logged_line;
static const gchar *logged_function;
static GString *stdout_string;
static gboolean end_of_log;

static CutLogLevelFlags original_log_level;
static GPrintFunc original_print_hander;

static gboolean default_handler_disconnected;

#ifdef __clang__
#  define FUNCTION(name) cut_take_string(g_strconcat("void ", name, "(void)", NULL))
#else
#  define FUNCTION(name) name
#endif

static void
cb_log (CutLogger *logger,
        const gchar *domain, CutLogLevelFlags level,
        const gchar *file, guint line, const gchar *function,
        GTimeVal *time_value, const gchar *message,
        gpointer user_data)
{
    logged_domain = cut_take_strdup(domain);
    logged_level = level;
    logged_file = cut_take_strdup(file);
    logged_line = line;
    logged_function = cut_take_strdup(FUNCTION(function));

    logged_message = cut_take_strdup(message);
}


void
setup (void)
{
    logger = NULL;

    logged_domain = NULL;
    logged_level = 0;
    logged_message = NULL;

    stdout_string = g_string_new(NULL);
    end_of_log = FALSE;

    original_print_hander = NULL;

    original_log_level = cut_get_log_level();
    cut_set_log_level(CUT_LOG_LEVEL_ALL);
    default_handler_disconnected = FALSE;

    g_signal_connect(cut_logger(), "log", G_CALLBACK(cb_log), NULL);
}

void
teardown (void)
{
    if (logger)
        g_object_unref(logger);

    cut_set_log_level(original_log_level);
    if (default_handler_disconnected)
        cut_logger_connect_default_handler(cut_logger());

    if (original_print_hander)
        g_set_print_handler(original_print_hander);

    g_signal_handlers_disconnect_by_func(cut_logger(),
                                         G_CALLBACK(cb_log), NULL);
    g_string_free(stdout_string, TRUE);
}

void
data_level_from_string (void)
{
#define ADD(label, expected, level_string)                              \
    gcut_add_datum(label,                                               \
                   "/expected", CUT_TYPE_LOG_LEVEL_FLAGS, expected,  \
                   "/level-string", G_TYPE_STRING, level_string,        \
                   NULL)

    ADD("all", CUT_LOG_LEVEL_ALL, "all");
    ADD("one", CUT_LOG_LEVEL_INFO, "info");
    ADD("multi",
        CUT_LOG_LEVEL_ERROR | CUT_LOG_LEVEL_CRITICAL,
        "error|critical");
    ADD("append",
        CUT_LOG_LEVEL_CRITICAL |
        CUT_LOG_LEVEL_ERROR |
        CUT_LOG_LEVEL_WARNING |
        CUT_LOG_LEVEL_MESSAGE |
        CUT_LOG_LEVEL_INFO |
        CUT_LOG_LEVEL_DEBUG,
        "+info|debug");
    ADD("remove",
        CUT_LOG_LEVEL_CRITICAL |
        CUT_LOG_LEVEL_MESSAGE,
        "-error|warning");

#undef ADD
}

void
test_level_from_string (gconstpointer data)
{
    const gchar *level_string;
    CutLogLevelFlags expected, actual;
    GError *error = NULL;

    expected = gcut_data_get_flags(data, "/expected");
    level_string = gcut_data_get_string(data, "/level-string");
    actual = cut_log_level_flags_from_string(level_string,
                                             CUT_LOG_LEVEL_DEFAULT,
                                             &error);
    gcut_assert_error(error);
    gcut_assert_equal_flags(CUT_TYPE_LOG_LEVEL_FLAGS,
                            expected,
                            actual);
}

#define cut_assert_equal_log(level, message)                       \
    cut_assert_equal_string(CUT_LOG_DOMAIN, logged_domain);        \
    gcut_assert_equal_flags(CUT_TYPE_LOG_LEVEL_FLAGS,              \
                            level,                                 \
                            logged_level);                         \
    cut_assert_equal_string(__FILE__, logged_file);                \
    cut_assert_equal_string(__PRETTY_FUNCTION__, logged_function); \
    cut_assert_equal_uint(line, logged_line);                      \
    cut_assert_equal_string(message, logged_message);

static void
disconnect_default_handler (void)
{
    cut_logger_disconnect_default_handler(cut_logger());
    default_handler_disconnected = TRUE;
}

void
test_error (void)
{
    guint line;

    disconnect_default_handler();
    cut_log_error("error"); line = __LINE__;
    cut_assert_equal_log(CUT_LOG_LEVEL_ERROR, "error");
}

void
test_critical (void)
{
    guint line;

    disconnect_default_handler();
    cut_log_critical("critical"); line = __LINE__;
    cut_assert_equal_log(CUT_LOG_LEVEL_CRITICAL, "critical");
}

void
test_message (void)
{
    guint line;

    disconnect_default_handler();
    cut_log_message("message"); line = __LINE__;
    cut_assert_equal_log(CUT_LOG_LEVEL_MESSAGE, "message");
}

void
test_warning (void)
{
    guint line;

    disconnect_default_handler();
    cut_log_warning("warning"); line = __LINE__;
    cut_assert_equal_log(CUT_LOG_LEVEL_WARNING, "warning");
}

void
test_debug (void)
{
    guint line;

    disconnect_default_handler();
    cut_log_debug("debug"); line = __LINE__;
    cut_assert_equal_log(CUT_LOG_LEVEL_DEBUG, "debug");
}

void
test_info (void)
{
    guint line;

    disconnect_default_handler();
    cut_log_info("info"); line = __LINE__;
    cut_assert_equal_log(CUT_LOG_LEVEL_INFO, "info");
}

static void
print_handler (const gchar *string)
{
    if (g_strrstr(string, "end-of-log"))
        end_of_log = TRUE;
    g_string_append(stdout_string, string);
}

void
test_console_output (void)
{
    original_print_hander = g_set_print_handler(print_handler);
    cut_set_log_level(CUT_LOG_LEVEL_INFO);
    cut_log_info("info");
    cut_log_info("end-of-log");
    g_set_print_handler(original_print_hander);
    original_print_hander = NULL;

    cut_assert_match("info", stdout_string->str);
}

void
test_target_level (void)
{
    cut_set_log_level(CUT_LOG_LEVEL_DEFAULT);

    logger = cut_logger_new();
    g_signal_connect(logger, "log",
                     G_CALLBACK(cut_logger_default_log_handler), NULL);

    original_print_hander = g_set_print_handler(print_handler);
    cut_logger_log(logger, "domain", CUT_LOG_LEVEL_INFO,
                   "file", 29, "function",
                   "message");
    g_set_print_handler(original_print_hander);
    original_print_hander = NULL;
    cut_assert_equal_string("", stdout_string->str);

    cut_logger_set_target_level(logger, CUT_LOG_LEVEL_INFO);
    original_print_hander = g_set_print_handler(print_handler);
    cut_logger_log(logger, "domain", CUT_LOG_LEVEL_INFO,
                   "file", 29, "function",
                   "message");
    g_set_print_handler(original_print_hander);
    original_print_hander = NULL;
    cut_assert_match("message", stdout_string->str);
}

void
test_interesting_level (void)
{
    logger = cut_logger_new();
    gcut_assert_equal_flags(CUT_TYPE_LOG_LEVEL_FLAGS,
                            CUT_LOG_LEVEL_MESSAGE |
                            CUT_LOG_LEVEL_WARNING |
                            CUT_LOG_LEVEL_ERROR |
                            CUT_LOG_LEVEL_CRITICAL,
                            cut_logger_get_interesting_level(logger));

    cut_logger_set_target_level(logger,
                                CUT_LOG_LEVEL_INFO | CUT_LOG_LEVEL_TRACE);
    gcut_assert_equal_flags(CUT_TYPE_LOG_LEVEL_FLAGS,
                            CUT_LOG_LEVEL_INFO | CUT_LOG_LEVEL_TRACE,
                            cut_logger_get_interesting_level(logger));

    cut_logger_set_interesting_level(logger,
                                     "syslog",
                                     CUT_LOG_LEVEL_DEBUG);
    gcut_assert_equal_flags(CUT_TYPE_LOG_LEVEL_FLAGS,
                            CUT_LOG_LEVEL_INFO |
                            CUT_LOG_LEVEL_DEBUG |
                            CUT_LOG_LEVEL_TRACE,
                            cut_logger_get_interesting_level(logger));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
