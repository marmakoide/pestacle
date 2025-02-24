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
StringListView_init(
	StringListView* self,
	const StringList* list
) {
	assert(self);
	assert(list);

	self->items = list->items;
	self->logical_len = list->logical_len;
}


void
StringListView_head(
	StringListView* self,
	size_t len
) {
	assert(self);
	assert(len <= self->logical_len);

	self->logical_len = len;
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


size_t
StringListView_length(
	const StringListView* self
) {
	assert(self);
	assert(self->items);

	return self->logical_len;
}


static size_t
char_array_content_length(
	const char** char_array,
	size_t len
)
{
	size_t ret = 0;
	for(size_t i = len; i != 0; --i, ++char_array)
		ret += strlen(*char_array);
	return ret;
}


extern size_t
StringList_content_length(
	const StringList* self
) {
	assert(self);

	return
		char_array_content_length(
			(const char**)self->items,
			self->logical_len
		);
}


extern size_t
StringListView_content_length(
	const StringListView* self
) {
	assert(self);

	return char_array_content_length(
		(const char**)self->items,
		self->logical_len
	);
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


const char*
StringListView_at(
	const StringListView* self,
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


static void
char_array_print(
	const char** char_array,
	size_t len,
	FILE* out
)
{
	for(size_t i = len; i != 0; --i, ++char_array) {
		fputs(*char_array, out);
		if (i > 1)
			fputc('.', out);
	}
}

 
void
StringList_print(
	const StringList* self,
	FILE* out
) {
	assert(self);
	assert(out);

	char_array_print((const char**)self->items, self->logical_len, out);
}


void
StringListView_print(
	const StringListView* self,
	FILE* out
) {
	assert(self);
	assert(out);

	char_array_print((const char**)self->items, self->logical_len, out);
}


static char*
char_array_join(
	const char** char_array,
	size_t len,
	char c
) {
	// Compute output size
	size_t out_len = 1;
	
	if (len > 0)
		out_len += len - 1;

	const char** str = char_array;
	for(size_t i = len; i != 0; --i, ++str)
		out_len += strlen(*str);

	// Build output
	char* out = checked_malloc(out_len);
	char* out_ptr = out;

	str = char_array;
	for(size_t i = len; i != 0; --i, ++str) {
		size_t str_len = strlen(*str);
		memcpy(out_ptr, *str, str_len);
		out_ptr += str_len;

		if (i > 1) {
			*out_ptr = c;
			out_ptr += 1;
		}
	}
	*out_ptr = '\0';

	// Job done
	return out;
}


char*
StringList_join(
	const StringList* self,
	char c
) {
	assert(self);

	return
		char_array_join((const char**)self->items, self->logical_len, c);
}


char*
StringListView_join(
	const StringListView* self,
	char c
) {
	assert(self);

	return
		char_array_join((const char**)self->items, self->logical_len, c);	
}