#ifndef __CUTTEST_UTILS_H__
#define __CUTTEST_UTILS_H__

#include <glib.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-run-context.h>

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

#endif
