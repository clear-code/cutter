#include <cutter.h>
#include <stack.h>

void test_new_stack (void);
void test_push (void);

static Stack *stack;

void
setup (void)
{
    stack = NULL;
}

void
teardown (void)
{
    if (stack)
        stack_free(stack);
}

void
test_new_stack (void)
{
    stack = stack_new();
    cut_assert(stack_is_empty(stack));
}

void
test_push (void)
{
    int value = 100;

    stack = stack_new();
    cut_assert_equal_int(0, stack_get_size(stack));
    stack_push(stack, value);
    cut_assert_equal_int(1, stack_get_size(stack));
    cut_assert(!stack_is_empty(stack));
}
