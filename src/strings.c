#include <assert.h>
#include <string.h>
#include "strings.h"


int
string_equals(
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