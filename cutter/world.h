/* Original:
 * 
 * world.h -- include needed by all test suites
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
/* $Id: world.h 7 2004-09-10 10:07:43Z kou $ */
#ifndef CUTTER_WORLD_H
#define CUTTER_WORLD_H

#include <cutter/cutter.h>

typedef struct utest_suite_stub_tag
{
	char libpath[MAXUTESTPATHLEN];
	int testCount;
	utest_suite* suite;
	struct utest_suite_stub_tag* next;
} utest_suite_stub;

typedef struct utest_world_tag
{
	/* config */
	const char* rootDir;
	
  char base[MAX_UTEST_BASE_LEN];
  
	/* test counters */
	int suiteCount;
	int testCount;

	int finSuiteCount;
	int finTestCount;

	int badTestCount;
	int goodTestCount;

	/* times */
	long secStart;
	long usecStart;
	long secEnd;
	long usecEnd;

	/* tests */
	void (*addstub)(struct utest_world_tag*, utest_suite_stub*);
	utest_suite_stub* stubHead;
	utest_suite_stub* stubTail;

	void (*addinfo)(struct utest_world_tag*, utest_info*);
	utest_info* infoHead;
	utest_info* infoTail;

  enum VerboseLevel verbose_level;
} utest_world;

void InitWorld (utest_world* world);
void InitWorldTimers (utest_world* world);
void FiniWorldTimers (utest_world* world);

#endif /* CUTTER_WORLD_H */
