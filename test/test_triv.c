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

CUT_DEFINE_TEST(ATest)
{
  CUT_PASS;
}

CUT_DEFINE_TEST(BTest)
{
  CUT_ASSERT (FALSE, "This is a Bad Test");

  CUT_PASS;
}

CUT_DEFINE_TEST(CTest)
{
  CUT_FAIL ("This test is doomed to fail");

  CUT_PASS;
}

CUT_DEFINE_TEST(DTest)
{
  CUT_ASSERT_EQUAL_DOUBLE(0.5, 0.1, 0.55, "equal double");
  
  CUT_PASS;
}

CUT_REGISTER_BEGIN("Example::Dummy")
CUT_REGISTER(ATest, "Good Test")
CUT_REGISTER(BTest, "Bad Test")
CUT_REGISTER(CTest, "Fail Test")
CUT_REGISTER(DTest, "Double Test")
CUT_REGISTER_END
