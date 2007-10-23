/* Original:
 * 
 * info.h 
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
#ifndef CUTTER_INFO_H
#define CUTTER_INFO_H

#include "world.h"

void UTestFillInfo (utest_info* info, int line, const char* file, const char* msg);
void AddInfo (utest_world* world, utest_info* info);
void InitInfo (utest_info* info, const char* suiteName, const char* testName);

#endif /* CUTTER_INFO_H */
