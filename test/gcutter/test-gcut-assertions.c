/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009  Kouhei Sutou <kou@clear-code.com>
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

#include <gcutter.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-result.h>
#include <cutter/cut-utils.h>
#include <cutter/cut-test-runner.h>
#include <cutter/cut-repository.h>
#include <cutter/cut-enum-types.h>
#include <cutter/cut-loader.h>
#include "../lib/cuttest-assertions.h"

void test_equal_type(void);
void test_equal_value(void);
void test_equal_list(void);
void test_equal_list_int(void);
void test_equal_list_uint(void);
void test_equal_list_string(void);
void test_equal_list_string_both_null(void);
void test_equal_list_string_other_null(void);
void test_equal_list_enum(void);
void test_equal_list_flags(void);
void test_equal_list_object(void);
void test_equal_hash_table(void);
void test_equal_hash_table_string_string(void);
void test_error(void);
void test_equal_error(void);
void test_equal_enum(void);
void test_equal_flags(void);
void test_equal_object(void);
void test_equal_object_null(void);
void test_equal_object_custom(void);
void test_equal_pid(void);
void test_not_equal_pid(void);

static CutTest *test;
static CutRunContext *run_context;
static CutTestContext *test_context;
static CutTestResult *test_result;

static GValue *value1, *value2;
static GList *list1, *list2;
static gboolean need_to_free_list_contents;
static GDestroyNotify list_element_free_function;
static GHashTable *hash1, *hash2;
static GObject *object1, *object2, *object3;

static GError *error;
static GError *error1, *error2;
static gboolean need_to_free_error;

static GType flags_type = 0;

static gint fail_line;

#define MARK_FAIL(assertion) do                 \
{                                               \
    fail_line = __LINE__;                       \
    assertion;                                  \
} while (0)

#define FAIL_LOCATION (cut_take_printf("%s:%d", __FILE__, fail_line))

static gboolean
run (void)
{
    gboolean success;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());

    test_context = cut_test_context_new(run_context, NULL, NULL, NULL, test);
    cut_test_context_current_push(test_context);
    success = cut_test_runner_run_test(CUT_TEST_RUNNER(run_context),
                                       test, test_context);
    cut_test_context_current_pop();

    return success;
}

void
cut_setup (void)
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
    list_element_free_function = g_free;

    hash1 = NULL;
    hash2 = NULL;

    object1 = NULL;
    object2 = NULL;
    object3 = NULL;

    error = NULL;
    need_to_free_error = FALSE;

    error1 = NULL;
    error2 = NULL;

    flags_type = 0;

    fail_line = 0;
}

void
cut_teardown (void)
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
        g_list_foreach(list1, (GFunc)list_element_free_function, NULL);
        g_list_foreach(list2, (GFunc)list_element_free_function, NULL);
    }
    g_list_free(list1);
    g_list_free(list2);

    if (hash1)
        g_hash_table_unref(hash1);
    if (hash2)
        g_hash_table_unref(hash2);

    if (object1)
        g_object_unref(object1);
    if (object2)
        g_object_unref(object2);
    if (object3)
        g_object_unref(object3);

    if (error && need_to_free_error)
        g_error_free(error);

    if (error1)
        g_error_free(error1);
    if (error2)
        g_error_free(error2);
}


static void
stub_equal_type (void)
{
    gcut_assert_equal_type(G_TYPE_INT, G_TYPE_INT);
    MARK_FAIL(gcut_assert_equal_type(G_TYPE_INT, G_TYPE_STRING));
}

void
test_equal_type (void)
{
    test = cut_test_new("equal_type test", stub_equal_type);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_type test",
                           NULL,
                           "<G_TYPE_INT == G_TYPE_STRING>",
                           "gint", "gchararray",
                           FAIL_LOCATION, "stub_equal_type",
                           NULL);
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

    MARK_FAIL(gcut_assert_equal_value(value1, value2));
}

void
test_equal_value (void)
{
    test = cut_test_new("equal_value test", stub_equal_value);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_value test",
                           NULL,
                           "<value1 == value2>",
                           "10 (gint)",
                           "\"String\" (gchararray)",
                           FAIL_LOCATION, "stub_equal_value",
                           NULL);
}

static gboolean
stub_equal_list_equal_func (gconstpointer a, gconstpointer b)
{
    return GPOINTER_TO_INT(a) == GPOINTER_TO_INT(b);
}

