/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string.h>
#include <locale.h>

#include <gcutter.h>
#include <cutter/cut-enum-types.h>
#include <cuttest-enum.h>

void data_get_enum(void);
void test_get_enum(gconstpointer data);
void data_get_flags(void);
void test_get_flags(gconstpointer data);

static GKeyFile *key_file;
static GError *actual_error;

static gchar *current_locale;

void
cut_setup (void)
{
    key_file = g_key_file_new();
    actual_error = NULL;

    current_locale = g_strdup(setlocale(LC_ALL, NULL));
    setlocale(LC_ALL, "C");
}

void
cut_teardown (void)
{
    if (current_locale) {
        setlocale(LC_ALL, current_locale);
        g_free(current_locale);
    } else {
        setlocale(LC_ALL, "");
    }

    if (key_file)
        g_key_file_free(key_file);

    if (actual_error)
        g_error_free(actual_error);
}

typedef struct _EnumTestData
{
    gchar *data;
    gchar *group_name;
    gchar *key_name;
    GType type;
    gint expected_enum_value;
    guint expected_flags_value;
    GError *expected_error;
} EnumTestData;

static EnumTestData *
enum_test_data_new (const gchar *data,
                    const gchar *group_name, const gchar *key_name,
                    GType type,
                    gint expected_enum_value,
                    guint expected_flags_value,
                    GError *expected_error)
{
    EnumTestData *test_data;

    test_data = g_new(EnumTestData, 1);
    test_data->data = g_strdup(data);
    test_data->group_name = g_strdup(group_name);
    test_data->key_name = g_strdup(key_name);
    test_data->type = type;
    test_data->expected_enum_value = expected_enum_value;
    test_data->expected_flags_value = expected_flags_value;
    test_data->expected_error = expected_error;

    return test_data;
}

static EnumTestData *
get_enum_test_data_new (const gchar *data,
                        const gchar *group_name, const gchar *key_name,
                        GType type, gint expected_enum_value,
                        GError *expected_error)
{
    return enum_test_data_new(data, group_name, key_name,
                              type, expected_enum_value, 0, expected_error);
}

static EnumTestData *
get_flags_test_data_new (const gchar *data,
                         const gchar *group_name, const gchar *key_name,
                         GType type, guint expected_flags_value,
                         GError *expected_error)
{
    return enum_test_data_new(data, group_name, key_name,
                              type, 0, expected_flags_value, expected_error);
}

static void
enum_test_data_free (EnumTestData *test_data)
{
    if (test_data->data)
        g_free(test_data->data);
    if (test_data->group_name)
        g_free(test_data->group_name);
    if (test_data->key_name)
        g_free(test_data->key_name);

    if (test_data->expected_error)
        g_error_free(test_data->expected_error);

    g_free(test_data);
}

void
data_get_enum (void)
{
    cut_add_data("exist",
                 get_enum_test_data_new(
                     "[group]\n"
                     "status=pending\n",
                     "group",
                     "status",
                     CUT_TYPE_TEST_RESULT_STATUS,
                     CUT_TEST_RESULT_PENDING,
                     NULL),
                 enum_test_data_free,
                 "nonexistent key",
                 get_enum_test_data_new(
                     "[group]\n"
                     "status=pending\n",
                     "group",
                     "unknown-key",
                     CUT_TYPE_TEST_RESULT_STATUS,
                     0,
                     g_error_new(G_KEY_FILE_ERROR,
                                 G_KEY_FILE_ERROR_KEY_NOT_FOUND,
                                 "Key file does not have key 'unknown-key'"
#if GLIB_CHECK_VERSION(2, 43, 0)
                                 " in group 'group'"
#endif
                                )),
                 enum_test_data_free,
                 "invalid enum value",
                 get_enum_test_data_new(
                     "[group]\n"
                     "status=unknown\n",
                     "group",
                     "status",
                     CUT_TYPE_TEST_RESULT_STATUS,
                     0,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_VALUE,
                                 "unknown enum value: "
                                 "<unknown>(CutTestResultStatus)")),
                 enum_test_data_free,
                 "invalid enum type",
                 get_enum_test_data_new(
                     "[group]\n"
                     "status=pending\n",
                     "group",
                     "status",
                     GCUT_TYPE_EGG,
                     0,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_TYPE,
                                 "invalid enum type: "
                                 "GCutEgg(%" G_GSIZE_FORMAT ")",
                                 GCUT_TYPE_EGG)),
                 enum_test_data_free);
}

