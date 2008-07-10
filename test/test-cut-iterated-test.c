#include <cutter.h>
#include <cutter/cut-test.h>
#include <cutter/cut-test-runner.h>

void data_iterated_test(void);
void test_iterated_test(void *data);

void
data_iterated_test (void)
{
    cut_add_data("First", g_strdup("XXX1"), g_free,
                 "Second", g_strdup("XXX2"), g_free,
                 "Third", g_strdup("XXX3"), g_free);
}

void
test_iterated_test (void *data)
{
    gchar *string = data;

    /* FAIL THIS TEST!!! */
    cut_assert_equal_string("XXX1", string);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
