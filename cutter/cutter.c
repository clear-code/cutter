/* Original:
 * CUnitTester http://www.gethos.net/opensource/cunit
 * unittester.c -- provides the main() entry point to launch the tester
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
/* $Id: cutter.c 7 2004-09-10 10:07:43Z kou $ */
#include <stdlib.h>
#include <string.h>

#include <cutter/cutter.h>

#include "world.h"
#include "run.h"
#include "compile.h"
#include "info.h"
#include "show.h"

/* 
 * This is the entry point for the 'C' unit test program.
 *
 * This will search for all DSO with a name of the form libtest_....so
 * and comiple a list of test suites. After this it will run each
 * suite and hence each test in each suite.
 *
 */

#define STRNSAME_P(target, test_str) \
    (strncmp((target), (test_str), strlen(test_str)) == 0)

static int
show_usage (int argc, char* argv[], const char* errmsg)
{
    printf("Usage: %s [OPTIONS] [search directory]\n", argv[0]);
    printf("  OPTIONS:\n");
    printf("    -vLEVEL or --verbose=LEVEL\n");
    printf("      LEVEL: s[ilent], n[ormal], p[rogress], v[erbose]\n");
    printf("    -bBASE or --base=BASE\n");
    printf("      BASE: base directory of tests. use when error is occurred\n");

    if (errmsg) printf("Error: %s\n", errmsg);
    printf("\n%s\n", CUTTER_BANNER);
    printf(
           "\n"
           "\tStarting in the specified search directory, Cutter\n"
           "\twill recurse through the sub directories and search for\n"
           "\tfiles with names matching libtest_*.so. The test suites\n"
           "\tdefined within these libraries are then loaded and run.\n"
           "\tThe final statistics are displayed along with any\n"
           "\taccumulated error messages.\n"
          );
    exit(1);
}


static bool
set_verbose_level(utest_world *world, const char *level) 
{
    if ((STRNSAME_P(level, "s") || (STRNSAME_P(level, "silent")))) {
        world->verbose_level = SILENT;
    } else if ((STRNSAME_P(level, "n") || (STRNSAME_P(level, "normal")))) {
        world->verbose_level = NORMAL;
    } else if ((STRNSAME_P(level, "p") || (STRNSAME_P(level, "progress")))) {
        world->verbose_level = PROGRESS;
    } else if ((STRNSAME_P(level, "v") || (STRNSAME_P(level, "verbose")))) {
        world->verbose_level = VERBOSE;
    } else {
        return FALSE;
    }
    return TRUE;
}

static void
show_usage_with_invalid_verbose_type(const char *type, int argc, char **argv)
{
    const char *message = "Invalid verbose type: ";
    int len = strlen(type);
    char *buffer;

    buffer = (char *)malloc(strlen(message) + len + 1);
    buffer = strdup(message);
    buffer = strncat(buffer, type, len);
    show_usage(argc, argv, buffer);
}

static bool
handle_verbose_arg(utest_world *world, const char *level, int argc, char **argv)
{
    const char *v = "-v";
    const char *verbose = "--verbose=";
    const char *type;

    if STRNSAME_P(level, v) {
        type = level + strlen(v);
        if (!set_verbose_level(world, type)) {
            show_usage_with_invalid_verbose_type(type, argc, argv);
        }
    } else if STRNSAME_P(level, verbose) {
        type = level + strlen(verbose);
        if (!set_verbose_level(world, type)) {
            show_usage_with_invalid_verbose_type(type, argc, argv);
        }
    } else {
        return FALSE;
    }
    return TRUE;
}

static bool
handle_base_arg(utest_world *world, const char *arg)
{
    const char *b = "-b";
    const char *base = "--base=";
    const char *where;

    if STRNSAME_P(arg, b) {
        where = arg + strlen(b);
        strncpy(world->base, where, MAX_UTEST_BASE_LEN);
    } else if STRNSAME_P(arg, base) {
        where = arg + strlen(base);
        strncpy(world->base, where, MAX_UTEST_BASE_LEN);
    } else {
        return FALSE;
    }
    return TRUE;
}

/* compiles a list of test suites and then runs them */
static int
RunTester (int argc, char* argv[])
{
    utest_world world;
    int i;

    InitWorld(&world);

    /* check command line parameters */
    for (i = 1; i < argc; i++) {
        if (STRNSAME_P(argv[i], "-h") || STRNSAME_P(argv[i], "--help")) {
            show_usage(argc, argv, NULL);
        } else if (handle_verbose_arg(&world, argv[i], argc, argv)) {
            /* do nothing */
        } else if (handle_base_arg(&world, argv[i])) {
            /* do nothing */
        } else {
            if (argv[i + 1] == NULL) {
                world.rootDir = argv[i];
            } else {
                show_usage(argc, argv, "Incorrect number of arguments");
            }
        }
    }

    /* banner */
    ShowBanner(&world);

    CompileWorld(&world);

    /* init timers */
    InitWorldTimers(&world);

    RunWorld(&world);

    /* final timers */
    FiniWorldTimers(&world);

    ShowProgress(&world);
    ShowInfo(&world);

    return (world.badTestCount ? 1 : 0);
}

int
main(int argc, char* argv[])
{
    int ret;

    ret = RunTester(argc, argv);

    return ret;
}
/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
