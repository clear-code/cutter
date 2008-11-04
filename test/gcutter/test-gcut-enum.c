/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>
#include <cutter/cut-enum-types.h>

void test_enum_inspect(void);
void data_enum_parse(void);
void test_enum_parse(gconstpointer data);
void test_flags_inspect(void);
void data_flags_parse(void);
void test_flags_parse(gconstpointer data);

static gchar *inspected;

static GError *actual_error;

static GType flags_type = 0;

void
startup (void)
{
    if (flags_type == 0) {
        static const GFlagsValue values[] = {
            {1 << 0, "CUTTEST_STUB_FIRST", "first"},
            {1 << 1, "CUTTEST_STUB_SECOND", "second"},
            {1 << 2, "CUTTEST_STUB_THIRD", "third"},
            {0, NULL, NULL}
        };
        flags_type = g_flags_register_static("CuttestStubFlags", values);
    }
}

void
setup (void)
{
    inspected = NULL;
    actual_error = NULL;
}

void
teardown (void)
{
    if (inspected)
        g_free(inspected);

    if (actual_error)
        g_error_free(actual_error);
}

void
test_enum_inspect (void)
{
    inspected = gcut_enum_inspect(CUT_TYPE_TEST_RESULT_STATUS,
                                  CUT_TEST_RESULT_PENDING);
    cut_assert_equal_string("#<CutTestResultStatus: "
                            "pending(CUT_TEST_RESULT_PENDING:3)>",
                            inspected);

    g_free(inspected);
    inspected = gcut_enum_inspect(CUT_TYPE_TEST_RESULT_STATUS, -100);
    cut_assert_equal_string("#<CutTestResultStatus: -100>", inspected);
}

typedef struct _ParseTestData
{
    GType type;
    gchar *value;
    gint expected_enum_value;
    guint expected_flags_value;
    GError *expected_error;
} ParseTestData;

static ParseTestData *
parse_test_data_new (GType type, const gchar *value, gint expected_enum_value,
                     guint expected_flags_value, GError *expected_error)
{
    ParseTestData *data;

    data = g_new(ParseTestData, 1);

    data->type = type;
    data->value = g_strdup(value);
    data->expected_enum_value = expected_enum_value;
    data->expected_flags_value = expected_flags_value;
    data->expected_error = expected_error;

    return data;
}

static ParseTestData *
parse_enum_test_data_new (GType type, const gchar *value,
                          gint expected_enum_value, GError *expected_error)
{
    return parse_test_data_new(type, value, expected_enum_value, 0,
                               expected_error);
}

static ParseTestData *
parse_flags_test_data_new (GType type, const gchar *value,
                           gint expected_flags_value, GError *expected_error)
{
    return parse_test_data_new(type, value, 0, expected_flags_value,
                               expected_error);
}

static void
parse_test_data_free (ParseTestData *data)
{
    if (data->value)
        g_free(data->value);

    if (data->expected_error)
        g_error_free(data->expected_error);

    g_free(data);
}

void
data_enum_parse (void)
{
    cut_add_data("nick",
                 parse_enum_test_data_new(CUT_TYPE_TEST_RESULT_STATUS,
                                          "pending", CUT_TEST_RESULT_PENDING,
                                          NULL),
                 parse_test_data_free,
                 "NICK",
                 parse_enum_test_data_new(CUT_TYPE_TEST_RESULT_STATUS,
                                          "PENDING", CUT_TEST_RESULT_PENDING,
                                          NULL),
                 parse_test_data_free,
                 "name",
                 parse_enum_test_data_new(CUT_TYPE_TEST_RESULT_STATUS,
                                          "CUT_TEST_RESULT_PENDING",
                                          CUT_TEST_RESULT_PENDING,
                                          NULL),
                 parse_test_data_free,
                 "NULL",
                 parse_enum_test_data_new(
                     CUT_TYPE_TEST_RESULT_STATUS,
                     NULL,
                     0,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_FORMAT,
                                 "enum value should not be NULL")),
                 parse_test_data_free,
                 "invalid type",
                 parse_enum_test_data_new(
                     GCUT_TYPE_EGG,
                     "pending",
                     0,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_TYPE,
                                 "invalid enum type: GCutEgg"
                                 "(%" G_GSIZE_FORMAT ")",
                                 GCUT_TYPE_EGG)),
                 parse_test_data_free,
                 "unknown nick",
                 parse_enum_test_data_new(
                     CUT_TYPE_TEST_RESULT_STATUS,
                     "unknown",
                     0,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_FORMAT,
                                 "unknown enum value: "
                                 "<unknown>(CutTestResultStatus)")),
                 parse_test_data_free);
}

