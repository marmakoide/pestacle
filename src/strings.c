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


int
String_equals(
	const String* self,
	const String* other
) {
	assert(self != 0);
	assert(other != 0);

	if (self->len != other->len)
		return 0;

	return
		strncmp(self->data, other->data, self->len) == 0;
}