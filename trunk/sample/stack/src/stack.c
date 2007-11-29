#include <stdlib.h>
#include "stack.h"

#define TRUE 1
#define FALSE 0

struct _Stack {
    int size;
    int *data;
};

Stack *
stack_new (void)
{
    Stack *stack;

    stack = malloc(sizeof(Stack));
    if (!stack)
        return NULL;

    stack->size = 0;
    stack->data = NULL;
    return stack;
}

void
stack_free (Stack *stack)
{
    free(stack->data);
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

static int
stack_realloc (Stack *stack)
{
    int *new_data;

    new_data = realloc(stack->data, sizeof(*stack->data) * stack->size);
    if (stack->size > 0 && !new_data) {
        free(stack->data);
        stack->data = NULL;
        stack->size = 0;
        return FALSE;
    }
    stack->data = new_data;

    return TRUE;
}

void
stack_push (Stack *stack, int value)
{
    stack->size++;
    if (!stack_realloc(stack))
        return;
    stack->data[stack->size - 1] = value;
}

int
stack_pop (Stack *stack)
{
    int value;

    stack->size--;
    value = stack->data[stack->size];
    stack_realloc(stack);
    return value;
}