void
test_enum_parse (gconstpointer data)
{
    const ParseTestData *test_data = data;
    GError *error = NULL;
    gint actual_value;

    actual_value = gcut_enum_parse(test_data->type, test_data->value, &error);
    if (test_data->expected_error) {
        actual_error = error;
        gcut_assert_equal_error(test_data->expected_error, actual_error);
    } else {
        gcut_assert_error(error);
    }

    gcut_assert_equal_enum(test_data->type,
                           test_data->expected_enum_value,
                           actual_value);
}

void
test_flags_inspect (void)
{
    inspected = gcut_flags_inspect(flags_type, 0);
    cut_assert_equal_string("#<CuttestStubFlags>", inspected);

    g_free(inspected);
    inspected = gcut_flags_inspect(flags_type, (1 << 0) | (1 << 1));
    cut_assert_equal_string("#<CuttestStubFlags: "
                            "first|second "
                            "(CUTTEST_STUB_FIRST:0x1)|"
                            "(CUTTEST_STUB_SECOND:0x2)>",
                            inspected);

    g_free(inspected);
    inspected = gcut_flags_inspect(flags_type, (1 << 0) | (1 << 3));
    cut_assert_equal_string("#<CuttestStubFlags: "
                            "first "
                            "(CUTTEST_STUB_FIRST:0x1) "
                            "(unknown flags: 0x8)>",
                            inspected);
}

void
data_flags_parse (void)
{
    cut_add_data("nick",
                 parse_flags_test_data_new(flags_type,
                                           "first|second",
                                           (1 << 0 | 1 << 1),
                                           NULL),
                 parse_test_data_free,
                 "NICK",
                 parse_flags_test_data_new(flags_type,
                                           "SECOND|thIrd",
                                           (1 << 1 | 1 << 2),
                                           NULL),
                 parse_test_data_free,
                 "name",
                 parse_flags_test_data_new(flags_type,
                                           "CUTTEST_STUB_FIRST",
                                           (1 << 0),
                                           NULL),
                 parse_test_data_free,
                 "empty",
                 parse_flags_test_data_new(flags_type, "", 0, NULL),
                 parse_test_data_free,
                 "NULL",
                 parse_flags_test_data_new(
                     flags_type,
                     NULL,
                     0,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_FORMAT,
                                 "flags value should not be NULL")),
                 parse_test_data_free,
                 "invalid type",
                 parse_flags_test_data_new(
                     GCUT_TYPE_EGG,
                     "first",
                     0,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_TYPE,
                                 "invalid flags type: GCutEgg"
                                 "(%" G_GSIZE_FORMAT ")",
                                 GCUT_TYPE_EGG)),
                 parse_test_data_free,
                 "unknown nick only",
                 parse_flags_test_data_new(
                     flags_type,
                     "unknown",
                     0,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_FORMAT,
                                 "unknown flags: "
                                 "<unknown>(CuttestStubFlags): "
                                 "<unknown>: "
                                 "#<CuttestStubFlags>")),
                 parse_test_data_free,
                 "unknown nick with valid nick",
                 parse_flags_test_data_new(
                     flags_type,
                     "first|unknown1|CUTTEST_STUB_THIRD|unknown2",
                     (1 << 0 | 1 << 2),
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_FORMAT,
                                 "unknown flags: "
                                 "<unknown1|unknown2>(CuttestStubFlags): "
                                 "<first|unknown1|CUTTEST_STUB_THIRD|unknown2>: "
                                 "#<CuttestStubFlags: first|third "
                                 "(CUTTEST_STUB_FIRST:0x1)|"
                                 "(CUTTEST_STUB_THIRD:0x4)>")),
                 parse_test_data_free);
}

void
test_flags_parse (gconstpointer data)
{
    const ParseTestData *test_data = data;
    GError *error = NULL;
    guint actual_value;

    actual_value = gcut_flags_parse(test_data->type, test_data->value, &error);
    if (test_data->expected_error) {
        actual_error = error;
        gcut_assert_equal_error(test_data->expected_error, actual_error);
    } else {
        gcut_assert_error(error);
    }

    gcut_assert_equal_flags(test_data->type,
                            test_data->expected_flags_value,
                            actual_value);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
