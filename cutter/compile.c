/* Original:
 * CUnitTester http://www.gethos.net/opensource/cunit
 * compile.c -- builds a list of suites and tests
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
/* $Id: compile.c 7 2004-09-10 10:07:43Z kou $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_ALLOCA
#include <alloca.h>
#endif

#include <cutter/cutter.h>

#include "load.h"
#include "compile.h"
#include "show.h"


/*
 * Compiling:
 * search for test suites and add them to the worl
 */

void RecurseDir(utest_world* world, const char* path, int depth)
{
	DIR* currdir;
	struct dirent* dent;
	int ret;

	currdir = opendir(path);
	if (!currdir) 
	{
		ShowError (
				"ERROR: opendir(%s) failed to open search directory: %s\n", 
				path, strerror (errno));
	}
	uassert (currdir);

	for (dent = readdir(currdir); dent !=NULL; dent = readdir(currdir))
	{
		int fnm;
		size_t flen = strlen(path) + strlen(dent->d_name)+2;
		char* fpath = alloca(flen);
		uassert (fpath);

		snprintf(fpath,flen,"%s/%s", path, dent->d_name);
		fnm = fnmatch("libtest_*.so",dent->d_name,0);
		uassert (fnm == 0 || fnm == FNM_NOMATCH);
		if (fnm == 0)
		{/* add to world */
			AddSuite (world, fpath, path);
		}
		if (dent->d_name[0] != '.')
		{
			struct stat sbuf;
			int ret;

			ret = stat(fpath, &sbuf);
			if (ret != 0)
			{
				ShowError (
						"ERROR: stat(%s,...) failed during suite search: %s\n", 
						fpath, strerror (errno));
				if (errno == ENOENT) continue;
			}
			uassert (ret == 0);

			if (S_ISDIR(sbuf.st_mode))
				RecurseDir(world, fpath, depth+1);
		}
	}

	ret = closedir(currdir);
	uassert (ret == 0);
}


void CompileWorld (utest_world* world)
{
	RecurseDir(world, world->rootDir?world->rootDir:".", 0);
}