static void
stub_equal_list_inspect_func (GString *string,
                              gconstpointer data, gpointer user_data)
{
    gchar *prefix = user_data;

    g_string_append_printf(string, "%s: <%d>", prefix, GPOINTER_TO_INT(data));
}

static void
stub_equal_list (void)
{
    list1 = g_list_append(list1, GINT_TO_POINTER(100));
    list1 = g_list_append(list1, GINT_TO_POINTER(-200));
    list2 = g_list_append(list2, GINT_TO_POINTER(-1000));
    list2 = g_list_append(list2, GINT_TO_POINTER(2000));

    gcut_assert_equal_list(list1, list1,
                           stub_equal_list_equal_func,
                           stub_equal_list_inspect_func, "AAA");
    gcut_assert_equal_list(list2, list2,
                           stub_equal_list_equal_func,
                           stub_equal_list_inspect_func, "BBB");

    MARK_FAIL(gcut_assert_equal_list(list1, list2,
                                     stub_equal_list_equal_func,
                                     stub_equal_list_inspect_func, "CCC"));
}

void
test_equal_list (void)
{
    test = cut_test_new("equal_list test", stub_equal_list);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(
        run_context, 0, CUT_TEST_RESULT_FAILURE,
        "equal_list test",
        NULL,
        "<stub_equal_list_equal_func(list1[i], list2[i]) == TRUE>",
        "(CCC: <100>, CCC: <-200>)",
        "(CCC: <-1000>, CCC: <2000>)",
        FAIL_LOCATION, "stub_equal_list",
        NULL);
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

    MARK_FAIL(gcut_assert_equal_list_int(list1, list2));
}

void
test_equal_list_int (void)
{
    test = cut_test_new("equal_list_int test", stub_equal_list_int);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_list_int test",
                           NULL,
                           "<list1 == list2>\n"
                           "expected: <(100, -200)>\n"
                           "  actual: <(-1000, 2000)>\n"
                           "\n"
                           "diff:\n"
                           "- (100, -200)\n"
                           "?      ^^\n"
                           "+ (-1000, 2000)\n"
                           "?  +   + ^   +",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_list_int",
                           NULL);
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

    MARK_FAIL(gcut_assert_equal_list_uint(list1, list2));
}

void
test_equal_list_uint (void)
{
    test = cut_test_new("equal_list_uint test", stub_equal_list_uint);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_list_uint test",
                           NULL,
                           "<list1 == list2>\n"
                           "expected: <(100, 200)>\n"
                           "  actual: <(1000, 2000)>\n"
                           "\n"
                           "diff:\n"
                           "- (100, 200)\n"
                           "+ (1000, 2000)\n"
                           "?     +     +",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_list_uint",
                           NULL);
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

    MARK_FAIL(gcut_assert_equal_list_string(list1, list2));
}

void
test_equal_list_string (void)
{
    test = cut_test_new("equal_list_string test", stub_equal_list_string);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_list_string test",
                           NULL,
                           "<list1 == list2>",
                           "(\"abc\", \"def\")",
                           "(\"zyx\", \"wvu\")",
                           FAIL_LOCATION, "stub_equal_list_string",
                           NULL);
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
    cut_assert_test_result_summary(run_context, 1, 3, 1, 0, 0, 0, 0, 0);
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

    MARK_FAIL(gcut_assert_equal_list_string(list1, list2));
}

void
test_equal_list_string_other_null (void)
{
    test = cut_test_new("equal_list_string test (other NULL)",
                        stub_equal_list_string_other_null);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_list_string test (other NULL)",
                           NULL,
                           "<list1 == list2>",
                           "(\"abc\", \"abc\", \"def\")",
                           "(NULL, \"abc\", \"def\")",
                           FAIL_LOCATION, "stub_equal_list_string_other_null",
                           NULL);
}

static void
stub_equal_list_enum (void)
{
    list1 = g_list_append(list1, GINT_TO_POINTER(CUT_TEST_RESULT_PENDING));
    list1 = g_list_append(list1, GINT_TO_POINTER(CUT_TEST_RESULT_SUCCESS));
    list2 = g_list_append(list2, GINT_TO_POINTER(CUT_TEST_RESULT_FAILURE));

    gcut_assert_equal_list_enum(CUT_TYPE_TEST_RESULT_STATUS, list1, list1);
    gcut_assert_equal_list_enum(CUT_TYPE_TEST_RESULT_STATUS, list2, list2);

    MARK_FAIL(gcut_assert_equal_list_enum(CUT_TYPE_TEST_RESULT_STATUS,
                                          list1, list2));
}

