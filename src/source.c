#include <assert.h>
#include "source.h"


struct Source*
source_allocate() {
	return (struct Source*)malloc(sizeof(struct Source));
}


int
source_setup(
	struct Source* self,
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
	struct Source* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	self->delegate->destroy(self);
}


void
source_update(
	struct Source* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	self->delegate->update(self);
}


void
source_handle_event(
	struct Source* self,
	const SDL_Event* event
) {
	assert(self != 0);
	assert(self->delegate != 0);
	assert(event != 0);

	self->delegate->handle_event(self, event);
}


const struct Matrix*
source_get(
	struct Source* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	return self->delegate->get(self);
}