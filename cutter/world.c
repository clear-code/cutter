/* Original:
 * CUnitTester http://www.gethos.net/opensource/cunit
 * world.c -- top level context information containing stats and suites
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
/* $Id: world.c 7 2004-09-10 10:07:43Z kou $ */
#include <sys/time.h>

#include <cutter/cutter.h>

#include "world.h"
#include "info.h"

/* initialises the run config and counters */
void InitWorld (utest_world* world)
{
	world->rootDir = ".";

  world->base[0] = '\0';

	world->suiteCount = 0;
	world->testCount = 0;

	world->finSuiteCount = 0;
	world->finTestCount = 0;

	world->badTestCount = 0;
	world->goodTestCount = 0;

	world->addstub = NULL;
	world->stubHead = NULL;
	world->stubTail = NULL;

	world->addinfo = &AddInfo;
	world->infoHead = NULL;
	world->infoTail = NULL;

  world->verbose_level = NORMAL;
}


void InitWorldTimers (utest_world* world)
{
	/* set timers */
	struct timeval tv;
	gettimeofday(&tv, NULL);
	world->secStart = tv.tv_sec;
	world->usecStart = tv.tv_usec;
}

void FiniWorldTimers (utest_world* world)
{
	/* final timers */
	struct timeval tv;
	gettimeofday(&tv, NULL);
	world->secEnd = tv.tv_sec;
	world->usecEnd = tv.tv_usec;
}