void
test_equal_list_enum (void)
{
    const gchar inspected_expected[] =
        "(#<CutTestResultStatus: "
        "pending(CUT_TEST_RESULT_PENDING:3)>, "
        "#<CutTestResultStatus: "
        "success(CUT_TEST_RESULT_SUCCESS:0)>)";
    const gchar inspected_actual[] =
        "(#<CutTestResultStatus: "
        "failure(CUT_TEST_RESULT_FAILURE:4)>)";
        ;
    const gchar *message, *message_with_diff;

    test = cut_test_new("equal_list_enum test", stub_equal_list_enum);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);

    message = cut_take_printf("<list1 == list2>\n"
                              "expected: <%s>\n"
                              "  actual: <%s>",
                              inspected_expected,
                              inspected_actual);
    message_with_diff = cut_append_diff(message,
                                        inspected_expected,
                                        inspected_actual);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_list_enum test",
                           NULL,
                           message_with_diff,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_list_enum",
                           NULL);
}

static void
stub_equal_list_flags (void)
{
    list1 = g_list_append(list1, GUINT_TO_POINTER(1 << 0 | 1 << 1));
    list1 = g_list_append(list1, GUINT_TO_POINTER(1 << 2));
    list2 = g_list_append(list2, GUINT_TO_POINTER(0));

    gcut_assert_equal_list_flags(flags_type, list1, list1);
    gcut_assert_equal_list_flags(flags_type, list2, list2);

    MARK_FAIL(gcut_assert_equal_list_flags(flags_type, list1, list2));
}

void
test_equal_list_flags (void)
{
    const gchar inspected_expected[] =
        "(#<CuttestAssertListStubFlags: "
        "first|second "
        "(CUTTEST_ASSERT_LIST_STUB_FIRST:0x1)|"
        "(CUTTEST_ASSERT_LIST_STUB_SECOND:0x2)>, "
        "#<CuttestAssertListStubFlags: "
        "third (CUTTEST_ASSERT_LIST_STUB_THIRD:0x4)>)";
    const gchar inspected_actual[] =
        "(#<CuttestAssertListStubFlags>)";
    const gchar *message, *message_with_diff;
    static const GFlagsValue values[] = {
        {1 << 0, "CUTTEST_ASSERT_LIST_STUB_FIRST", "first"},
        {1 << 1, "CUTTEST_ASSERT_LIST_STUB_SECOND", "second"},
        {1 << 2, "CUTTEST_ASSERT_LIST_STUB_THIRD", "third"},
        {0, NULL, NULL}
    };

    flags_type = g_flags_register_static("CuttestAssertListStubFlags", values);


    test = cut_test_new("equal_list_flags test", stub_equal_list_flags);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);

    message = cut_take_printf("<list1 == list2>\n"
                              "expected: <%s>\n"
                              "  actual: <%s>",
                              inspected_expected,
                              inspected_actual);
    message_with_diff = cut_append_diff(message,
                                        inspected_expected,
                                        inspected_actual);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_list_flags test",
                           NULL,
                           message_with_diff,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_list_flags",
                           NULL);
}

static void
g_object_unref_with_null_check (gpointer object)
{
    if (object)
        g_object_unref(object);
}

static void
stub_equal_list_object (void)
{
    need_to_free_list_contents = TRUE;
    list_element_free_function = g_object_unref_with_null_check;

    list1 = g_list_append(list1, cut_repository_new("directory1"));
    list1 = g_list_append(list1, NULL);
    list1 = g_list_append(list1, cut_test_new("test1", NULL));
    list2 = g_list_append(list2, cut_repository_new("directory2"));
    list2 = g_list_append(list2, NULL);
    list2 = g_list_append(list2, cut_test_new("test2", NULL));

    gcut_assert_equal_list_object(list1, list1);
    gcut_assert_equal_list_object(list2, list2);

    MARK_FAIL(gcut_assert_equal_list_object(list1, list2));
}

