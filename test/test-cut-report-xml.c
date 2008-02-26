#include "cutter.h"
#include "cut-test-result.h"

void test_result_success_log (void);
void test_result_failure_log (void);
void test_result_penging_log (void);

static CutTestResult *result;
static gchar *log;

void
setup (void)
{
    result = NULL;
    log = NULL;
}

void
teardown (void)
{
    if (result)
        g_object_unref(result);
    if (log)
        g_free(log);
}

void
test_result_success_log (void)
{
    gchar expected[] = "<result>"
                       "  <status>success</status>"
                       "  <detail></detail>"
                       "  <elapsed>0.0001</elapsed>"
                       "</result>";
    result = cut_test_result_new(CUT_TEST_RESULT_SUCCESS,
                                 "test_my_name",
                                 "MyTestCase",
                                 "MyTestSuite",
                                 "",
                                 "",
                                 "",
                                 "",
                                 0);
    cut_assert(result);
    cut_assert_equal_string(expected, cut_test_result_to_xml(result));
}

void
test_result_failure_log (void)
{
    gchar expected[] = "<result>"
                       "  <status>failure</status>"
                       "  <detail>"
                       "&lt;\"1234\" == cut_test_get_metadata(CUT_TEST(tests-&gt;data), \"bug\")&gt;"
                       "expected: &lt;1234&gt;"
                       "but was: &lt;(null)&gt;"
                       "  </detail>"
                       "  <backtrace>"
                       "    <entry>"
                       "      <file>./test_get_metadata.c</file>"
                       "      <line>12</line>"
                       "      <info>test_get_bug_id()</info>"
                       "    </entry>"
                       "  </backtrace>"
                       "  <elapsed>0.002</elapsed>"
                       "</result>";
    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 "test_my_name",
                                 "MyTestCase",
                                 "MyTestSuite",
                                 "<\"1234\" == cut_test_get_metadata(CUT_TEST(tests->data), \"bug\")>;\n expected: <1234>;\n but was: <(null)>;",
                                 "<\"1234\" == cut_test_get_metadata(CUT_TEST(tests->data), \"bug\")>;\n expected: <1234>;\n but was: <(null)>;",
                                 "test_get_bug_id()",
                                 "./test_get_metadata.c",
                                 12);
    cut_assert(result);
    cut_assert_equal_string(expected, cut_test_result_to_xml(result));
}

void
test_result_penging_log (void)
{
    gchar expected[] = "<result>"
                       "  <status>penging</status>"
                       "  <detail>"
                       "Cannot set locale to de_DE, skipping"
                       "  </detail>"
                       "  <backtrace>"
                       "    <entry>"
                       "      <file>./option.c</file>"
                       "      <line>396</line>"
                       "      <info>test_arg_double_de_DE_locale()</info>"
                       "    </entry>"
                       "  </backtrace>"
                       "  <elapsed>0.001</elapsed>"
                       "</result>";
    result = cut_test_result_new(CUT_TEST_RESULT_FAILURE,
                                 "test_my_pending_test",
                                 "MyTestCase",
                                 "MyTestSuite",
                                 "Cannot set locale to de_DE, skipping",
                                 "Cannot set locale to de_DE, skipping",
                                 "test_arg_double_de_DE_locale()",
                                 "./option.c",
                                 396);
    cut_assert(result);
    cut_assert_equal_string(expected, cut_test_result_to_xml(result));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
