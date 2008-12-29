/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>
#include <cutter/cut-enum-types.h>
#include <cuttest-enum.h>

void test_enum_inspect(void);
void data_enum_parse(void);
void test_enum_parse(gconstpointer data);
void test_flags_inspect(void);
void data_flags_parse(void);
void test_flags_parse(gconstpointer data);
void data_flags_get_all(void);
void test_flags_get_all(gconstpointer _data);

static gchar *inspected;

static GError *actual_error;

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
                                 GCUT_ENUM_ERROR_INVALID_VALUE,
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
                                 GCUT_ENUM_ERROR_INVALID_VALUE,
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
    inspected = gcut_flags_inspect(CUTTEST_TYPE_FLAGS, 0);
    cut_assert_equal_string("#<CuttestFlags>", inspected);

    g_free(inspected);
    inspected = gcut_flags_inspect(CUTTEST_TYPE_FLAGS,
                                   CUTTEST_FLAG_FIRST | CUTTEST_FLAG_SECOND);
    cut_assert_equal_string("#<CuttestFlags: "
                            "first|second "
                            "(CUTTEST_FLAG_FIRST:0x1)|"
                            "(CUTTEST_FLAG_SECOND:0x2)>",
                            inspected);

    g_free(inspected);
    inspected = gcut_flags_inspect(CUTTEST_TYPE_FLAGS,
                                   CUTTEST_FLAG_FIRST |
                                   (CUTTEST_FLAG_THIRD << 1));
    cut_assert_equal_string("#<CuttestFlags: "
                            "first "
                            "(CUTTEST_FLAG_FIRST:0x1) "
                            "(unknown flags: 0x8)>",
                            inspected);
}

void
data_flags_parse (void)
{
    cut_add_data("nick",
                 parse_flags_test_data_new(CUTTEST_TYPE_FLAGS,
                                           "first|second",
                                           CUTTEST_FLAG_FIRST |
                                           CUTTEST_FLAG_SECOND,
                                           NULL),
                 parse_test_data_free,
                 "NICK",
                 parse_flags_test_data_new(CUTTEST_TYPE_FLAGS,
                                           "SECOND|thIrd",
                                           CUTTEST_FLAG_SECOND |
                                           CUTTEST_FLAG_THIRD,
                                           NULL),
                 parse_test_data_free,
                 "name",
                 parse_flags_test_data_new(CUTTEST_TYPE_FLAGS,
                                           "CUTTEST_FLAG_FIRST",
                                           CUTTEST_FLAG_FIRST,
                                           NULL),
                 parse_test_data_free,
                 "empty",
                 parse_flags_test_data_new(CUTTEST_TYPE_FLAGS, "", 0, NULL),
                 parse_test_data_free,
                 "NULL",
                 parse_flags_test_data_new(
                     CUTTEST_TYPE_FLAGS,
                     NULL,
                     0,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_VALUE,
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
                     CUTTEST_TYPE_FLAGS,
                     "unknown",
                     0,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_VALUE,
                                 "unknown flags: "
                                 "<unknown>(CuttestFlags): "
                                 "<unknown>: #<CuttestFlags>")),
                 parse_test_data_free,
                 "unknown nick with valid nick",
                 parse_flags_test_data_new(
                     CUTTEST_TYPE_FLAGS,
                     "first|unknown1|CUTTEST_FLAG_THIRD|unknown2",
                     (1 << 0 | 1 << 2),
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_VALUE,
                                 "unknown flags: "
                                 "<unknown1|unknown2>(CuttestFlags): "
                                 "<first|unknown1|CUTTEST_FLAG_THIRD|unknown2>: "
                                 "#<CuttestFlags: first|third "
                                 "(CUTTEST_FLAG_FIRST:0x1)|"
                                 "(CUTTEST_FLAG_THIRD:0x4)>")),
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

void
data_flags_get_all (void)
{
#define ADD(label, expected, type, error)                               \
    gcut_add_datum(label,                                               \
                   "expected", G_TYPE_UINT, expected,                   \
                   "type", G_TYPE_GTYPE, type,                          \
                   "error", G_TYPE_POINTER, error, g_error_free,        \
                   NULL)

    ADD("success",
        CUTTEST_FLAG_FIRST | CUTTEST_FLAG_SECOND | CUTTEST_FLAG_THIRD,
        CUTTEST_TYPE_FLAGS,
        NULL);
    ADD("invalid type",
        0,
        GCUT_TYPE_EGG,
        g_error_new(GCUT_ENUM_ERROR,
                    GCUT_ENUM_ERROR_INVALID_TYPE,
                    "invalid flags type: GCutEgg"
                    "(%" G_GSIZE_FORMAT ")",
                    GCUT_TYPE_EGG));
#undef ADD
}

void
test_flags_get_all (gconstpointer _data)
{
    const GCutData *data = _data;
    GType flags_type;
    GError *error = NULL;
    const GError *expected_error;
    guint actual_value;

    flags_type = gcut_data_get_type(data, "type");
    actual_value = gcut_flags_get_all(flags_type, &error);
    expected_error = gcut_data_get_pointer(data, "error");
    if (expected_error) {
        gcut_take_error(error);
        gcut_assert_equal_error(expected_error, error);
    } else {
        gcut_assert_error(error);
    }

    gcut_assert_equal_flags(flags_type,
                            gcut_data_get_flags(data, "expected"),
                            actual_value);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
