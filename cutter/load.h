/* Original:
 * 
 * load.h 
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
#ifndef CUTTER_LOAD_H
#define CUTTER_LOAD_H

#include "world.h"

typedef struct utest_suitelib_tag
{
	utest_suite* suite;
	void* handle;
} utest_suitelib;


utest_suite* LoadSuite (const char* tpath, utest_suitelib* sl);
void UnloadSuite (utest_suitelib* sl);

void AddSuite (utest_world* world, const char* libpath, const char *dir);

#endif /* CUTTER_LOAD_H */
