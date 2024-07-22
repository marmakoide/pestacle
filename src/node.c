#include <assert.h>
#include "node.h"


Node*
node_allocate() {
	return (Node*)malloc(sizeof(Node));
}


void
node_init(
	Node* self,
	const NodeDelegate* delegate,
	void* data
) {
	assert(self != 0);

	self->data = data;
	self->delegate = delegate;

	if (delegate->input_count == 0)
		self->inputs = 0;
	else {
		self->inputs =
			(Node**)malloc(delegate->input_count * sizeof(Node*));

		Node** input_ptr = self->inputs;
		for(size_t i = delegate->input_count; i != 0; --i, ++input_ptr)
			*input_ptr = 0;
	}
}


int
node_setup(
	Node* self,
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
node_destroy(
	Node* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	if (self->delegate->methods.destroy)
		self->delegate->methods.destroy(self);

	if (self->delegate->input_count > 0) {
		#ifdef DEBUG
		Node** input_ptr = self->inputs;
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
node_set_input_slot(
	Node* self,
	const String* name,
	Node* other
) {
	assert(self != 0);
	assert(other != 0);
	assert(name != 0);
	assert(name->data != 0);

	const NodeInputSlotDefinition* slot_ptr = self->delegate->input_defs;
	for(size_t i = 0; i < self->delegate->input_count; ++i, ++slot_ptr)
		if (string_equals(name, &(slot_ptr->name))) {
			self->inputs[i] = other;
			return 1;
		}
	
	return 0;
}


void
node_update(
	Node* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	if (self->delegate->methods.update)
		self->delegate->methods.update(self);
}


void
node_handle_event(
	Node* self,
	const Event* event
) {
	assert(self != 0);
	assert(self->delegate != 0);
	assert(event != 0);

	if (self->delegate->methods.handle_event)
		self->delegate->methods.handle_event(self, event);
}


const Matrix*
node_get(
	Node* self
) {
	assert(self != 0);
	assert(self->delegate != 0);
	assert(self->delegate->methods.get != 0);

	return self->delegate->methods.get(self);
}