#include <assert.h>
#include "source.h"


Source*
source_allocate() {
	return (Source*)malloc(sizeof(Source));
}


void
source_init(
	Source* self,
	const SourceDelegate* delegate,
	void* data
) {
	assert(self != 0);

	self->data = data;
	self->delegate = delegate;

	if (delegate->input_count == 0)
		self->inputs = 0;
	else {
		self->inputs =
			(Source**)malloc(delegate->input_count * sizeof(Source*));

		Source** input_ptr = self->inputs;
		for(size_t i = delegate->input_count; i != 0; --i, ++input_ptr)
			*input_ptr = 0;
	}
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

	if (self->delegate->input_count > 0) {
		#ifdef DEBUG
		Source** input_ptr = self->inputs;
		for(size_t i = delegate->input_count; i != 0; --i, ++input_ptr)
			*input_ptr = 0;
		#endif
	
		free(self->inputs);
	}

	#ifdef DEBUG
	self->data = 0;
	self->delegate = 0;
	self->inputs = 0;
	#endif
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


const Matrix*
source_get(
	Source* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	return self->delegate->get(self);
}