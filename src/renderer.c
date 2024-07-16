#include <assert.h>
#include "renderer.h"


struct Renderer*
renderer_allocate() {
	return (struct Renderer*)malloc(sizeof(struct Renderer));
}



int
renderer_setup(
	struct Renderer* self,
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
renderer_destroy(
	struct Renderer* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	self->delegate->destroy(self);
}


void
renderer_render(
	struct Renderer* self,
	const struct Matrix* src,
	SDL_Surface* dst
) {
	assert(self != 0);
	assert(self->delegate != 0);
	assert(src != 0);
	assert(dst != 0);

	self->delegate->render(self, src, dst);
}
