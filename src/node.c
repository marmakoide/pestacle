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

	// Setup inputs array
	if (delegate->input_count == 0)
		self->inputs = 0;
	else {
		self->inputs =
			(Node**)malloc(delegate->input_count * sizeof(Node*));

		Node** input_ptr = self->inputs;
		for(size_t i = delegate->input_count; i != 0; --i, ++input_ptr)
			*input_ptr = 0;
	}

	// Setup parameters array
	if (delegate->parameter_count == 0)
		self->parameters = 0;
	else {
		self->parameters =
			(NodeParameter*)malloc(delegate->parameter_count * sizeof(NodeParameter));

		NodeParameter* param_ptr = self->parameters;
		const NodeParameterDefinition* param_def_ptr = delegate->parameter_defs;
		for(size_t i = delegate->parameter_count; i != 0; --i, ++param_ptr, ++param_def_ptr)
			param_ptr->value = param_def_ptr->default_value;
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

	// Deallocate input array
	if (self->delegate->input_count > 0) {
		#ifdef DEBUG
		Node** input_ptr = self->inputs;
		for(size_t i = delegate->input_count; i != 0; --i, ++input_ptr)
			*input_ptr = 0;
		#endif
	
		free(self->inputs);
	}

	// Deallocate parameter array
	if (self->delegate->parameter_count > 0)
		free(self->parameters);

	#ifdef DEBUG
	self->data = 0;
	self->delegate = 0;
	self->inputs = 0;
	#endif
}


NodeParameter*
node_get_parameter_by_name(
	Node* self,
	const String* name
) {
	assert(self != 0);
	assert(name != 0);
	assert(name->data != 0);

	const NodeParameterDefinition* param_def_ptr =
		self->delegate->parameter_defs;

	for(size_t i = 0; i < self->delegate->parameter_count; ++i, ++param_def_ptr)
		if (string_equals(name, &(param_def_ptr->name)))
			return self->parameters + i;

	return 0;
}



int
node_set_input_slot_by_name(
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