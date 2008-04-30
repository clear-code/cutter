#include "cutter.h"
#include <cutter/cut-test-context.h>

void test_inspect_string_array (void);

static CutTestContext *context;

void
setup (void)
{
    context = cut_test_context_new(NULL, NULL, NULL);
}

void
teardown (void)
{
    g_object_unref(context);
}

void
test_inspect_string_array (void)
{
    const gchar *strings[] = {"a", "b", "c", NULL};

    cut_assert_equal_string("[\"a\", \"b\", \"c\"]",
                            cut_test_context_inspect_string_array(context, strings));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
