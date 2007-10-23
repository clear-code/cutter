#include <cutter/cutter.h>

#include "calc.h"

UT_DEF(add_test)
{
  UT_ASSERT_EQUAL_INT(3, add(1, 2), "1 + 2");
  UT_ASSERT_EQUAL_INT(1, add(3, -2), "3 + -2");
  UT_PASS;
}

UT_DEF(sub_test)
{
  UT_ASSERT_EQUAL_INT(1, sub(3, 2), "3 - 2");
  UT_PASS;
}

UT_REGISTER_BEGIN("calc test")
UT_REGISTER(add_test, "add test")
UT_REGISTER(sub_test, "sub test")
UT_REGISTER_END
