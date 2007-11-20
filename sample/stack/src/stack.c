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

void
stack_push (Stack *stack, int value)
{
    int *new_data;

    stack->size++;
    new_data = realloc(stack->data, sizeof(*stack->data) * stack->size);
    if (!new_data) {
        free(stack->data);
        stack->data = NULL;
        stack->size = 0;
        return;
    }
    stack->data = new_data;

    stack->data[stack->size - 1] = value;
}

int
stack_pop (Stack *stack)
{
    int value;
    int *new_data;

    stack->size--;
    value = stack->data[stack->size];

    new_data = realloc(stack->data, sizeof(*stack->data) * stack->size);
    if (stack->size > 0 && !new_data) {
        free(stack->data);
        stack->data = NULL;
        stack->size = 0;
        return;
    }
    stack->data = new_data;

    return value;
}
