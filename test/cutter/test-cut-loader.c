/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2010  Kouhei Sutou <kou@clear-code.com>
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

#include <string.h>

#include <gmodule.h>

#include <cutter.h>
#include <cutter/cut-loader.h>
#include <cutter/cut-test-runner.h>

#include "../lib/cuttest-assertions.h"

void test_load_function (void);
void data_fixture_function (void);
void test_fixture_function (gconstpointer data);
void test_fail_to_load (void);
void test_load_test_iterator (void);
void test_load_cpp_namespace (void);
void test_load_cpp_multi_namespace (void);
void data_cpp_fixture_function (void);
void test_cpp_fixture_function (gconstpointer data);

static CutLoader *loader;
static CutTestCase *test_case;
static CutRunContext *run_context;
static GPtrArray *test_names;
static GModule *module;

void
cut_setup (void)
{
    loader = NULL;
    test_case = NULL;
    test_names = NULL;
    run_context = NULL;
    module = NULL;
}

void
cut_teardown (void)
{
    if (loader)
        g_object_unref(loader);
    if (test_case)
        g_object_unref(test_case);
    if (test_names)
        g_ptr_array_free(test_names, TRUE);
    if (run_context)
        g_object_unref(run_context);
    if (module)
        g_module_close(module);
}

static CutLoader *
loader_new (const gchar *directory, const gchar *module_name)
{
    CutLoader *loader;
    gchar *test_path;

    test_path = g_build_filename(cuttest_get_base_dir(),
                                 "fixtures",
                                 "loader",
                                 directory,
#ifndef G_OS_WIN32
                                 ".libs",
#endif
                                 module_name,
                                 NULL);
    loader = cut_loader_new(test_path);
    g_free(test_path);

    return loader;
}

static gboolean
run (void)
{
    cut_assert_not_null(test_case);

    run_context = cut_test_runner_new();
    return cut_test_runner_run_test_case(CUT_TEST_RUNNER(run_context),
                                         test_case);
}

static int
compare_test_name (gconstpointer a, gconstpointer b)
{
    return strcmp(*(gchar * const *)a, *(gchar * const *)b);
}

void
test_load_function (void)
{
    CutTestContainer *container;
    GList *tests, *list;
    gint i;
    const gchar *target_test_names[] = {"/.*/", NULL};
    gchar *expected_functions[] = {
        "test_abcdefghijklmnopqratuvwzyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789",
        "test_stub_function1",
        "test_stub_function2",
        "test_stub_function3",
        NULL
    };

    loader = loader_new("test", "stub-test-functions." G_MODULE_SUFFIX);
    test_case = cut_loader_load_test_case(loader);
    cut_assert(test_case);

    container = CUT_TEST_CONTAINER(test_case);
    cut_assert_equal_int(4,
                         cut_test_container_get_n_tests(container, NULL));

    run_context = cut_test_runner_new();
    cut_run_context_set_target_test_names(run_context, target_test_names);
    cut_assert_equal_int(4,
                         cut_test_container_get_n_tests(container, run_context));

    tests = (GList *)cut_test_container_get_children(container);
    cut_assert(tests);

    test_names = g_ptr_array_new_with_free_func(g_free);
    for (list = tests, i = 0; list; list = g_list_next(list), i++) {
        CutTest *test;

        cut_assert(list->data);
        cut_assert(CUT_IS_TEST(list->data));

        test = CUT_TEST(list->data);
        g_ptr_array_add(test_names, g_strdup(cut_test_get_name(test)));
    }
    g_ptr_array_sort(test_names, compare_test_name);
    g_ptr_array_add(test_names, NULL);
    cut_assert_equal_string_array(expected_functions,
                                  (gchar **)test_names->pdata);
}

typedef struct _FixtureTestData
{
    gchar *file_name;
    gchar *startup_function_name;
    gchar *setup_function_name;
    gchar *teardown_function_name;
    gchar *shutdown_function_name;
} FixtureTestData;

