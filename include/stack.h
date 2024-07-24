#ifndef PESTACLE_STACK_H
#define PESTACLE_STACK_H

#include <stddef.h>
#include <stdbool.h>


typedef struct {
	size_t logical_len;
	size_t physical_len;
	void** data;
} Stack;


extern void
stack_init(
	Stack* self
);


extern void
stack_destroy(
	Stack* self
);


extern void
stack_clear(
	Stack* self
);


extern void
stack_push(
	Stack* self,
	void* item
);


extern void*
stack_pop(
	Stack* self
);


extern void*
stack_peek(
	Stack* self
);


extern bool
stack_empty(
	const Stack* self
);


#endif /* PESTACLE_STACK_H */