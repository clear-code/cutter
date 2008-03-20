#ifndef __CUTTEST_UTILS_H__
#define __CUTTEST_UTILS_H__

#include <glib.h>
#include <cutter/cut-test-case.h>

const gchar *cuttest_get_base_dir (void);
void         cuttest_add_test     (CutTestCase *test_case,
                                   const gchar *test_name,
                                   CutTestFunction test_function);

#endif
