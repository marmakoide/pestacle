#ifndef PESTACLE_STACK_H
#define PESTACLE_STACK_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
  Implementation of a stack, that will extend itself as needed
 *****************************************************************************/


#include <stddef.h>
#include <stdbool.h>


typedef struct {
	size_t logical_len;
	size_t physical_len;
	void** data;
} Stack;


extern void
Stack_init(
	Stack* self
);


extern void
Stack_destroy(
	Stack* self
);


extern void
Stack_copy(
	Stack* self,
	Stack* other
);


extern void
Stack_clear(
	Stack* self
);


extern void
Stack_push(
	Stack* self,
	void* item
);


extern void*
Stack_pop(
	Stack* self
);


extern void*
Stack_peek(
	Stack* self
);


extern bool
Stack_empty(
	const Stack* self
);


extern size_t
Stack_length(
	const Stack* self
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_STACK_H */