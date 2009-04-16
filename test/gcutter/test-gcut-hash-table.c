/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gcutter.h>

void test_new(void);
void test_equal_same_content(void);
void test_equal_same_size_and_different_content(void);
void test_equal_different_size(void);
void test_equal_null(void);
void test_equal_empty(void);
void test_inspect(void);
void test_inspect_empty(void);
void test_inspect_null(void);

static GHashTable *hash1, *hash2;
static gchar *inspected;

void
cut_setup (void)
{
    hash1 = NULL;
    hash2 = NULL;

    inspected = NULL;
}

void
cut_teardown (void)
{
    if (hash1)
        g_hash_table_unref(hash1);
    if (hash2)
        g_hash_table_unref(hash2);

    if (inspected)
        g_free(inspected);
}

static GHashTable *
stub_gcut_hash_table_string_string_new_va_list (const gchar *key, ...)
{
    GHashTable *hash;
    va_list args;

    va_start(args, key);
    hash = gcut_hash_table_string_string_new_va_list(key, args);
    va_end(args);

    return hash;
}

void
test_new (void)
{
    hash1 = gcut_hash_table_string_string_new("key1", "value1",
                                              "key2", "value2",
                                              "key3", "value3",
                                              NULL);
    hash2 = stub_gcut_hash_table_string_string_new_va_list("key1", "value1",
                                                           "key2", "value2",
                                                           "key3", "value3",
                                                           NULL);

    cut_assert_true(gcut_hash_table_string_equal(hash1, hash2));
}

void
test_equal_same_content (void)
{
    hash1 = gcut_hash_table_string_string_new("key1", "value1",
                                              "key2", "value2",
                                              NULL);
    hash2 = gcut_hash_table_string_string_new("key1", "value1",
                                              "key2", "value2",
                                              NULL);

    cut_assert_true(gcut_hash_table_string_equal(hash1, hash2));
}

void
test_equal_same_size_and_different_content (void)
{
    hash1 = gcut_hash_table_string_string_new("key1", "value1",
                                              "key2", "value2",
                                              NULL);
    hash2 = gcut_hash_table_string_string_new("key1", "value1",
                                              "key22", "value22",
                                              NULL);

    cut_assert_false(gcut_hash_table_string_equal(hash1, hash2));
}

void
test_equal_different_size (void)
{
    hash1 = gcut_hash_table_string_string_new("key1", "value1",
                                              "key2", "value2",
                                              NULL);
    hash2 = gcut_hash_table_string_string_new("key1", "value1",
                                              NULL);

    cut_assert_false(gcut_hash_table_string_equal(hash1, hash2));
}

void
test_equal_empty (void)
{
    hash1 = gcut_hash_table_string_string_new(NULL, NULL);
    hash2 = gcut_hash_table_string_string_new(NULL, NULL);
    cut_assert_true(gcut_hash_table_string_equal(hash1, hash2));
}

void
test_equal_null (void)
{
    cut_assert_true(gcut_hash_table_string_equal(NULL, NULL));
}

void
test_inspect (void)
{
    hash1 = gcut_hash_table_string_string_new("key1", "value1",
                                              "key2", "value2",
                                              NULL);
    inspected = gcut_hash_table_string_string_inspect(hash1);
    cut_assert_equal_string("{\"key1\" => \"value1\", \"key2\" => \"value2\"}",
                            inspected);
}

void
test_inspect_empty (void)
{
    hash1 = gcut_hash_table_string_string_new(NULL, NULL);
    inspected = gcut_hash_table_string_string_inspect(hash1);
    cut_assert_equal_string("{}", inspected);
}

void
test_inspect_null (void)
{
    inspected = gcut_hash_table_string_string_inspect(NULL);
    cut_assert_equal_string("(null)", inspected);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
