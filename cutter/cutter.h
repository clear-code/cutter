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

G_BEGIN_DECLS

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

#define CUT_INFO _ut_info
#define CUT_SUITE _ut_suite
#define CUT_NAME _ut_name

#include <assersions.h>

#define CUT_INITIALIZER NULL
#define CUT_FINALIZER NULL
#define CUT_SETUP NULL
#define CUT_TEARDOWN NULL

#define MAXCUTESTNAMELEN 100
#define MAXCUTESTFILELEN 100
#define MAXCUTESTPATHLEN 200
#define MAXCUTESTMESGLEN 200
#define MAX_CUTEST_BASE_LEN 200

typedef struct utest_info_tag
{
    int line;
    char file[MAXCUTESTFILELEN];
    char base[MAX_CUTEST_BASE_LEN];
    char msg[MAXCUTESTMESGLEN];
    char suiteName[MAXCUTESTNAMELEN];
    char testName[MAXCUTESTNAMELEN];
    int status;
    void (*logerror)(struct utest_info_tag*,int,const char*,const char*);
    struct utest_info_tag* next;
} utest_info;

typedef struct utest_test_tag
{
    char name[MAXCUTESTNAMELEN];
    gboolean (*theTest)(utest_info*);
} utest_test;

typedef struct utest_suite_tag
{
    char name[MAXCUTESTNAMELEN];
    gboolean (*initializer)(utest_info*);
    gboolean (*finalizer)(utest_info*);
    gboolean (*setup)(utest_info*);
    gboolean (*teardown)(utest_info*);
    utest_test tests[];
} utest_suite;


#define CUT_DEFINE_TEST(name)                   \
    static gboolean                             \
name(utest_info *CUT_INFO)


#define CUT_REGISTER_BEGIN(name)                 \
    static utest_suite CUT_SUITE = {             \
        name,                                   \
        CUT_INITIALIZER,                         \
        CUT_FINALIZER,                           \
        CUT_SETUP,                               \
        CUT_TEARDOWN,                            \
        {

#define CUT_REGISTER(func_name, desc)            \
            {desc, &func_name},

#define CUT_REGISTER_END                         \
            {"null", NULL}                      \
        }                                       \
    };                                          \
                                                \
utest_suite*                                    \
register_suite(void)                            \
{                                               \
    return &CUT_SUITE;                           \
}

#define CUTTER_BANNER "Cutter is based on CUnitTester by Stewart Gebbie <cunit@gethos.net> 2004"

G_END_DECLS

#endif /* CUTTER_CUTTER_H */
/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
