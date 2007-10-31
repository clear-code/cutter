/* Original:
 * CUnitTester http://www.gethos.net/opensource/cunit
 * load.c -- loads test suites from shared objects
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
/* $Id: load.c 7 2004-09-10 10:07:43Z kou $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "cutter.h"

#include "load.h"
#include "show.h"

int CountTests (utest_suite *suite);

/* count how many tests there are in a test suite */
int
CountTests (utest_suite *suite)
{
    int c;
    for (c = 0; suite->tests[c].theTest != NULL; c++);
    return c;
}



utest_suite*
LoadSuite (const char* tpath, utest_suitelib* sl)
{
    /* load the library */
    utest_suite* (*reghandle)(void) = NULL;
    utest_suite* suite = NULL;
    const char* derr = NULL;
    void* handle = dlopen(tpath,RTLD_LAZY);
    derr = dlerror();
    if (derr) {
        ShowError("dlopen() failed: %s\n", derr);
    }
    uassert(handle);

    /* retrieve the registration function */
    reghandle = NULL;
    reghandle = (utest_suite* (*)(void))dlsym(handle, "register_suite");
    derr = dlerror();
    if (derr) {
        ShowError("dlsym() failed: %s\n", derr);
        dlclose(handle);
        abort();
    }

    /* run the registration function to obtain an test suite instance */
    suite=(*reghandle)();
    uassert(suite!=NULL);

    sl->suite = suite;
    sl->handle = handle;

    return suite;
}


void
UnloadSuite (utest_suitelib* sl)
{
    uassert(sl->handle);
    dlclose(sl->handle);
}


/* search for DSOs with names of the form libtest....so and */
/* build up the world of tests. */
void
AddSuite (utest_world* world, const char* libpath, const char *dir)
{
    utest_suitelib sl;
    utest_suite* suite;
    utest_suite_stub* stub = (utest_suite_stub*)malloc(sizeof(utest_suite_stub));

    /* load DSO */
    suite = LoadSuite(libpath, &sl);

    stub->suite = suite;
    stub->testCount = CountTests(stub->suite);
    strncpy(stub->libpath, libpath, MAXUTESTPATHLEN);
    stub->next = NULL;

    ShowAddTest(world, stub);

    world->suiteCount += 1;
    world->testCount += stub->testCount;
    if (world->stubHead == NULL) {
        world->stubHead = stub;
        world->stubTail = stub;
    } else {
        world->stubTail->next = stub;
        world->stubTail = stub;
    }

    /* unload DSO */
    UnloadSuite(&sl);
}
/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
