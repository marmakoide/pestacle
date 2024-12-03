#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <pestacle/memory.h>
#include <pestacle/strings.h>
#include <pestacle/string_list.h>

#define INITIAL_PHYSICAL_LEN 8


#ifdef DEBUG
static void
StringList_fill_with_null(
	StringList* self
) {
	char** ptr = self->items;
	for(size_t i = self->physical_len; i != 0; --i, ++ptr)
		*ptr = 0;
}
#endif


static void
StringList_destroy_items(
	StringList* self
) {
	char** ptr = self->items;
	for(size_t i = self->logical_len; i != 0; --i, ++ptr)
		free(*ptr);
}


void
StringList_init(
	StringList* self
) {

	self->logical_len = 0;
	self->physical_len = INITIAL_PHYSICAL_LEN;
	self->items = (char**)checked_malloc(self->physical_len * sizeof(char*));

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
	assert(self);
	assert(self->items);

	StringList_destroy_items(self);
	self->logical_len = 0;
}


void
StringList_copy(
	StringList* self,
	const StringList* src
) {
	assert(self);
	assert(src);

	// Destroy all current items
	StringList_destroy_items(self);

	// Reallocate item array if necessary
	if (self->physical_len < src->logical_len) {
		free(self->items);
		self->physical_len = src->logical_len;
		self->items = (char**)checked_malloc(self->physical_len * sizeof(char*));
	}

	// Copy each string
	char** dst_str = self->items;
	char* const* src_str = src->items;
	for(size_t i = src->logical_len; i != 0; --i, ++src_str, ++dst_str)
		*dst_str = strclone(*src_str);

	// Update logical len
	self->logical_len = src->logical_len;

}


bool
StringList_empty(
	const StringList* self
) {
	assert(self);
	assert(self->items);

	return self->logical_len == 0;
}


size_t
StringList_length(
	const StringList* self
) {
	assert(self);
	assert(self->items);

	return self->logical_len;
}


extern const char**
StringList_items(
	const StringList* self
) {
	assert(self);
	assert(self->items);

	return (const char**)self->items;
}


const char*
StringList_at(
	const StringList* self,
	size_t i
) {
	assert(self);
	assert(i < self->logical_len);

	return self->items[i];
}


void
StringList_append(
	StringList* self,
	const char* str
) {
	assert(self);
	assert(str);

	// Extend the stack storage if required
	if (self->physical_len == self->logical_len) {
		char** new_items= (char**)checked_malloc(2 * self->physical_len * sizeof(char*));
		memcpy(new_items, self->items, self->physical_len * sizeof(char*));
		self->physical_len *= 2;
		free(self->items);
		self->items = new_items;
	}

	// Append the new item
	self->items[self->logical_len] = strclone(str);

	
	self->logical_len += 1;
}


void
StringList_print(
	const StringList* self,
	FILE* out
) {
	assert(self);
	assert(out);

	const char** str = (const char**)self->items;
	for(size_t i = self->logical_len; i != 0; --i, ++str) {
		fputs(*str, out);
		if (i > 1)
			fputc('.', out);
	}
}
