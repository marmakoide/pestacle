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

	if (self->delegate->methods.setup)
		return self->delegate->methods.setup(self, width, height);

	return 1;
}


void
source_destroy(
	Source* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	if (self->delegate->methods.destroy)
		self->delegate->methods.destroy(self);

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


int
source_set_input_slot(
	Source* self,
	const String* name,
	Source* other
) {
	assert(self != 0);
	assert(other != 0);
	assert(name != 0);
	assert(name->data != 0);

	const SourceInputSlotDefinition* slot_ptr = self->delegate->input_defs;
	for(size_t i = 0; i < self->delegate->input_count; ++i, ++slot_ptr)
		if (string_equals(name, &(slot_ptr->name))) {
			self->inputs[i] = other;
			return 1;
		}
	
	return 0;
}


void
source_update(
	Source* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	if (self->delegate->methods.update)
		self->delegate->methods.update(self);
}


void
source_handle_event(
	Source* self,
	const Event* event
) {
	assert(self != 0);
	assert(self->delegate != 0);
	assert(event != 0);

	if (self->delegate->methods.handle_event)
		self->delegate->methods.handle_event(self, event);
}


const Matrix*
source_get(
	Source* self
) {
	assert(self != 0);
	assert(self->delegate != 0);
	assert(self->delegate->methods.get != 0);

	return self->delegate->methods.get(self);
}