#include "cutter.h"
#include <cutter/cut-test-context.h>

void test_user_data (void);

static CutTestContext *context;

static guint32 user_data;

void
setup (void)
{
    context = cut_test_context_new(NULL, NULL, NULL);

    user_data = 0;
}

void
teardown (void)
{
    g_object_unref(context);
}

static void
destroy_user_data (gpointer data)
{
    cut_assert_equal_int(user_data,
                         GPOINTER_TO_UINT(data));
}

void
test_user_data (void)
{
    user_data = g_random_int();

    cut_test_context_set_user_data(context,
                                   GUINT_TO_POINTER(user_data), 
                                   destroy_user_data);
    cut_assert_equal_int(user_data,
                         cut_test_context_get_user_data(context));

    cut_test_context_set_user_data(context, 0, NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
