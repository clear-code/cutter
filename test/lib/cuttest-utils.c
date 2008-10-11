#include "cuttest-utils.h"
#include <cutter/cut-backtrace-entry.h>

const gchar *
cuttest_get_base_dir(void)
{
    const gchar *dir;

    dir = g_getenv("BASE_DIR");
    return dir ? dir : ".";
}

void
cuttest_add_test (CutTestCase *test_case, const gchar *test_name,
                  CutTestFunction test_function)
{
    CutTest *test;

    test = cut_test_new(test_name, test_function);
    cut_test_case_add_test(test_case, test);
    g_object_unref(test);
}

GList *
cuttest_result_summary_list_new (guint n_tests,
                                 guint n_assertions,
                                 guint n_successes,
                                 guint n_failures,
                                 guint n_errors,
                                 guint n_pendings,
                                 guint n_notifications,
                                 guint n_omissions)
{
    GList *list = NULL;

#define APPEND(uint_value)                                      \
    list = g_list_append(list, GUINT_TO_POINTER(uint_value));   \

    APPEND(n_tests);
    APPEND(n_assertions);
    APPEND(n_successes);
    APPEND(n_failures);
    APPEND(n_errors);
    APPEND(n_pendings);
    APPEND(n_notifications);
    APPEND(n_omissions);

#undef APPEND

    return list;
}

GList *
cuttest_result_summary_list_new_from_run_context (CutRunContext *run_context)
{
    return cuttest_result_summary_list_new(
        cut_run_context_get_n_tests(run_context),
        cut_run_context_get_n_assertions(run_context),
        cut_run_context_get_n_successes(run_context),
        cut_run_context_get_n_failures(run_context),
        cut_run_context_get_n_errors(run_context),
        cut_run_context_get_n_pendings(run_context),
        cut_run_context_get_n_notifications(run_context),
        cut_run_context_get_n_omissions(run_context));
}

GList *
cuttest_result_string_list_new_va_list (const gchar *test_name,
                                        const gchar *user_message,
                                        const gchar *system_message,
                                        const gchar *message,
                                        const gchar *backtrace,
                                        va_list args)
{
    GList *strings = NULL;
    const gchar *file_line, *function;

#define APPEND(value)                           \
    strings = g_list_append(strings, g_strdup(value))

    APPEND(test_name);
    APPEND(user_message);
    APPEND(system_message);
    if (message) {
        APPEND(message);
    } else if (user_message && system_message) {
        gchar *computed_message;

        computed_message = g_strdup_printf("%s\n%s",
                                           user_message, system_message);
        APPEND(computed_message);
        g_free(computed_message);
    } else if (user_message) {
        APPEND(user_message);
    } else if (system_message) {
        APPEND(system_message);
    } else {
        APPEND(NULL);
    }

    file_line = backtrace;
    while (file_line) {
        function = va_arg(args, const gchar *);
        APPEND(file_line);
        APPEND(function);
        file_line = va_arg(args, const gchar *);
    }

#undef APPEND

    return strings;
}

GList *
cuttest_result_string_list_new (const gchar *test_name,
                                const gchar *user_message,
                                const gchar *system_message,
                                        const gchar *message,
                                const gchar *backtrace,
                                ...)
{
    GList *result_list;
    va_list args;

    va_start(args, backtrace);
    result_list = cuttest_result_string_list_new_va_list(test_name,
                                                         user_message,
                                                         system_message,
                                                         message,
                                                         backtrace,
                                                         args);
    va_end(args);

    return result_list;
}

GList *
cuttest_result_string_list_new_from_result (CutTestResult *result)
{
    GList *result_list;
    const GList *backtrace, *node;

    result_list = cuttest_result_string_list_new(
        cut_test_result_get_test_name(result),
        cut_test_result_get_user_message(result),
        cut_test_result_get_system_message(result),
        cut_test_result_get_message(result),
        NULL);

    backtrace = cut_test_result_get_backtrace(result);
    for (node = backtrace; node; node = g_list_next(node)) {
        CutBacktraceEntry *entry = backtrace->data;
        const gchar *file, *function_name;
        guint line;

        file = cut_backtrace_entry_get_file(entry);
        line = cut_backtrace_entry_get_line(entry);
        function_name = cut_backtrace_entry_get_function(entry);
        result_list = g_list_append(result_list, g_strdup_printf("%s:%u",
                                                                 file, line));
        result_list = g_list_append(result_list, g_strdup(function_name));
    }

    return result_list;
}