static FixtureTestData *
fixture_test_data_new (gchar *file_name,
                       gchar *startup_function_name,
                       gchar *setup_function_name,
                       gchar *teardown_function_name,
                       gchar *shutdown_function_name)
{
    FixtureTestData *data;

    data = g_new(FixtureTestData, 1);
    data->file_name = g_strdup(file_name);
    data->startup_function_name = g_strdup(startup_function_name);
    data->setup_function_name = g_strdup(setup_function_name);
    data->teardown_function_name = g_strdup(teardown_function_name);
    data->shutdown_function_name = g_strdup(shutdown_function_name);

    return data;
}

static void
fixture_test_data_free (FixtureTestData *data)
{
    g_free(data->file_name);
    g_free(data->startup_function_name);
    g_free(data->setup_function_name);
    g_free(data->teardown_function_name);
    g_free(data->shutdown_function_name);
    g_free(data);
}


void
data_fixture_function (void)
{
    cut_add_data("without prefix",
                 fixture_test_data_new("without-prefix." G_MODULE_SUFFIX,
                                       NULL,
                                       "setup",
                                       "teardown",
                                       NULL),
                 fixture_test_data_free,
                 "with prefix",
                 fixture_test_data_new("with-prefix." G_MODULE_SUFFIX,
                                       "cut_startup",
                                       "cut_setup",
                                       "cut_teardown",
                                       "cut_shutdown"),
                 fixture_test_data_free,
                 "all",
                 fixture_test_data_new("all." G_MODULE_SUFFIX,
                                       "cut_startup",
                                       "cut_setup",
                                       "cut_teardown",
                                       "cut_shutdown"),
                 fixture_test_data_free,
                 NULL);
}

