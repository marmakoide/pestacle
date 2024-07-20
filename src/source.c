#include <assert.h>
#include "source.h"


Source*
source_allocate() {
	return (Source*)malloc(sizeof(Source));
}


int
source_setup(
	Source* self,
	int width,
	int height
) {
	assert(self != 0);
	assert(self->delegate != 0);
	assert(width > 0);
	assert(height > 0);

	return self->delegate->setup(self, width, height);
}


void
source_destroy(
	Source* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	self->delegate->destroy(self);
}


void
source_update(
	Source* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	self->delegate->update(self);
}


void
source_handle_event(
	Source* self,
	const Event* event
) {
	assert(self != 0);
	assert(self->delegate != 0);
	assert(event != 0);

	self->delegate->handle_event(self, event);
}


const struct Matrix*
source_get(
	Source* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	return self->delegate->get(self);
}