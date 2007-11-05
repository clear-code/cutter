#include <cut.h>

#include "calc.h"

void test_add_function(void);
void test_sub_function(void);

void
test_add_function(void)
{
  cut_assert_equal_int(3, add(1, 2));
  cut_assert_equal_int(1, add(3, -2));
}

void
test_sub_function(void)
{
  cut_assert_equal_int(1, sub(3, 2));
}

