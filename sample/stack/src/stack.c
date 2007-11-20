#include <stdlib.h>
#include "stack.h"

#define TRUE 1
#define FALSE 0

struct _Stack {
    int size;
};

Stack *
stack_new (void)
{
    Stack *stack;

    stack = malloc(sizeof(Stack));
    if (!stack)
        return NULL;

    stack->size = 0;
    return stack;
}

void
stack_free (Stack *stack)
{
    free(stack);
}

int
stack_is_empty (Stack *stack)
{
    return stack->size == 0;
}

int
stack_get_size (Stack *stack)
{
    return stack->size;
}

void
stack_push (Stack *stack, int value)
{
    stack->size++;
}
