#include <cutter/cutter.h>

#include "calc.h"

CUT_DEFINE_TEST(add_test)
{
  CUT_ASSERT_EQUAL_INT(3, add(1, 2), "1 + 2");
  CUT_ASSERT_EQUAL_INT(1, add(3, -2), "3 + -2");
  CUT_PASS;
}

CUT_DEFINE_TEST(sub_test)
{
  CUT_ASSERT_EQUAL_INT(1, sub(3, 2), "3 - 2");
  CUT_PASS;
}

CUT_REGISTER_BEGIN("calc test")
CUT_REGISTER(add_test, "add test")
CUT_REGISTER(sub_test, "sub test")
CUT_REGISTER_END
