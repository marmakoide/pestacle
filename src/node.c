#include <assert.h>
#include "node.h"
#include "memory.h"


// --- NodeParameter ----------------------------------------------------------

static void
NodeParameterValue_copy(
	NodeParameterValue* dst,
	const NodeParameterValue* src,
	enum NodeParameterType type
) {
	assert(dst != 0);
	assert(src != 0);

	switch(type) {
		case NodeParameterType__invalid:
			assert(0);
			break;
		case NodeParameterType__integer:
			dst->int64_value = src->int64_value;
			break;
		case NodeParameterType__real:
			dst->real_value = src->real_value;
			break;
		case NodeParameterType__string:
			String_clone(&(dst->string_value), &(src->string_value));
			break;
	}
}


// --- Node -------------------------------------------------------------------

static Node*
Node_new(
	const String* name,
	const NodeDelegate* delegate
) {
	assert(delegate != 0);

	// Allocate
	Node* ret = (Node*)checked_malloc(sizeof(Node));

	// Setup
	ret->data = 0;
	ret->delegate = delegate;
	String_clone(&(ret->name), name);

	// Setup inputs array
	if (delegate->input_count == 0)
		ret->inputs = 0;
	else {
		ret->inputs =
			(Node**)checked_malloc(delegate->input_count * sizeof(Node*));

		Node** input_ptr = ret->inputs;
		for(size_t i = delegate->input_count; i != 0; --i, ++input_ptr)
			*input_ptr = 0;
	}

	// Setup parameters array
	if (delegate->parameter_count == 0)
		ret->parameters = 0;
	else {
		ret->parameters =
			(NodeParameterValue*)checked_malloc(delegate->parameter_count * sizeof(NodeParameterValue));

		NodeParameterValue* param_ptr = ret->parameters;
		const NodeParameterDefinition* param_def_ptr = delegate->parameter_defs;
		for(size_t i = delegate->parameter_count; i != 0; --i, ++param_ptr, ++param_def_ptr)
			NodeParameterValue_copy(
				param_ptr,
				&(param_def_ptr->default_value),
				param_def_ptr->type
			);
	}

	// Job done
	return ret;
}


bool
Node_setup(
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

	return true;
}


void
Node_destroy(
	Node* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	if (self->delegate->methods.destroy)
		self->delegate->methods.destroy(self);

	String_destroy(&(self->name));

	// Deallocate input array
	if (self->delegate->input_count > 0) {
		#ifdef DEBUG
		Node** input_ptr = self->inputs;
		for(size_t i = self->delegate->input_count; i != 0; --i, ++input_ptr)
			*input_ptr = 0;
		#endif
	
		free(self->inputs);
	}

	// Deallocate parameters
	for(size_t i = 0; i < self->delegate->parameter_count; ++i) {
		const NodeParameterDefinition* param_def = self->delegate->parameter_defs;
		if (param_def->type == NodeParameterType__string)
			String_destroy(&(self->parameters[i].string_value));

		#ifdef DEBUG
		param_def = NodeParameterType__invalid;
		#endif
	}

	// Deallocate parameter array
	if (self->delegate->parameter_count > 0)
		free(self->parameters);

	#ifdef DEBUG
	self->data = 0;
	self->delegate = 0;
	self->name.data = 0;
	self->inputs = 0;
	#endif
}


Node*
Node_create_by_name(
	const String* name,
	const String* delegate_name	
) {
	assert(name != 0);
	assert(name->data != 0);
	assert(delegate_name != 0);
	assert(delegate_name->data != 0);

	const NodeDelegate* delegate = get_node_delegate_by_name(delegate_name);
	if (!delegate)
		return 0;

	return Node_new(name, delegate);
}


bool
Node_get_parameter_by_name(
	Node* self,
	const String* name,
	const NodeParameterDefinition** node_def_ptr,
	NodeParameterValue** node_value_ptr
) {
	assert(self != 0);
	assert(name != 0);
	assert(name->data != 0);

	const NodeParameterDefinition* param_def_ptr =
		self->delegate->parameter_defs;

	for(size_t i = 0; i < self->delegate->parameter_count; ++i, ++param_def_ptr)
		if (String_equals(name, &(param_def_ptr->name))) {
			if (node_def_ptr)
				*node_def_ptr = param_def_ptr;

			if (node_value_ptr)
				*node_value_ptr = self->parameters + i;
			
			return true;
		}
		
	return false;
}



bool
Node_set_input_slot_by_name(
	Node* self,
	const String* name,
	Node* other
) {
	assert(self != 0);
	assert(other != 0);
	assert(name != 0);
	assert(name->data != 0);

	const NodeInputDefinition* input_ptr = self->delegate->input_defs;
	for(size_t i = 0; i < self->delegate->input_count; ++i, ++input_ptr)
		if (String_equals(name, &(input_ptr->name))) {
			self->inputs[i] = other;
			return true;
		}

	return false;
}


bool
Node_is_complete(
	const Node* self
) {
	assert(self != 0);

	Node* const* node_ptr = self->inputs;
	for(size_t i = 0; i < self->delegate->input_count; ++i, ++node_ptr)
		if (*node_ptr == 0)
			return false;

	return true;
}


void
Node_update(
	Node* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	if (self->delegate->methods.update)
		self->delegate->methods.update(self);
}


void
Node_handle_event(
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
Node_get(
	Node* self
) {
	assert(self != 0);
	assert(self->delegate != 0);
	assert(self->delegate->methods.get != 0);

	return self->delegate->methods.get(self);
}