#include <cutter.h>

void test_abcdefghijklmnopqratuvwzyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789 (void);
void test_stub_function1 (void);
void test_stub_function2 (void);
void test_stub_function3 (void);

/*
int  test_invalid_return_value_function (void);
void test_invalid_argument_type_function (int arugment);
*/
void text_invalid_prefix_function (void);

void
test_stub_function1 (void)
{
}

void
test_stub_function2 (void)
{
}

void
test_stub_function3 (void)
{
}

void
test_abcdefghijklmnopqratuvwzyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789 (void)
{
}

static void
test_static_function (void)
{
    cut_error("This function is declared as static, should not be loaded!");
}

void
text_invalid_prefix_function (void)
{
    cut_error("This test name does not have \"test_\" prefix,"
              "should not be loaded!");
    test_static_function(); /* must not called! */
}

/*
vi:nowrap:ai:expandtab:sw=4
*/