void
test_equal_list_object (void)
{
    const gchar *inspected_expected, *inspected_actual;
    const gchar *message, *message_with_diff;

    test = cut_test_new("equal_list_object test",
                        stub_equal_list_object);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);

    inspected_expected = cut_take_printf(
        "(#<CutRepository:%p directory=<\"directory1\">>, "
          "(null), "
         "#<CutTest:%p name=<\"test1\">, element-name=<\"test\">, "
            "test-function=<NULL>, base-directory=<NULL>>)",
        list1->data, g_list_nth_data(list1, 2));
    inspected_actual = cut_take_printf(
        "(#<CutRepository:%p directory=<\"directory2\">>, "
          "(null), "
         "#<CutTest:%p name=<\"test2\">, element-name=<\"test\">, "
            "test-function=<NULL>, base-directory=<NULL>>)",
        list2->data, g_list_nth_data(list2, 2));
    message = cut_take_printf("<list1 == list2>\n"
                              "expected: <%s>\n"
                              "  actual: <%s>",
                              inspected_expected,
                              inspected_actual);
    message_with_diff = cut_append_diff(message,
                                        inspected_expected,
                                        inspected_actual);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_list_object test",
                           NULL,
                           message_with_diff,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_list_object",
                           NULL);
}

static void
stub_equal_hash_table (void)
{
    hash1 = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
    hash2 = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);

    g_hash_table_insert(hash1, GUINT_TO_POINTER(1), g_strdup("11"));
    g_hash_table_insert(hash1, GUINT_TO_POINTER(10), g_strdup("22"));
    g_hash_table_insert(hash2, GUINT_TO_POINTER(2), g_strdup("99"));
    g_hash_table_insert(hash2, GUINT_TO_POINTER(20), g_strdup("88"));

    gcut_assert_equal_hash_table(hash1, hash1,
                                 g_str_equal,
                                 gcut_inspect_direct,
                                 gcut_inspect_string,
                                 NULL);
    gcut_assert_equal_hash_table(hash2, hash2,
                                 g_str_equal,
                                 gcut_inspect_direct,
                                 gcut_inspect_string,
                                 NULL);

    MARK_FAIL(gcut_assert_equal_hash_table(hash1, hash2,
                                           g_str_equal,
                                           gcut_inspect_direct,
                                           gcut_inspect_string,
                                           NULL));
}

void
test_equal_hash_table (void)
{
    test = cut_test_new("equal_hash_table test",
                        stub_equal_hash_table);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_hash_table test",
                           NULL,
                           "<g_str_equal(hash1[key], hash2[key]) == TRUE>\n"
                           "expected: <{1 => \"11\", 10 => \"22\"}>\n"
                           "  actual: <{2 => \"99\", 20 => \"88\"}>",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_hash_table",
                           NULL);
}

static void
stub_equal_hash_table_string_string (void)
{
    hash1 = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    hash2 = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    g_hash_table_insert(hash1, g_strdup("abc"), g_strdup("11"));
    g_hash_table_insert(hash1, g_strdup("def"), g_strdup("22"));
    g_hash_table_insert(hash2, g_strdup("zyx"), g_strdup("99"));
    g_hash_table_insert(hash2, g_strdup("wvu"), g_strdup("88"));

    gcut_assert_equal_hash_table_string_string(hash1, hash1);
    gcut_assert_equal_hash_table_string_string(hash2, hash2);

    MARK_FAIL(gcut_assert_equal_hash_table_string_string(hash1, hash2));
}

void
test_equal_hash_table_string_string (void)
{
    test = cut_test_new("equal_hash_table_string_string test",
                        stub_equal_hash_table_string_string);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 2, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_hash_table_string_string test",
                           NULL,
                           "<hash1 == hash2>\n"
                           "expected: <{\"abc\" => \"11\", \"def\" => \"22\"}>\n"
                           "  actual: <{\"wvu\" => \"88\", \"zyx\" => \"99\"}>",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_hash_table_string_string",
                           NULL);
}

static void
stub_error (void)
{
    gcut_assert_error(error);

    error = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "not found");
    MARK_FAIL(gcut_assert_error(error));
}

void
test_error (void)
{
    test = cut_test_new("gcut_assert_error test", stub_error);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "gcut_assert_error test",
                           NULL,
                           "expected: <error> is NULL\n"
                           "  actual: <g-file-error-quark:4: not found>",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_error",
                           NULL);
}

static void
stub_equal_error (void)
{
    error1 = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "not found");
    error2 = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "not found");
    gcut_assert_equal_error(error1, error2);

    g_error_free(error2);
    error2 = g_error_new(G_FILE_ERROR, G_FILE_ERROR_NOENT, "no entry");
    MARK_FAIL(gcut_assert_equal_error(error1, error2));
}

