#include <gcutter.h>
#include "cut-test.h"
#include "cut-test-result.h"
#include "cut-utils.h"
#include "cut-test-runner.h"
#include "lib/cuttest-assertions.h"

void test_equal_type(void);
void test_equal_value(void);
void test_equal_list_int(void);
void test_equal_list_uint(void);
void test_equal_list_string(void);
void test_equal_list_string_both_null(void);
void test_equal_list_string_other_null(void);
void test_equal_hash_string_string(void);
void test_error(void);

static CutTest *test;
static CutRunContext *run_context;
static CutTestContext *test_context;
static CutTestResult *test_result;

static GValue *value1, *value2;
static GList *list1, *list2;
static gboolean need_to_free_list_contents;
static GHashTable *hash1, *hash2;

static GError *error;
static gboolean need_to_free_error;

static gboolean
run (void)
{
    gboolean success;
    CutTestContext *original_test_context;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());

    test_context = cut_test_context_new(NULL, NULL, NULL, test);
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

    hash1 = NULL;
    hash2 = NULL;

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

    if (hash1)
        g_hash_table_destroy(hash1);
    if (hash2)
        g_hash_table_destroy(hash2);

    if (error && need_to_free_error)
        g_error_free(error);
}


static void
stub_equal_type (void)
{
    gcut_assert_equal_type(G_TYPE_INT, G_TYPE_INT);
    gcut_assert_equal_type(G_TYPE_INT, G_TYPE_STRING);
}

void
test_equal_type (void)
{
    test = cut_test_new("equal_type test", stub_equal_type);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_type test",
                           NULL,
                           "<G_TYPE_INT == G_TYPE_STRING>\n"
                           "expected: <gint>\n"
                           " but was: <gchararray>",
                           "stub_equal_type");
}


static void
stub_equal_value (void)
{
    g_value_init(value1, G_TYPE_INT);
    g_value_set_int(value1, 10);
    g_value_init(value2, G_TYPE_STRING);
    g_value_set_string(value2, "String");

    gcut_assert_equal_value(value1, value1);
    gcut_assert_equal_value(value2, value2);

    gcut_assert_equal_value(value1, value2);
}

void
test_equal_value (void)
{
    test = cut_test_new("equal_value test", stub_equal_value);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_value test",
                           NULL,
                           "<value1 == value2>\n"
                           "expected: <10> (gint)\n"
                           " but was: <\"String\"> (gchararray)",
                           "stub_equal_value");
}

static void
stub_equal_list_int (void)
{
    list1 = g_list_append(list1, GINT_TO_POINTER(100));
    list1 = g_list_append(list1, GINT_TO_POINTER(-200));
    list2 = g_list_append(list2, GINT_TO_POINTER(-1000));
    list2 = g_list_append(list2, GINT_TO_POINTER(2000));

    gcut_assert_equal_list_int(list1, list1);
    gcut_assert_equal_list_int(list2, list2);

    gcut_assert_equal_list_int(list1, list2);
}

void
test_equal_list_int (void)
{
    test = cut_test_new("equal_list_int test", stub_equal_list_int);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_list_int test",
                           NULL,
                           "<list1 == list2>\n"
                           "expected: <(100, -200)>\n"
                           " but was: <(-1000, 2000)>\n"
                           "\n"
                           "diff:\n"
                           "- (100, -200)\n"
                           "?      ^^\n"
                           "+ (-1000, 2000)\n"
                           "?  +   + ^   +",
                           "stub_equal_list_int");
}

static void
stub_equal_list_uint (void)
{
    list1 = g_list_append(list1, GUINT_TO_POINTER(100));
    list1 = g_list_append(list1, GUINT_TO_POINTER(200));
    list2 = g_list_append(list2, GUINT_TO_POINTER(1000));
    list2 = g_list_append(list2, GUINT_TO_POINTER(2000));

    gcut_assert_equal_list_uint(list1, list1);
    gcut_assert_equal_list_uint(list2, list2);

    gcut_assert_equal_list_uint(list1, list2);
}

void
test_equal_list_uint (void)
{
    test = cut_test_new("equal_list_uint test", stub_equal_list_uint);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_list_uint test",
                           NULL,
                           "<list1 == list2>\n"
                           "expected: <(100, 200)>\n"
                           " but was: <(1000, 2000)>\n"
                           "\n"
                           "diff:\n"
                           "- (100, 200)\n"
                           "+ (1000, 2000)\n"
                           "?     +     +",
                           "stub_equal_list_uint");
}

