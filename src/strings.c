#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "strings.h"
#include "memory.h"


void
String_clone(
	String* self,
	const String* other
) {
	assert(self != 0);
	assert(other != 0);
	assert(other->data != 0);

	self->data = (char*)checked_malloc(other->len * sizeof(char));
	self->len = other->len;

	memcpy(self->data, other->data, other->len);
}


void
String_destroy(
	String* self
) {
	assert(self != 0);
	assert(self->data != 0);

	free(self->data);

	#ifdef DEBUG
	self->data = 0;
	self->len = 0;
	#endif
}


uint32_t
String_djb_hash(const String *restrict str) {
	// Classic DJB hash, by Daniel J. Berstein
	assert(str != 0);
	
	uint32_t hash = 5381;

	const char* char_ptr = str->data;
	for(size_t i = str->len - 1; i != 0; --i, ++char_ptr)
		hash = ((hash << 5) + hash) + *char_ptr; // equivalent to hash * 33 + c

	return hash;
}


bool
String_equals(
	const String *restrict self,
	const String *restrict other
) {
	assert(self != 0);
	assert(other != 0);

	if (self->len != other->len)
		return 0;

	return
		strncmp(self->data, other->data, self->len) == 0;
}
