#include "../include/stack.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


#define STACK_INITIAL_CAP 64

#define deref(s) (*(struct stack_internal *) s)


struct stack_internal {
    size_t cap;
    size_t len;
    size_t item_size;
    char *data;
};


void stack_init(struct stack *s, size_t item_size)
{
	deref(s).cap = STACK_INITIAL_CAP;
	deref(s).len = 0;
	deref(s).item_size = item_size;
	deref(s).data = malloc(item_size * STACK_INITIAL_CAP);
}

void stack_destroy(struct stack *s)
{
	free(deref(s).data);
}

void *stack_pop(struct stack *s)
{
	assert(deref(s).len && "stack underflow");

	void *item = stack_at(s, deref(s).len - 1);

	deref(s).len--;

	return item;
}

void stack_push(struct stack *s, void *item)
{
	if (deref(s).len == deref(s).cap) {
		deref(s).cap *= 2;

		deref(s).data = realloc(deref(s).data,
					deref(s).cap * deref(s).item_size);
	}

	memcpy(&deref(s).data[deref(s).len * deref(s).item_size],
	       item, deref(s).item_size);

	deref(s).len++;
}

void *stack_top(struct stack *s)
{
	return stack_at(s, deref(s).len - 1);
}

void *stack_at(struct stack *s, size_t index)
{
	return &deref(s).data[index * deref(s).item_size];
}

size_t stack_len(const struct stack *s)
{
	return deref(s).len;
}
