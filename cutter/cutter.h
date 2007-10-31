/* Original:
 * 
 * unittester.h -- include needed by all test suites
 *
 * Copyright 2003 by Stewart Gebbie
 * For license terms, see the file COPYING in the top directory.
 */
/* $Id: cutter.h 12 2004-09-13 09:05:58Z kou $ */
#ifndef CUTTER_CUTTER_H
#define CUTTER_CUTTER_H

#include <glib.h>

enum VerboseLevel {
    SILENT,
    PROGRESS,
    NORMAL,
    VERBOSE
};

#define uassert(exp) do {                         \
    if (!(exp))                                   \
    {                                             \
        fprintf(stderr,                           \
                "Assertion (%s) failed %s:%d\n",  \
                (exp), __FILE__, __LINE__);       \
        abort();                                  \
    }                                             \
} while (0)

#define UT_INFO _ut_info
#define UT_SUITE _ut_suite
#define UT_NAME _ut_name

#include <cutter/assersions.h>

#define UT_INITIALIZER NULL
#define UT_FINALIZER NULL
#define UT_SETUP NULL
#define UT_TEARDOWN NULL

#define MAXUTESTNAMELEN 100
#define MAXUTESTFILELEN 100
#define MAXUTESTPATHLEN 200
#define MAXUTESTMESGLEN 200
#define MAX_UTEST_BASE_LEN 200

typedef struct utest_info_tag
{
    int line;
    char file[MAXUTESTFILELEN];
    char base[MAX_UTEST_BASE_LEN];
    char msg[MAXUTESTMESGLEN];
    char suiteName[MAXUTESTNAMELEN];
    char testName[MAXUTESTNAMELEN];
    int status;
    void (*logerror)(struct utest_info_tag*,int,const char*,const char*);
    struct utest_info_tag* next;
} utest_info;

typedef struct utest_test_tag
{
    char name[MAXUTESTNAMELEN];
    gboolean (*theTest)(utest_info*);
} utest_test;

typedef struct utest_suite_tag
{
    char name[MAXUTESTNAMELEN];
    gboolean (*initializer)(utest_info*);
    gboolean (*finalizer)(utest_info*);
    gboolean (*setup)(utest_info*);
    gboolean (*teardown)(utest_info*);
    utest_test tests[];
} utest_suite;


#define UT_DEF(name)                            \
    static gboolean                                 \
name(utest_info *UT_INFO)


#define UT_REGISTER_BEGIN(name)                 \
    static utest_suite UT_SUITE = {             \
        name,                                   \
        UT_INITIALIZER,                         \
        UT_FINALIZER,                           \
        UT_SETUP,                               \
        UT_TEARDOWN,                            \
        {

#define UT_REGISTER(func_name, desc)            \
            {desc, &func_name},

#define UT_REGISTER_END                         \
            {"null", NULL}                      \
        }                                       \
    };                                          \
                                                \
utest_suite*                                    \
register_suite(void)                            \
{                                               \
    return &UT_SUITE;                           \
}

#define CUTTER_BANNER "Cutter is based on CUnitTester by Stewart Gebbie <cunit@gethos.net> 2004"

#endif /* CUTTER_CUTTER_H */
/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
