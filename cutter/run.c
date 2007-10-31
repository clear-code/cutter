/* Original:
 * CUnitTester http://www.gethos.net/opensource/cunit
 * run.c -- runs the actual tests in all the suites
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
/* $Id: run.c 10 2004-09-12 09:59:51Z kou $ */
#include <stdio.h>
#include <stdlib.h>

#include <cutter/cutter.h>

#include "run.h"
#include "info.h"
#include "load.h"
#include "show.h"

int RunSuite (utest_world* world, utest_suite_stub* stub);

static void
init_info_with_base(utest_info* info,
                    const char* suiteName,
                    const char* testName,
                    const char* base)
{
    InitInfo(info, suiteName, testName);
    if (strlen(base) == 0) {
        strncpy(info->base, ".", MAX_UTEST_BASE_LEN);
    } else {
        strncpy(info->base, base, MAX_UTEST_BASE_LEN);
    }
}


int
RunSuite (utest_world* world, utest_suite_stub* stub)
{
    int t;
    int testStatus;
    utest_suite* suite = stub->suite;
    utest_info uinfo;

    /* run suite initializer */
    init_info_with_base(&uinfo, suite->name, "Initializer", world->base);
    if (suite->initializer && !suite->initializer (&uinfo))
        goto suite_init_bad;
    /* run each test */
    for (t = 0; t < stub->testCount; t++) {
        ShowProgress(world);

        /* run setup */
        init_info_with_base(&uinfo, suite->name, "Setup", world->base);
        if (suite->setup && !suite->setup (&uinfo))
            goto stage_setup_bad;

        /* run test */
        init_info_with_base(&uinfo, suite->name, suite->tests[t].name, world->base);
        uassert(suite->tests[t].theTest != NULL);
        testStatus = suite->tests[t].theTest (&uinfo);

        uinfo.status = testStatus;

        world->addinfo(world, &uinfo);

        /* run teardown */
        init_info_with_base(&uinfo, suite->name, "Teardown", world->base);
        if (suite->teardown && !suite->teardown (&uinfo))
            goto stage_teardown_bad;
    }
    /* run suite finialiser */
    init_info_with_base(&uinfo, suite->name, "Finalizer", world->base);
    if (suite->finalizer && !suite->finalizer (&uinfo))
        goto suite_fini_bad;

    /* end of suite suiteok: */
    world->finSuiteCount++;
    return TRUE;

stage_setup_bad:
    ShowError("\nERROR: test setup failed for suite: %s\n", suite->name);
    return FALSE;
stage_teardown_bad:
    ShowError("\nERROR: test teardown failed for suite: %s\n", suite->name);
    return FALSE;
suite_init_bad:
    ShowError("\nERROR: suite init failed for suite: %s\n", suite->name);
    return FALSE;
suite_fini_bad:
    ShowError("\nERROR: suite fini failed for suite: %s\n", suite->name);
    return FALSE;
}


/* run each suite in the world */
void
RunWorld (utest_world* world)
{
    utest_suite_stub *current = NULL;

    ShowStart(world);
    if (world->testCount == 0) {
        ShowMsg("There are no tests to run.\n");
        return;
    }

    for (current = world->stubHead; current != NULL; current = current->next) {
        utest_suitelib sl;
        utest_suite* suite;

        suite = LoadSuite(current->libpath, &sl);
        uassert(suite);

        /* ensure that we reset the suite pointer as it may have 
         * changed after the first dlopen()
         * (thanks Jaco Breitenbach <jjb@herfs.net>)
         */
        current->suite = suite;

        RunSuite(world, current);

        UnloadSuite(&sl);
    }

#if 0
    if (world->testCount != world->finTestCount)
        ShowError ("\nNOT ALL TESTS RAN! expected %d but only %d completed\n",
                world->testCount, world->finTestCount);
    if (world->suiteCount != world->finSuiteCount)
        ShowError ("\nNOT ALL SUITES RAN! expected %d but only %d completed\n",
                world->suiteCount, world->finSuiteCount);
#endif
} 
/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
