#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "parser/string_list.h"

#define INITIAL_PHYSICAL_LEN 8


#ifdef DEBUG
static void
StringList_fill_with_null(
	StringList* self
) {
	String* ptr = self->items;
	for(size_t i = self->physical_len; i != 0; --i, ++ptr) {
		ptr->data = 0;
		ptr->len = 0;
	}
}
#endif


static void
StringList_destroy_items(
	StringList* self
) {
	String* ptr = self->items;
	for(size_t i = self->logical_len; i != 0; --i, ++ptr)
		String_destroy(ptr);
}


void
StringList_init(
	StringList* self
) {

	self->logical_len = 0;
	self->physical_len = INITIAL_PHYSICAL_LEN;
	self->items = (String*)checked_malloc(self->physical_len * sizeof(String));

	#ifdef DEBUG
	StringList_fill_with_null(self);
	#endif
}


void
StringList_destroy(
	StringList* self
) {
	StringList_destroy_items(self);
	free(self->items);

	#ifdef DEBUG
	self->logical_len = 0;
	self->physical_len = 0;
	self->items = 0;
	#endif
}


void
StringList_clear(
	StringList* self
) {
	assert(self != 0);
	assert(self->data != 0);

	StringList_destroy_items(self);
	self->logical_len = 0;
}


bool
StringList_empty(
	const StringList* self
) {
	assert(self != 0);
	assert(self->data != 0);

	return self->logical_len == 0;
}


size_t
StringList_length(
	const StringList* self
) {
	assert(self != 0);
	assert(self->data != 0);

	return self->logical_len;
}


const String*
StringList_at(
	const StringList* self,
	size_t i
) {
	assert(self != 0);
	assert(i < self->logical_len);

	return self->items + i;
}


void
StringList_append(
	StringList* self,
	const String* str
) {
	assert(self != 0);
	assert(str != 0);

	// Extend the stack storage if required
	if (self->physical_len == self->logical_len) {
		String* new_items= (String*)checked_malloc(2 * self->physical_len * sizeof(String));
		memcpy(new_items, self->items, self->physical_len * sizeof(String));
		self->physical_len *= 2;
		free(self->items);
		self->items = new_items;
	}

	// Append the new item
	String_clone(self->items + self->logical_len, str);
	self->logical_len += 1;
}
