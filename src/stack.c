#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "stack.h"
#include "memory.h"

#define INITIAL_PHYSICAL_LEN 8


#ifdef DEBUG
static void
Stack_fill_stack_with_null(
	Stack* self
) {
	void** ptr = self->data;
	for(size_t i = self->physical_len; i != 0; --i, ++ptr)
		*ptr = 0;
}
#endif


void
Stack_init(
	Stack* self
) {
	assert(self != 0);

	self->logical_len = 0;
	self->physical_len = INITIAL_PHYSICAL_LEN;
	self->data = (void**)checked_malloc(self->physical_len * sizeof(void*));

	#ifdef DEBUG
	Stack_fill_stack_with_null(self);
	#endif
}

void
Stack_destroy(
	Stack* self
) {
	assert(self != 0);
	assert(self->data != 0);

	#ifdef DEBUG
	Stack_fill_stack_with_null(self);
	#endif

	free(self->data);

	#ifdef DEBUG
	self->logical_len = 0;
	self->physical_len = 0;
	self->data = 0;
	#endif
}


void
Stack_copy(
	Stack* self,
	Stack* other
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(other != 0);
	assert(other->data != 0);

	// Extend the stack storage if required
	if (self->physical_len < other->logical_len) {
		size_t new_physical_len = 2 * self->physical_len;
		while(new_physical_len < other->logical_len)
			new_physical_len *= 2;

		void** new_data = (void**)checked_malloc(new_physical_len * sizeof(void*));
		self->physical_len = new_physical_len;
		free(self->data);
		self->data = new_data;
	}

	// Copy the data
	memcpy(self->data, other->data, other->logical_len * sizeof(void*));
	self->logical_len = other->logical_len;
}


void
Stack_clear(
	Stack* self
) {
	assert(self != 0);
	assert(self->data != 0);

	self->logical_len = 0;

	#ifdef DEBUG
	Stack_fill_stack_with_null(self);
	#endif
}


void
Stack_push(
	Stack* self,
	void* item
) {
	assert(self != 0);
	assert(self->data != 0);

	// Extend the stack storage if required
	if (self->physical_len == self->logical_len) {
		void** new_data = (void**)checked_malloc(2 * self->physical_len * sizeof(void*));
		memcpy(new_data, self->data, self->physical_len * sizeof(void*));
		self->physical_len *= 2;
		free(self->data);
		self->data = new_data;
	}

	// Append the new item
	self->data[self->logical_len] = item;
	self->logical_len += 1;
}


void*
Stack_pop(
	Stack* self
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(self->logical_len != 0);

	self->logical_len -= 1;
	void* ret = self->data[self->logical_len];
	
	#ifdef DEBUG
	self->data[self->logical_len] = 0;
	#endif

	return ret;
}


void*
Stack_top(
	Stack* self
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(self->logical_len != 0);

	return self->data[self->logical_len - 1];
}


bool
Stack_empty(
	const Stack* self
) {
	assert(self != 0);
	assert(self->data != 0);

	return self->logical_len == 0;
}


size_t
Stack_length(
	const Stack* self
) {
	assert(self != 0);
	assert(self->data != 0);

	return self->logical_len;
}