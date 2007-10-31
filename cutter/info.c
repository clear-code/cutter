/* Original:
 * CUnitTester http://www.gethos.net/opensource/cunit
 * info.c -- manages info produced by running tests
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
/* $Id: info.c 7 2004-09-10 10:07:43Z kou $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cutter/cutter.h>

#include "info.h"

/* TODO UTestFillInfo could be passed as a function pointer in the info struct */
void
UTestFillInfo (utest_info* info, int line, const char* file, const char* msg)
{
    uassert(info);
    uassert(file);
    uassert(msg);

    info->line = line;
    strncpy(info->msg, msg, MAXUTESTMESGLEN);
    strncpy(info->file, file, MAXUTESTFILELEN);
}

void
AddInfo (utest_world* world, utest_info* info)
{
    utest_info* copy = (utest_info*)malloc(sizeof(utest_info));

    copy->line = info->line;
    copy->status = info->status;
    strncpy(copy->msg, info->msg, MAXUTESTMESGLEN);
    strncpy(copy->file, info->file, MAXUTESTFILELEN);
    strncpy(copy->base, info->base, MAX_UTEST_BASE_LEN);
    strncpy(copy->suiteName, info->suiteName, MAXUTESTNAMELEN);
    strncpy(copy->testName, info->testName, MAXUTESTNAMELEN);
    copy->next = NULL;

    if (world->infoHead == NULL) {
        world->infoHead = copy;
        world->infoTail = copy;
    } else {
        world->infoTail->next = copy;
        world->infoTail = copy;
    }

    world->finTestCount++;
    if (info->status)
        world->goodTestCount++;
    else
        world->badTestCount++;

}

void
InitInfo (utest_info* info, const char* suiteName, const char* testName)
{
    info->line = 0; 
    info->file[0] = '\0'; 
    info->base[0] = '\0';
    info->msg[0] = '\0';

    strncpy(info->suiteName, suiteName, MAXUTESTNAMELEN);
    strncpy(info->testName, testName, MAXUTESTNAMELEN);

    info->logerror = &UTestFillInfo;
}
/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
