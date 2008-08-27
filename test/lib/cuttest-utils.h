#ifndef __CUTTEST_UTILS_H__
#define __CUTTEST_UTILS_H__

#include <glib.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-run-context.h>
#include <cutter/cut-test-result.h>

#define ISO8601_PATTERN_WITHOUT_YEAR                    \
    "\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}(?:\\.\\d+)?Z"
#define ISO8601_PATTERN "\\d{4}-" ISO8601_PATTERN_WITHOUT_YEAR

#define CUTTEST_TEST_DIR_KEY "CUTTEST_TEST_DIR"

const gchar *cuttest_get_base_dir (void);
void         cuttest_add_test     (CutTestCase *test_case,
                                   const gchar *test_name,
                                   CutTestFunction test_function);
GList       *cuttest_result_summary_list_new
                                  (guint n_tests,
                                   guint n_assertions,
                                   guint n_successes,
                                   guint n_failures,
                                   guint n_errors,
                                   guint n_pendings,
                                   guint n_notifications,
                                   guint n_omissions);
GList       *cuttest_result_summary_list_new_from_run_context
                                  (CutRunContext *run_context);

GList       *cuttest_result_string_list_new
                                  (const gchar *test_name,
                                   const gchar *user_message,
                                   const gchar *system_message,
                                   const gchar *function_name);
GList       *cuttest_result_string_list_new_from_result
                                  (CutTestResult *result);

#endif
