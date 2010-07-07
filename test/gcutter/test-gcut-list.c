/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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
#include <cutter/cut-enum-types.h>

void test_equal_string_including_null(void);
void test_inspect_string_including_null(void);
void test_inspect_enum(void);
void test_inspect_flags(void);

static gchar *inspected;

void
cut_setup (void)
{
    inspected = NULL;
}

void
cut_teardown (void)
{
    if (inspected)
        g_free(inspected);
}

void
test_equal_string_including_null (void)
{
    GList *list1 = NULL, *list2 = NULL;

    list1 = g_list_append(list1, g_strdup("a"));
    list1 = g_list_append(list1, NULL);
    list1 = g_list_append(list1, g_strdup("c"));
    gcut_take_list(list1, g_free);

    cut_assert_true(gcut_list_equal_string(list1, list1));

    list2 = g_list_append(list2, NULL);
    list2 = g_list_append(list2, g_strdup("b"));
    list2 = g_list_append(list2, g_strdup("c"));
    gcut_take_list(list2, g_free);

    cut_assert_false(gcut_list_equal_string(list1, list2));
}

void
test_inspect_string_including_null (void)
{
    GList *list = NULL;

    list = g_list_append(list, g_strdup("a"));
    list = g_list_append(list, NULL);
    list = g_list_append(list, g_strdup("c"));
    gcut_take_list(list, g_free);

    inspected = gcut_list_inspect_string(list);
    cut_assert_equal_string("(\"a\", NULL, \"c\")",
                            inspected);
}

void
test_inspect_enum (void)
{
    const GList *list;

    list = gcut_take_new_list_int(2,
                                  CUT_TEST_RESULT_PENDING,
                                  CUT_TEST_RESULT_NOTIFICATION);
    inspected = gcut_list_inspect_enum(CUT_TYPE_TEST_RESULT_STATUS, list);
    cut_assert_equal_string("(#<CutTestResultStatus: "
                            "pending(CUT_TEST_RESULT_PENDING:3)>, "
                            "#<CutTestResultStatus: "
                            "notification(CUT_TEST_RESULT_NOTIFICATION:1)>)",
                            inspected);
}

void
test_inspect_flags (void)
{
    const GList *list;
    static GType type = 0;
    if (type == 0) {
        static const GFlagsValue values[] = {
            {1 << 0, "CUTTEST_LIST_STUB_FIRST", "first"},
            {1 << 1, "CUTTEST_LIST_STUB_SECOND", "second"},
            {1 << 2, "CUTTEST_LIST_STUB_THIRD", "third"},
            {0, NULL, NULL}
        };
        type = g_flags_register_static("CuttestListStubFlags", values);
    }

    list = gcut_take_new_list_uint(2,
                                   1 << 0 | 1 << 1,
                                   1 << 2);
    inspected = gcut_list_inspect_flags(type, list);
    cut_assert_equal_string("(#<CuttestListStubFlags: "
                            "first|second "
                            "(CUTTEST_LIST_STUB_FIRST:0x1)|"
                            "(CUTTEST_LIST_STUB_SECOND:0x2)>, "
                            "#<CuttestListStubFlags: "
                            "third "
                            "(CUTTEST_LIST_STUB_THIRD:0x4)>)",
                            inspected);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
