#include <gcutter.h>
#include "cut-test.h"
#include "cut-test-result.h"
#include "cut-utils.h"
#include "cut-runner.h"
#include "cuttest-assertions.h"

void test_equal_g_type(void);
void test_equal_g_value(void);
void test_equal_int_g_list(void);

static CutTest *test;
static CutRunner *runner;
static CutTestContext *test_context;
static CutTestResult *test_result;

static GValue *value1, *value2;
static GList *list1, *list2;

static gboolean
run (CutTest *_test)
{
    gboolean success;
    CutTestContext *original_test_context;

    test = _test;

    runner = cut_runner_new();

    test_context = cut_test_context_new(NULL, NULL, test);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, runner);
    set_current_test_context(original_test_context);

    return success;
}

void
setup (void)
{
    test = NULL;
    runner = NULL;
    test_context = NULL;
    test_result = NULL;

    value1 = g_new0(GValue, 1);
    value2 = g_new0(GValue, 1);
}

void
teardown (void)
{
    if (test)
        g_object_unref(test);
    if (runner)
        g_object_unref(runner);
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

    g_list_free(list1);
    g_list_free(list2);
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
    cut_assert_test_result_summary(runner, 1, 1, 1, 0, 0, 0, 0);
    cut_assert_test_result(runner, 0, CUT_TEST_RESULT_FAILURE,
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
    cut_assert_test_result_summary(runner, 1, 2, 1, 0, 0, 0, 0);
    cut_assert_test_result(runner, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_g_value test",
                           NULL,
                           "<value1 == value2>\n"
                           "expected: <10> (gint)\n"
                           " but was: <\"String\"> (gchararray)",
                           "equal_g_value_test");
}

static void
equal_int_g_list_test (void)
{
    list1 = g_list_append(list1, GINT_TO_POINTER(100));
    list1 = g_list_append(list1, GINT_TO_POINTER(200));
    list2 = g_list_append(list2, GINT_TO_POINTER(1000));
    list2 = g_list_append(list2, GINT_TO_POINTER(2000));

    cut_assert_equal_int_g_list(list1, list1);
    cut_assert_equal_int_g_list(list2, list2);

    cut_assert_equal_int_g_list(list1, list2);
}

void
test_equal_int_g_list (void)
{
    CutTest *test;

    test = cut_test_new("equal_int_g_list test", equal_int_g_list_test);
    cut_assert(test);

    cut_assert(!run(test));
    cut_assert_test_result_summary(runner, 1, 2, 1, 0, 0, 0, 0);
    cut_assert_test_result(runner, 0, CUT_TEST_RESULT_FAILURE,
                           "equal_int_g_list test",
                           NULL,
                           "<list1 == list2>\n"
                           "expected: <(100, 200)>\n"
                           " but was: <(1000, 2000)>",
                           "equal_int_g_list_test");
}

/*
vi:nowrap:ai:expandtab:sw=4
*/

