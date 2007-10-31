/* Original:
 * 
 * show.h 
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
#ifndef CUTTER_SHOW_H
#define CUTTER_SHOW_H

#include "world.h"

void ShowError (const char* fmt,...);
void ShowMsg (const char* fmt,...);

void ShowBanner (utest_world* world);
void ShowInfo (utest_world* world);
void ShowStart (utest_world* world);
void ShowProgress (utest_world* world);

void ShowTestInfo (utest_info* info);

void ShowAddTest (utest_world* world, utest_suite_stub* stub);

#endif /* CUTTER_SHOW_H */
