/* Original:
 * CUnitTester http://www.gethos.net/opensource/cunit
 * trivsuite.c -- an example test suite
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
/* $Id: test_triv.c 20 2004-11-05 12:05:36Z kou $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cutter/cutter.h>

/* an example test suite */

UT_DEF(ATest)
{
  UT_PASS;
}

UT_DEF(BTest)
{
  UT_ASSERT (FALSE, "This is a Bad Test");

  UT_PASS;
}

UT_DEF(CTest)
{
  UT_FAIL ("This test is doomed to fail");

  UT_PASS;
}

UT_DEF(DTest)
{
  UT_ASSERT_EQUAL_DOUBLE(0.5, 0.1, 0.55, "equal double");
  
  UT_PASS;
}

UT_REGISTER_BEGIN("Example::Dummy")
UT_REGISTER(ATest, "Good Test")
UT_REGISTER(BTest, "Bad Test")
UT_REGISTER(CTest, "Fail Test")
UT_REGISTER(DTest, "Double Test")
UT_REGISTER_END
