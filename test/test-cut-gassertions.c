#include <gcutter.h>
#include "cut-test.h"
#include "cut-test-result.h"
#include "cut-utils.h"
#include "cut-test-runner.h"
#include "cuttest-assertions.h"

void test_equal_g_type(void);
void test_equal_g_value(void);
void test_equal_g_list_int(void);
void test_equal_g_list_uint(void);
void test_equal_g_list_string(void);
void test_equal_g_list_string_both_null(void);
void test_equal_g_list_string_other_null(void);
void test_g_error(void);

static CutTest *test;
static CutRunContext *run_context;
static CutTestContext *test_context;
static CutTestResult *test_result;

static GValue *value1, *value2;
static GList *list1, *list2;
static gboolean need_to_free_list_contents;

static GError *error;
static gboolean need_to_free_error;

static gboolean
run (CutTest *_test)
{
    gboolean success;
    CutTestContext *original_test_context;

    test = _test;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());

    test_context = cut_test_context_new(NULL, NULL, test);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, run_context);
    set_current_test_context(original_test_context);

    return success;
}

void
setup (void)
{
    test = NULL;
    run_context = NULL;
    test_context = NULL;
    test_result = NULL;

    value1 = g_new0(GValue, 1);
    value2 = g_new0(GValue, 1);

    list1 = NULL;
    list2 = NULL;
    need_to_free_list_contents = FALSE;

    error = NULL;
    need_to_free_error = FALSE;
}

void
teardown (void)
{
    if (test)
        g_object_unref(test);
    if (run_context)
        g_object_unref(run_context);
    if (test_context)
        g_object_unref(test_context);
    if (test_result)
        g_object_unref(test_result);

    if (G_IS_VALUE(value1))
        g_value_unset(value1);
    if (G_IS_VALUE(value2))
        g_value_unset(value2);

    g_free(value1);
    g_free(value2);

    if (need_to_free_list_contents) {
        g_list_foreach(list1, (GFunc)g_free, NULL);
        g_list_foreach(list2, (GFunc)g_free, NULL);
    }
    g_list_free(list1);
    g_list_free(list2);

    if (error && need_to_free_error)
        g_error_free(error);
}


static void
equal_g_type_test (void)
{
    cut_assert_equal_g_type(G_TYPE_INT, G_TYPE_INT);
    cut_assert_equal_g_type(G_TYPE_INT, G_TYPE_STRING);
}

void
test_equal_g_type (void)
{
    CutTest *test;

    test = cut_test_new("equal_g_type test", equal_g_type_test);
    cut_assert(test);

    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 0, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_g_type test",
                           NULL,
                           "<G_TYPE_INT == G_TYPE_STRING>\n"
                           "expected: <gint>\n"
                           " but was: <gchararray>",
                           "equal_g_type_test");
}


static void
equal_g_value_test (void)
{
    g_value_init(value1, G_TYPE_INT);
    g_value_set_int(value1, 10);
    g_value_init(value2, G_TYPE_STRING);
    g_value_set_string(value2, "String");

    cut_assert_equal_g_value(value1, value1);
    cut_assert_equal_g_value(value2, value2);

    cut_assert_equal_g_value(value1, value2);
}

void
test_equal_g_value (void)
{
    CutTest *test;

    test = cut_test_new("equal_g_value test", equal_g_value_test);
    cut_assert(test);

    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_g_value test",
                           NULL,
                           "<value1 == value2>\n"
                           "expected: <10> (gint)\n"
                           " but was: <\"String\"> (gchararray)",
                           "equal_g_value_test");
}

static void
equal_g_list_int_test (void)
{
    list1 = g_list_append(list1, GINT_TO_POINTER(100));
    list1 = g_list_append(list1, GINT_TO_POINTER(-200));
    list2 = g_list_append(list2, GINT_TO_POINTER(-1000));
    list2 = g_list_append(list2, GINT_TO_POINTER(2000));

    cut_assert_equal_g_list_int(list1, list1);
    cut_assert_equal_g_list_int(list2, list2);

    cut_assert_equal_g_list_int(list1, list2);
}

void
test_equal_g_list_int (void)
{
    CutTest *test;

    test = cut_test_new("equal_g_list_int test", equal_g_list_int_test);
    cut_assert(test);

    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_g_list_int test",
                           NULL,
                           "<list1 == list2>\n"
                           "expected: <(100, -200)>\n"
                           " but was: <(-1000, 2000)>",
                           "equal_g_list_int_test");
}