void
test_fixture_function (gconstpointer data)
{
    const FixtureTestData *test_data = data;
    CutStartupFunction expected_startup_function = NULL;
    CutStartupFunction actual_startup_function = NULL;
    CutShutdownFunction expected_shutdown_function = NULL;
    CutShutdownFunction actual_shutdown_function = NULL;
    CutSetupFunction expected_setup_function = NULL;
    CutSetupFunction actual_setup_function = NULL;
    CutTeardownFunction expected_teardown_function = NULL;
    CutTeardownFunction actual_teardown_function = NULL;
    gchar *so_filename;

    loader = loader_new("fixture", test_data->file_name);
    test_case = cut_loader_load_test_case(loader);
    cut_assert(test_case);

    g_object_get(G_OBJECT(loader),
                 "so-filename", &so_filename,
                 NULL);
    module = g_module_open(so_filename,
                           G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
    g_free(so_filename);
    cut_assert_not_null(module);
    if (test_data->startup_function_name)
        cut_assert_true(g_module_symbol(module,
                                        test_data->startup_function_name,
                                        (gpointer)&expected_startup_function));
    if (test_data->setup_function_name)
        cut_assert_true(g_module_symbol(module,
                                        test_data->setup_function_name,
                                        (gpointer)&expected_setup_function));
    if (test_data->teardown_function_name)
        cut_assert_true(g_module_symbol(module,
                                        test_data->teardown_function_name,
                                        (gpointer)&expected_teardown_function));
    if (test_data->shutdown_function_name)
        cut_assert_true(g_module_symbol(module,
                                        test_data->shutdown_function_name,
                                        (gpointer)&expected_shutdown_function));

    g_object_get(G_OBJECT(test_case),
                 "startup-function", &actual_startup_function,
                 "setup-function", &actual_setup_function,
                 "teardown-function", &actual_teardown_function,
                 "shutdown-function", &actual_shutdown_function,
                 NULL);
    cut_assert_equal_pointer(expected_startup_function,
                             actual_startup_function);
    cut_assert_equal_pointer(expected_setup_function,
                             actual_setup_function);
    cut_assert_equal_pointer(expected_teardown_function,
                             actual_teardown_function);
    cut_assert_equal_pointer(expected_shutdown_function,
                             actual_shutdown_function);
}

void
test_fail_to_load (void)
{
    loader = loader_new("module", "cannot-load-module." G_MODULE_SUFFIX);
    cut_assert_not_null(loader);

    test_case = cut_loader_load_test_case(loader);
    cut_assert_null(test_case);
}

void
test_load_test_iterator (void)
{
    const GList *tests;
    CutTestContainer *container;
    CutTestIterator *test_iterator;
    CutIteratedTestFunction iterated_test_function = NULL;
    CutDataSetupFunction data_setup_function = NULL;

    loader = loader_new("iterator", "success-iterated-test." G_MODULE_SUFFIX);
    cut_assert_not_null(loader);

    test_case = cut_loader_load_test_case(loader);
    cut_assert_not_null(test_case);
    container = CUT_TEST_CONTAINER(test_case);
    cut_assert_equal_int(1, cut_test_container_get_n_tests(container, NULL));

    tests = cut_test_container_get_children(container);
    cut_assert_not_null(tests);

    test_iterator = tests->data;
    cut_assert_not_null(test_iterator);

    g_object_get(test_iterator,
                 "iterated-test-function", &iterated_test_function,
                 "data-setup-function", &data_setup_function,
                 NULL);
    cut_assert_not_null(iterated_test_function);
    cut_assert_not_null(data_setup_function);

    cut_assert_true(run());
    cut_assert_test_result_summary(run_context, 2, 2, 2, 0, 0, 0, 0, 0);
}

void
test_load_cpp_namespace (void)
{
    CutTestContainer *container;
    GList *tests, *list;
    gint i;
    const gchar *target_test_names[] = {"/.*/", NULL};
    gchar *expected_functions[] = {
        "calc::test_add",
        "calc::test_sub",
        NULL
    };

    loader = loader_new("cpp", "stub-namespace." G_MODULE_SUFFIX);
    test_case = cut_loader_load_test_case(loader);
    cut_assert(test_case);

    container = CUT_TEST_CONTAINER(test_case);
    cut_assert_equal_int(2,
                         cut_test_container_get_n_tests(container, NULL));

    run_context = cut_test_runner_new();
    cut_run_context_set_target_test_names(run_context, target_test_names);
    cut_assert_equal_int(2,
                         cut_test_container_get_n_tests(container, run_context));

    tests = (GList *)cut_test_container_get_children(container);
    cut_assert(tests);

    test_names = g_ptr_array_new_with_free_func(g_free);
    for (list = tests, i = 0; list; list = g_list_next(list), i++) {
        CutTest *test;

        cut_assert(list->data);
        cut_assert(CUT_IS_TEST(list->data));

        test = CUT_TEST(list->data);
        g_ptr_array_add(test_names, g_strdup(cut_test_get_name(test)));
    }
    g_ptr_array_sort(test_names, compare_test_name);
    g_ptr_array_add(test_names, NULL);
    cut_assert_equal_string_array(expected_functions,
                                  (gchar **)test_names->pdata);
}

void
test_load_cpp_multi_namespace (void)
{
    GList *test_cases, *test_case_node;
    gchar *expected_functions[] = {
        "adder::test_compute",
        "subtracter::test_compute",
        NULL
    };

    loader = loader_new("cpp", "stub-multi-namespace." G_MODULE_SUFFIX);
    test_cases = cut_loader_load_test_cases(loader);
    cut_assert_equal_int(2, g_list_length(test_cases));

    test_names = g_ptr_array_new_with_free_func(g_free);
    for (test_case_node = test_cases;
         test_case_node;
         test_case_node = g_list_next(test_case_node)) {
        CutTestContainer *container;
        GList *tests, *test_node;

        container = CUT_TEST_CONTAINER(test_case_node->data);
        tests = (GList *)cut_test_container_get_children(container);
        for (test_node = tests; test_node; test_node = g_list_next(test_node)) {
            CutTest *test = test_node->data;

            cut_assert(CUT_IS_TEST(test));
            g_ptr_array_add(test_names, g_strdup(cut_test_get_name(test)));
        }
    }
    g_ptr_array_sort(test_names, compare_test_name);
    g_ptr_array_add(test_names, NULL);
    cut_assert_equal_string_array(expected_functions,
                                  (gchar **)test_names->pdata);
}

#ifdef _WIN32
static const gchar *
mangle (const gchar *component, ...)
{
    GString *mangled;
    va_list args;

    mangled = g_string_new("@YAXXZ");
    va_start(args, component);
    while (component) {
        g_string_prepend(mangled, "@");
        g_string_prepend(mangled, component);
        component = va_arg(args, gchar *);
    }
    va_end(args);
    g_string_prepend(mangled, "?");

    return cut_take_string(g_string_free(mangled, FALSE));
}
#else
static const gchar *
mangle (const gchar *component, ...)
{
    GString *mangled;
    va_list args;

    mangled = g_string_new("_ZN");
    va_start(args, component);
    while (component) {
        g_string_append_printf(mangled,
                               "%" G_GSIZE_FORMAT "%s",
                               strlen(component), component);
        component = va_arg(args, gchar *);
    }
    va_end(args);
    g_string_append(mangled, "Ev");

    return cut_take_string(g_string_free(mangled, FALSE));
}
#endif

void
data_cpp_fixture_function (void)
{
    cut_add_data("without prefix",
                 fixture_test_data_new("without-prefix." G_MODULE_SUFFIX,
                                       "startup",
                                       "setup",
                                       "teardown",
                                       "shutdown"),
                 fixture_test_data_free,
                 "with prefix",
                 fixture_test_data_new("with-prefix." G_MODULE_SUFFIX,
                                       "cut_startup",
                                       "cut_setup",
                                       "cut_teardown",
                                       "cut_shutdown"),
                 fixture_test_data_free,
                 "all",
                 fixture_test_data_new("all." G_MODULE_SUFFIX,
                                       "cut_startup",
                                       "cut_setup",
                                       "cut_teardown",
                                       "cut_shutdown"),
                 fixture_test_data_free,
                 NULL);
}

void
test_cpp_fixture_function (gconstpointer data)
{
    const FixtureTestData *test_data = data;
    CutStartupFunction expected_startup_function = NULL;
    CutStartupFunction actual_startup_function = NULL;
    CutShutdownFunction expected_shutdown_function = NULL;
    CutShutdownFunction actual_shutdown_function = NULL;
    CutSetupFunction expected_setup_function = NULL;
    CutSetupFunction actual_setup_function = NULL;
    CutTeardownFunction expected_teardown_function = NULL;
    CutTeardownFunction actual_teardown_function = NULL;
    gchar *so_filename;

    loader = loader_new("cpp-fixture", test_data->file_name);
    test_case = cut_loader_load_test_case(loader);
    cut_assert(test_case);

    g_object_get(G_OBJECT(loader),
                 "so-filename", &so_filename,
                 NULL);
    module = g_module_open(so_filename,
                           G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
    g_free(so_filename);
    cut_assert_not_null(module);
    if (test_data->startup_function_name)
        cut_assert_true(g_module_symbol(module,
                                        mangle("fixture",
                                               test_data->startup_function_name,
                                               NULL),
                                        (gpointer)&expected_startup_function));
    if (test_data->setup_function_name)
        cut_assert_true(g_module_symbol(module,
                                        mangle("fixture",
                                               test_data->setup_function_name,
                                               NULL),
                                        (gpointer)&expected_setup_function));
    if (test_data->teardown_function_name)
        cut_assert_true(g_module_symbol(module,
                                        mangle("fixture",
                                               test_data->teardown_function_name,
                                               NULL),
                                        (gpointer)&expected_teardown_function));
    if (test_data->shutdown_function_name)
        cut_assert_true(g_module_symbol(module,
                                        mangle("fixture",
                                               test_data->shutdown_function_name,
                                               NULL),
                                        (gpointer)&expected_shutdown_function));

    g_object_get(G_OBJECT(test_case),
                 "startup-function", &actual_startup_function,
                 "setup-function", &actual_setup_function,
                 "teardown-function", &actual_teardown_function,
                 "shutdown-function", &actual_shutdown_function,
                 NULL);
    cut_assert_equal_pointer(expected_startup_function,
                             actual_startup_function);
    cut_assert_equal_pointer(expected_setup_function,
                             actual_setup_function);
    cut_assert_equal_pointer(expected_teardown_function,
                             actual_teardown_function);
    cut_assert_equal_pointer(expected_shutdown_function,
                             actual_shutdown_function);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
