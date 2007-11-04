#include "cut.h"
#include <stdio.h>

void test_dummy_function1 (void);
void test_dummy_function2 (void);
void test_dummy_function3 (void);
void test_abcdefghijklmnopqratuvwzyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789 (void);
gboolean test_invalid_return_value_function (void);
void test_invalid_argument_type_function (gboolean arugment);
void text_invalid_prefix_function (void);

void
test_dummy_function1 (void)
{
}

void
test_dummy_function2 (void)
{
}

void
test_dummy_function3 (void)
{
}

void
test_abcdefghijklmnopqratuvwzyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789 (void)
{
}

static void
test_static_function (void)
{
    printf("This function is declared as static, should not be loaded!");
}

gboolean
test_invalid_return_value_function (void)
{
    printf("This test returns boolean value, should not be loaded!");
    return FALSE;
}

void
test_invalid_argument_type_function (gboolean argument)
{
    printf("This test has an boolean argument, should not be loaded!");
}

void
text_invalid_prefix_function (void)
{
    printf("This test name does not have \"test_\" prefix, should not be loaded!");
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