void
test_equal_error (void)
{
    test = cut_test_new("gcut_assert_equal_error test", stub_equal_error);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "gcut_assert_equal_error test",
                           NULL,
                           "<error1 == error2>\n"
                           "expected: <g-file-error-quark:4: not found>\n"
                           "  actual: <g-file-error-quark:4: no entry>\n"
                           "\n"
                           "diff:\n"
                           "- g-file-error-quark:4: not found\n"
                           "?                          ^^^^^^\n"
                           "+ g-file-error-quark:4: no entry\n"
                           "?                         +++ ^^",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_error",
                           NULL);
}

static void
stub_equal_enum (void)
{
    gcut_assert_equal_enum(CUT_TYPE_TEST_RESULT_STATUS,
                           CUT_TEST_RESULT_PENDING,
                           CUT_TEST_RESULT_PENDING);
    MARK_FAIL(gcut_assert_equal_enum(CUT_TYPE_TEST_RESULT_STATUS,
                                     CUT_TEST_RESULT_FAILURE,
                                     CUT_TEST_RESULT_PENDING));
}

void
test_equal_enum (void)
{
    test = cut_test_new("gcut_assert_equal_enum test", stub_equal_enum);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(
        run_context, 0, CUT_TEST_RESULT_FAILURE,
        "gcut_assert_equal_enum test",
        NULL,
        "<CUT_TEST_RESULT_FAILURE == CUT_TEST_RESULT_PENDING>"
        " (CUT_TYPE_TEST_RESULT_STATUS)\n"
        "expected: <#<CutTestResultStatus: "
        "failure(CUT_TEST_RESULT_FAILURE:4)>>\n"
        "  actual: <#<CutTestResultStatus: "
        "pending(CUT_TEST_RESULT_PENDING:3)>>\n"
        "\n"
        "diff:\n"
        "- #<CutTestResultStatus: failure(CUT_TEST_RESULT_FAILURE:4)>\n"
        "?                        ^^ ^^^^                 ^^ ^^^^ ^\n"
        "+ #<CutTestResultStatus: pending(CUT_TEST_RESULT_PENDING:3)>\n"
        "?                        ^^^^ ^^                 ^^^^ ^^ ^",
        NULL, NULL,
        FAIL_LOCATION, "stub_equal_enum",
        NULL);
}

static void
stub_equal_flags (void)
{
    gcut_assert_equal_flags(flags_type,
                            (1 << 0) | (1 << 2),
                            (1 << 0) | (1 << 2));
    MARK_FAIL(gcut_assert_equal_flags(flags_type,
                                      (1 << 1) | (1 << 2),
                                      (1 << 3)));
}

void
test_equal_flags (void)
{
    static const GFlagsValue values[] = {
        {1 << 0, "CUTTEST_ASSERT_STUB_FIRST", "first"},
        {1 << 1, "CUTTEST_ASSERT_STUB_SECOND", "second"},
        {1 << 2, "CUTTEST_ASSERT_STUB_THIRD", "third"},
        {0, NULL, NULL}
    };

    flags_type = g_flags_register_static("CuttestAssertStubFlags", values);

    test = cut_test_new("gcut_assert_equal_flags test", stub_equal_flags);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "gcut_assert_equal_flags test",
                           NULL,
                           "<(1 << 1) | (1 << 2) == (1 << 3)> (flags_type)\n"
                           "expected: <#<CuttestAssertStubFlags: "
                           "second|third "
                           "(CUTTEST_ASSERT_STUB_SECOND:0x2)|"
                           "(CUTTEST_ASSERT_STUB_THIRD:0x4)>>\n"
                           "  actual: <#<CuttestAssertStubFlags: "
                           "(unknown flags: 0x8)>>",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_flags",
                           NULL);
}

static void
stub_equal_object (void)
{
    object1 = G_OBJECT(cut_loader_new("so-name1"));
    object2 = G_OBJECT(cut_loader_new("so-name2"));

    gcut_assert_equal_object(object1, object1);
    gcut_assert_equal_object(object2, object2);
    gcut_assert_equal_object(NULL, NULL);
    MARK_FAIL(gcut_assert_equal_object(object1, object2));
}

void
test_equal_object (void)
{
    const gchar *inspected_expected, *inspected_actual;
    const gchar *message;

    test = cut_test_new("gcut_assert_equal_object test", stub_equal_object);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 3, 0, 1, 0, 0, 0, 0);

    inspected_expected = cut_take_printf("#<CutLoader:%p "
                                         "so-filename=<\"so-name1\">, "
                                         "base-directory=<NULL>>",
                                         object1);
    inspected_actual = cut_take_printf("#<CutLoader:%p "
                                       "so-filename=<\"so-name2\">, "
                                       "base-directory=<NULL>>",
                                       object2);
    message = cut_take_printf("<object1 == object2>\n"
                              "expected: <%s>\n"
                              "  actual: <%s>",
                              inspected_expected, inspected_actual);
    message = cut_append_diff(message, inspected_expected, inspected_actual);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "gcut_assert_equal_object test",
                           NULL,
                           message,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_object",
                           NULL);
}