void
test_get_enum (gconstpointer data)
{
    const EnumTestData *test_data = data;
    GError *error = NULL;
    gint enum_value;

    g_key_file_load_from_data(key_file, test_data->data, strlen(test_data->data),
                              0, &error);
    gcut_assert_error(error);

    enum_value = gcut_key_file_get_enum(key_file,
                                        test_data->group_name,
                                        test_data->key_name,
                                        test_data->type,
                                        &error);
    if (test_data->expected_error) {
        actual_error = error;
        gcut_assert_equal_error(test_data->expected_error, actual_error);
    } else {
        gcut_assert_error(error);
    }

    gcut_assert_equal_enum(test_data->type,
                           test_data->expected_enum_value,
                           enum_value);
}

void
data_get_flags (void)
{
    cut_add_data("exist",
                 get_flags_test_data_new(
                     "[group]\n"
                     "flags=first|third\n",
                     "group",
                     "flags",
                     CUTTEST_TYPE_FLAGS,
                     CUTTEST_FLAG_FIRST | CUTTEST_FLAG_THIRD,
                     NULL),
                 enum_test_data_free,
                 "nonexistent key",
                 get_flags_test_data_new(
                     "[group]\n"
                     "flags=second\n",
                     "group",
                     "unknown-key",
                     CUTTEST_TYPE_FLAGS,
                     0,
                     g_error_new(G_KEY_FILE_ERROR,
                                 G_KEY_FILE_ERROR_KEY_NOT_FOUND,
                                 "Key file does not have key 'unknown-key'"
#if GLIB_CHECK_VERSION(2, 43, 0)
                                 " in group 'group'"
#endif
                                )),
                 enum_test_data_free,
                 "invalid flags value",
                 get_flags_test_data_new(
                     "[group]\n"
                     "flags=First|unknown1|CUTTEST_FLAG_THIRD|unknown2\n",
                     "group",
                     "flags",
                     CUTTEST_TYPE_FLAGS,
                     CUTTEST_FLAG_FIRST | CUTTEST_FLAG_THIRD,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_VALUE,
                                 "unknown flags: "
                                 "<unknown1|unknown2>(CuttestFlags): "
                                 "<First|unknown1|CUTTEST_FLAG_THIRD|unknown2>: "
                                 "#<CuttestFlags: first|third "
                                 "(CUTTEST_FLAG_FIRST:0x1)|"
                                 "(CUTTEST_FLAG_THIRD:0x4)>")),
                 enum_test_data_free,
                 "invalid flags type",
                 get_flags_test_data_new(
                     "[group]\n"
                     "flags=second\n",
                     "group",
                     "flags",
                     GCUT_TYPE_EGG,
                     0,
                     g_error_new(GCUT_ENUM_ERROR,
                                 GCUT_ENUM_ERROR_INVALID_TYPE,
                                 "invalid flags type: "
                                 "GCutEgg(%" G_GSIZE_FORMAT ")",
                                 GCUT_TYPE_EGG)),
                 enum_test_data_free);
}

void
test_get_flags (gconstpointer data)
{
    const EnumTestData *test_data = data;
    GError *error = NULL;
    guint flags_value;

    g_key_file_load_from_data(key_file, test_data->data, strlen(test_data->data),
                              0, &error);
    gcut_assert_error(error);

    flags_value = gcut_key_file_get_flags(key_file,
                                          test_data->group_name,
                                          test_data->key_name,
                                          test_data->type,
                                          &error);
    if (test_data->expected_error) {
        actual_error = error;
        gcut_assert_equal_error(test_data->expected_error, actual_error);
    } else {
        gcut_assert_error(error);
    }

    gcut_assert_equal_flags(test_data->type,
                            test_data->expected_flags_value,
                            flags_value);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
