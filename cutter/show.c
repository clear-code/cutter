/* Original:
 * CUnitTester http://www.gethos.net/opensource/cunit
 * show.c -- handles all output that the user sees
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
/* $Id: show.c 12 2004-09-13 09:05:58Z kou $ */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include <cutter/cutter.h>

#include "show.h"

#define CUTTER_BANNER "Cutter is based on CUnitTester by Stewart Gebbie <cunit@gethos.net> 2004"
#define CUNIT_URL "CUnitTester: http://www.gethos.net/opensource/cunit"

static int
can_use_escape(void)
{
    return (getenv("TERM") && 0 == strcmp(getenv("TERM") + 1, "term"));
}

static const char*
use_if_can(const char* str)
{
    if (can_use_escape()) {
        return str;
    } else {
        return "";
    }
}

static const char* colourRed(void) { return use_if_can("\033[01;31m"); }
static const char* colourGreen(void) { return use_if_can("\033[01;32m"); }
static const char* colourYellow(void) { return use_if_can("\033[01;33m"); }
static const char* colourBlue(void) { return use_if_can("\033[01;34m"); }
static const char* colourNormal(void) { return use_if_can("\033[00m"); }

static void
vl_printf(utest_world *world, enum VerboseLevel level, char *format, ...)
{
    if (world->verbose_level >= level) {
        va_list ap;

        va_start(ap, format);
        vprintf(format, ap);
        va_end(ap);
    }
}

void
ShowError (const char* fmt, ...)
{
    va_list ap;

    printf("%s", colourRed());
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("%s", colourNormal());
}

void
ShowMsg (const char* fmt, ...)
{
    va_list ap;

    printf("%s", colourYellow());
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("%s", colourNormal());
}

void
ShowUsage (int argc, char* argv[], const char* errmsg)
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
}


void
ShowTestInfo (utest_info* info)
{
    if (info->status == FALSE) {
        printf("%s/%s:%d: - %s %s - %s\n", 
               info->base,
               info->file,
               info->line, 
               info->suiteName, 
               info->testName, 
               info->msg);
    }
}


void
ShowInfo (utest_world* world)
{
    double percentGood;
    double percentBad;
    double runTime;

    utest_info* current;

    if (world->badTestCount > 0)
        printf("\n%sErrors%s:\n", colourYellow(), colourNormal());

    for (current = world->infoHead; current != NULL; current = current->next)
        ShowTestInfo(current);

    percentGood = 100.0*((double)world->goodTestCount/(double)world->testCount);
    percentBad = 100.0*((double)world->badTestCount/(double)world->testCount);

    vl_printf(world, NORMAL, "\n%sSummary%s:\n", colourYellow(), colourNormal());
    vl_printf(world, NORMAL, "\tTotal Tests %d\n", world->testCount);
    vl_printf(world, NORMAL, "\tGood Tests %d %3.2f%% \n", world->goodTestCount, percentGood);
    vl_printf(world, NORMAL, "\tBad Tests %d %3.2f%% \n", world->badTestCount, percentBad);

    /* calculate times */
    runTime = (world->secEnd + ((double)world->usecEnd/(double)1000000))
            - (world->secStart + ((double)world->usecStart/(double)1000000));
    vl_printf(world, NORMAL, "\tRun Time %3.3f secs\n", runTime);

    vl_printf(world, NORMAL,
              "\n%sBottom Line%s:\n\t", colourYellow(), colourNormal());
    vl_printf(world, PROGRESS,
              "\n%s[%d/%d]%s(%s%3.2f%%%s) tests passed.\n",
              colourBlue(),
              world->goodTestCount, 
              world->testCount, 
              colourNormal(),
              (world->goodTestCount == world->testCount ? colourGreen() : colourRed()),
              percentGood,
              colourNormal()
              );
}


void
ShowStart (utest_world* world)
{
    vl_printf(world, NORMAL,
              "There %s %d test%s in %d suite%s to run:\n", 
              world->testCount == 1 ? "is" : "are", 
              world->testCount, 
              world->testCount == 1 ? "" : "s", 
              world->suiteCount,
              world->suiteCount == 1 ? "" : "s"
              );
}

static char swizzle[]={
    '-','\\','|','/',
    '\0'
};

void
ShowProgress (utest_world* world)
{
    int perc = 0;
    int width = 0;
    int good = 0;
    int bad = 0;
    int blank = 0;
    const char* theColour = NULL;
    int i;

    if (world->verbose_level <= SILENT || world->testCount == 0)
        return;

    if (!can_use_escape()) {
        if (world->infoTail) {
            if (world->infoTail->status) {
                vl_printf(world, PROGRESS, ".");
            } else {
                vl_printf(world, PROGRESS, "E");
            }
        }
        return;
    }

    /* draw swizzle */
    printf("%c ", swizzle[world->finTestCount%(sizeof(swizzle)-1)]);

    /* draw % */
    printf("[%d/%d] ",world->finTestCount,world->testCount);

    /* draw % */
    perc = (int)(100*(double)world->finTestCount/(double)world->testCount);
    printf("%3d%% ",perc);

    /* draw progress bar */
    width=55;
    good = (int)ceil(width*((double)world->goodTestCount/
                (double)world->testCount));
    bad = (int)ceil(width*((double)world->badTestCount/
                (double)world->testCount));
    blank=width - (good + bad);/* - (good?1:0) - (bad?1:0); */

    theColour = (bad>0 ? colourRed() : colourGreen());

    printf("<%s", theColour);
    for (i = 0;i < bad;i++) printf("-");
    /* for(int i=0;i<err;i++) cout<<":"; */
    for (i = 0; i < good; i++) printf("=");
    if (perc != 100) {
        printf (">");
        blank--;
    }
    for (i = 0; i < blank; i++) printf(" ");
    printf("%s>\r", colourNormal());

#if 0
    printf("\n");

    printf ("t:%d f:%d g:%d b:%d\n",world->testCount, 
            world->finTestCount,
            world->goodTestCount, 
            world->badTestCount);
#endif

    /* end progress bar */
    fflush(stdout);
}


void
ShowBanner (utest_world* world)
{
    vl_printf(world, VERBOSE,
              "%s%s%s:\n", colourYellow(), CUTTER_BANNER, colourNormal());
    vl_printf(world, VERBOSE, "\t" CUNIT_URL "\n");
    vl_printf(world, VERBOSE, "Search Directory [%s]\n", world->rootDir);
}


void
ShowAddTest (utest_world *world, utest_suite_stub* stub)
{
    char path[100];
    char name[100];

    snprintf(path, sizeof(path), "[%-30.30s]", stub->libpath);
    snprintf(name, sizeof(path), "%s", stub->suite->name);
    vl_printf(world, NORMAL,
              "Adding suite %s %-20.20s %2d test%s\n", 
              path,
              name,
              stub->testCount,
              stub->testCount == 1 ? "" : "s");
}
/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