static void
stub_equal_object_null (void)
{
    object1 = G_OBJECT(cut_loader_new("so-name"));
    MARK_FAIL(gcut_assert_equal_object(object1, NULL));
}

void
test_equal_object_null (void)
{
    const gchar *inspected_expected;
    const gchar *message;

    test = cut_test_new("gcut_assert_equal_object_null test",
                        stub_equal_object_null);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 0, 0, 1, 0, 0, 0, 0);

    inspected_expected = cut_take_printf("#<CutLoader:%p "
                                         "so-filename=<\"so-name\">, "
                                         "base-directory=<NULL>>",
                                         object1);
    message = cut_take_printf("<object1 == NULL>\n"
                              "expected: <%s>\n"
                              "  actual: <%s>",
                              inspected_expected, "(null)");
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "gcut_assert_equal_object_null test",
                           NULL,
                           message,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_object_null",
                           NULL);
}

static gboolean
equal_name (gconstpointer data1, gconstpointer data2)
{
    return g_str_equal(cut_test_get_name(CUT_TEST(data1)),
                       cut_test_get_name(CUT_TEST(data2)));
}

static void
stub_equal_object_custom (void)
{
    object1 = G_OBJECT(cut_test_new("test-name", NULL));
    object2 = G_OBJECT(cut_test_new("test-name", NULL));
    object3 = G_OBJECT(cut_test_new("no-name", NULL));

    gcut_assert_equal_object_custom(object1, object2, equal_name);
    MARK_FAIL(gcut_assert_equal_object_custom(object1, object3, equal_name));
}

void
test_equal_object_custom (void)
{
    const gchar *inspected_expected, *inspected_actual;
    const gchar *message;

    test = cut_test_new("gcut_assert_equal_object_custom test",
                        stub_equal_object_custom);
    cut_assert_not_null(test);

    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);

    inspected_expected = cut_take_printf("#<CutTest:%p "
                                         "name=<\"test-name\">, "
                                         "element-name=<\"test\">, "
                                         "test-function=<NULL>, "
                                         "base-directory=<NULL>>",
                                         object1);
    inspected_actual = cut_take_printf("#<CutTest:%p "
                                       "name=<\"no-name\">, "
                                       "element-name=<\"test\">, "
                                       "test-function=<NULL>, "
                                       "base-directory=<NULL>>",
                                       object3);
    message = cut_take_printf("<equal_name(object1, object3)>\n"
                              "expected: <%s>\n"
                              "  actual: <%s>",
                              inspected_expected, inspected_actual);
    message = cut_append_diff(message, inspected_expected, inspected_actual);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "gcut_assert_equal_object_custom test",
                           NULL,
                           message,
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_object_custom",
                           NULL);
}

static void
stub_equal_pid (void)
{
    gcut_assert_equal_pid(0, 0);
    MARK_FAIL(gcut_assert_equal_pid(0, (GPid)100));
}

void
test_equal_pid (void)
{
    test = cut_test_new("cut_assert_equal_pid()", stub_equal_pid);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_equal_pid()", NULL,
                           "<0 == (GPid)100>\n"
                           "expected: <0>\n"
                           "  actual: <100>",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_equal_pid",
                           NULL);
}

static void
stub_not_equal_pid (void)
{
    gcut_assert_not_equal_pid(0, (GPid)100);
    MARK_FAIL(gcut_assert_not_equal_pid(0, 0));
}

void
test_not_equal_pid (void)
{
    test = cut_test_new("cut_assert_not_equal_pid()", stub_not_equal_pid);
    cut_assert_false(run());
    cut_assert_test_result_summary(run_context, 1, 1, 0, 1, 0, 0, 0, 0);
    cut_assert_test_result(run_context, 0, CUT_TEST_RESULT_FAILURE,
                           "cut_assert_not_equal_pid()", NULL,
                           "<0 != 0>\n"
                           "expected: <0>\n"
                           "  actual: <0>",
                           NULL, NULL,
                           FAIL_LOCATION, "stub_not_equal_pid",
                           NULL);
}


/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