static void
stub_equal_list_string (void)
{
    need_to_free_list_contents = TRUE;

    list1 = g_list_append(list1, g_strdup("abc"));
    list1 = g_list_append(list1, g_strdup("def"));
    list2 = g_list_append(list2, g_strdup("zyx"));
    list2 = g_list_append(list2, g_strdup("wvu"));

    gcut_assert_equal_list_string(list1, list1);
    gcut_assert_equal_list_string(list2, list2);

    gcut_assert_equal_list_string(list1, list2);
}

void
test_equal_list_string (void)
{
    test = cut_test_new("equal_list_string test", stub_equal_list_string);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_list_string test",
                           NULL,
                           "<list1 == list2>\n"
                           "expected: <(\"abc\", \"def\")>\n"
                           " but was: <(\"zyx\", \"wvu\")>",
                           "stub_equal_list_string");
}

static void
stub_equal_list_string_both_null (void)
{
    need_to_free_list_contents = TRUE;

    list1 = g_list_append(list1, g_strdup("abc"));
    list1 = g_list_append(list1, NULL);
    list1 = g_list_append(list1, g_strdup("def"));
    list2 = g_list_append(list2, g_strdup("abc"));
    list2 = g_list_append(list2, NULL);
    list2 = g_list_append(list2, g_strdup("def"));

    gcut_assert_equal_list_string(list1, list1);
    gcut_assert_equal_list_string(list2, list2);

    gcut_assert_equal_list_string(list1, list2);
}

void
test_equal_list_string_both_null (void)
{
    test = cut_test_new("equal_list_string test (both NULL)",
                        stub_equal_list_string_both_null);
    cut_assert_not_null(test);

    cut_assert_true(run());
    cut_assert_test_result_summary(run_context, 0, 3, 1, 0, 0, 0, 0, 0);
}

static void
stub_equal_list_string_other_null (void)
{
    need_to_free_list_contents = TRUE;

    list1 = g_list_append(list1, g_strdup("abc"));
    list1 = g_list_append(list1, g_strdup("abc"));
    list1 = g_list_append(list1, g_strdup("def"));
    list2 = g_list_append(list2, NULL);
    list2 = g_list_append(list2, g_strdup("abc"));
    list2 = g_list_append(list2, g_strdup("def"));

    gcut_assert_equal_list_string(list1, list1);
    gcut_assert_equal_list_string(list2, list2);

    gcut_assert_equal_list_string(list1, list2);
}

void
test_equal_list_string_other_null (void)
{
    test = cut_test_new("equal_list_string test (other NULL)",
                        stub_equal_list_string_other_null);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_list_string test (other NULL)",
                           NULL,
                           "<list1 == list2>\n"
                           "expected: <(\"abc\", \"abc\", \"def\")>\n"
                           " but was: <(NULL, \"abc\", \"def\")>",
                           "stub_equal_list_string_other_null");
}

static void
stub_equal_hash_string_string (void)
{
    hash1 = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    hash2 = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    g_hash_table_insert(hash1, g_strdup("abc"), g_strdup("11"));
    g_hash_table_insert(hash1, g_strdup("def"), g_strdup("22"));
    g_hash_table_insert(hash2, g_strdup("zyx"), g_strdup("99"));
    g_hash_table_insert(hash2, g_strdup("wvu"), g_strdup("88"));

    gcut_assert_equal_hash_string_string(hash1, hash1);
    gcut_assert_equal_hash_string_string(hash2, hash2);

    gcut_assert_equal_hash_string_string(hash1, hash2);
}

void
test_equal_hash_string_string (void)
{
    test = cut_test_new("equal_hash_string_string test",
                        stub_equal_hash_string_string);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_hash_string_string test",
                           NULL,
                           "<hash1 == hash2>\n"
                           "expected: <{\"def\" => \"22\", \"abc\" => \"11\"}>\n"
                           " but was: <{\"zyx\" => \"99\", \"wvu\" => \"88\"}>",
                           "stub_equal_hash_string_string");
}

static void
stub_error (void)
{
    gcut_assert_error(error);

    error = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "not found");
    gcut_assert_error(error);
}

void
test_error (void)
{
    test = cut_test_new("gcut_assert_error test", stub_error);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 0, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "gcut_assert_error test",
                           NULL,
                           "expected: <error> is NULL\n"
                           " but was: <g-file-error-quark:4: not found>",
                           "stub_error");
}


/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