static void
equal_g_list_uint_test (void)
{
    list1 = g_list_append(list1, GUINT_TO_POINTER(100));
    list1 = g_list_append(list1, GUINT_TO_POINTER(200));
    list2 = g_list_append(list2, GUINT_TO_POINTER(1000));
    list2 = g_list_append(list2, GUINT_TO_POINTER(2000));

    cut_assert_equal_g_list_uint(list1, list1);
    cut_assert_equal_g_list_uint(list2, list2);

    cut_assert_equal_g_list_uint(list1, list2);
}

void
test_equal_g_list_uint (void)
{
    CutTest *test;

    test = cut_test_new("equal_g_list_uint test", equal_g_list_uint_test);
    cut_assert(test);

    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_g_list_uint test",
                           NULL,
                           "<list1 == list2>\n"
                           "expected: <(100, 200)>\n"
                           " but was: <(1000, 2000)>",
                           "equal_g_list_uint_test");
}

static void
stub_equal_g_list_string (void)
{
    need_to_free_list_contents = TRUE;

    list1 = g_list_append(list1, g_strdup("abc"));
    list1 = g_list_append(list1, g_strdup("def"));
    list2 = g_list_append(list2, g_strdup("zyx"));
    list2 = g_list_append(list2, g_strdup("wvu"));

    cut_assert_equal_g_list_string(list1, list1);
    cut_assert_equal_g_list_string(list2, list2);

    cut_assert_equal_g_list_string(list1, list2);
}

void
test_equal_g_list_string (void)
{
    CutTest *test;

    test = cut_test_new("equal_g_list_string test", stub_equal_g_list_string);
    cut_assert(test);

    cut_assert_false(run(test));
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_g_list_string test",
                           NULL,
                           "<list1 == list2>\n"
                           "expected: <(\"abc\", \"def\")>\n"
                           " but was: <(\"zyx\", \"wvu\")>",
                           "stub_equal_g_list_string");
}

static void
stub_equal_g_list_string_both_null (void)
{
    need_to_free_list_contents = TRUE;

    list1 = g_list_append(list1, g_strdup("abc"));
    list1 = g_list_append(list1, NULL);
    list1 = g_list_append(list1, g_strdup("def"));
    list2 = g_list_append(list2, g_strdup("abc"));
    list2 = g_list_append(list2, NULL);
    list2 = g_list_append(list2, g_strdup("def"));

    cut_assert_equal_g_list_string(list1, list1);
    cut_assert_equal_g_list_string(list2, list2);

    cut_assert_equal_g_list_string(list1, list2);
}

void
test_equal_g_list_string_both_null (void)
{
    CutTest *test;

    test = cut_test_new("equal_g_list_string test (both NULL)",
                        stub_equal_g_list_string_both_null);
    cut_assert(test);

    cut_assert_true(run(test));
    cut_assert_test_result_summary(run_context, 0, 3, 1, 0, 0, 0, 0, 0);
}

static void
stub_equal_g_list_string_other_null (void)
{
    need_to_free_list_contents = TRUE;

    list1 = g_list_append(list1, g_strdup("abc"));
    list1 = g_list_append(list1, g_strdup("abc"));
    list1 = g_list_append(list1, g_strdup("def"));
    list2 = g_list_append(list2, NULL);
    list2 = g_list_append(list2, g_strdup("abc"));
    list2 = g_list_append(list2, g_strdup("def"));

    cut_assert_equal_g_list_string(list1, list1);
    cut_assert_equal_g_list_string(list2, list2);

    cut_assert_equal_g_list_string(list1, list2);
}

void
test_equal_g_list_string_other_null (void)
{
    CutTest *test;

    test = cut_test_new("equal_g_list_string test (other NULL)",
                        stub_equal_g_list_string_other_null);
    cut_assert(test);

    cut_assert_false(run(test));
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_g_list_string test (other NULL)",
                           NULL,
                           "<list1 == list2>\n"
                           "expected: <(\"abc\", \"abc\", \"def\")>\n"
                           " but was: <(NULL, \"abc\", \"def\")>",
                           "stub_equal_g_list_string_other_null");
}

static void
g_error_test (void)
{
    cut_assert_g_error(error);

    error = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "not found");
    cut_assert_g_error(error);
}

void
test_g_error (void)
{
    CutTest *test;

    test = cut_test_new("cut_assert_g_error test", g_error_test);
    cut_assert(test);

    cut_assert(!run(test));
    cut_assert_test_result_summary(run_context, 0, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_g_error test",
                           NULL,
                           "expected: <error> is NULL\n"
                           " but was: <g-file-error-quark:4: not found>",
                           "g_error_test");
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
