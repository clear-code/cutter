#include <cutter.h>
#include <stack.h>

void test_new_stack (void);
void test_push (void);
void test_pop (void);

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
    stack = stack_new();
    cut_assert_equal_int(0, stack_get_size(stack));
    stack_push(stack, 100);
    cut_assert_equal_int(1, stack_get_size(stack));
    cut_assert(!stack_is_empty(stack));
}

void
test_pop (void)
{
    stack = stack_new();

    stack_push(stack, 10);
    stack_push(stack, 20);
    stack_push(stack, 30);

    cut_assert_equal_int(3, stack_get_size(stack));
    cut_assert_equal_int(30, stack_pop(stack));
    cut_assert_equal_int(2, stack_get_size(stack));
    cut_assert_equal_int(20, stack_pop(stack));
    cut_assert_equal_int(1, stack_get_size(stack));

    stack_push(stack, 40);
    cut_assert_equal_int(2, stack_get_size(stack));
    cut_assert_equal_int(40, stack_pop(stack));
    cut_assert_equal_int(1, stack_get_size(stack));
    cut_assert_equal_int(10, stack_pop(stack));
    cut_assert_equal_int(0, stack_get_size(stack));
    cut_assert(stack_is_empty(stack));
}
