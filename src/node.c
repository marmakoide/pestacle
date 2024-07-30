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
		case NodeParameterType__last:
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


// --- NodeDelegate -----------------------------------------------------------

static bool
NodeDelegate_has_inputs(const NodeDelegate* self) {
	assert(self != 0);

	return self->input_defs->type == NodeType__last;
}


static size_t
NodeDelegate_input_count(const NodeDelegate* self) {
	assert(self != 0);

	size_t count = 0;
	const NodeInputDefinition* input_def = self->input_defs;
	for( ; input_def->type != NodeType__last; ++input_def, ++count);

	return count;
}


static bool
NodeDelegate_has_parameters(const NodeDelegate* self) {
	assert(self != 0);

	return self->parameter_defs->type == NodeParameterType__last;
}


static size_t
NodeDelegate_parameter_count(const NodeDelegate* self) {
	assert(self != 0);

	size_t count = 0;
	const NodeParameterDefinition* param_def = self->parameter_defs;
	for( ; param_def->type != NodeParameterType__last; ++param_def, ++count);

	return count;
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
	if (NodeDelegate_has_inputs(delegate))
		ret->inputs = 0;
	else {
		size_t input_count = NodeDelegate_input_count(delegate);
		ret->inputs = (Node**)checked_malloc(input_count * sizeof(Node*));

		Node** input_ptr = ret->inputs;
		const NodeInputDefinition* input_def = ret->delegate->input_defs;
		for( ; input_def->type != NodeType__last; ++input_ptr, ++input_def)
			*input_ptr = 0;
	}

	// Setup parameters array
	if (NodeDelegate_has_parameters(delegate))
		ret->parameters = 0;
	else {
		size_t parameter_count = NodeDelegate_parameter_count(delegate);
		
		ret->parameters =
			(NodeParameterValue*)checked_malloc(parameter_count * sizeof(NodeParameterValue));

		NodeParameterValue* param = ret->parameters;
		const NodeParameterDefinition* param_def = delegate->parameter_defs;
		for( ; param_def->type != NodeParameterType__last; ++param, ++param_def)
			NodeParameterValue_copy(
				param,
				&(param_def->default_value),
				param_def->type
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
	if (NodeDelegate_has_inputs(self->delegate)) {
		#ifdef DEBUG
		Node** input_ptr = self->inputs;
		const NodeInputDefinition* input_def = self->delegate->input_defs;
		for( ; input_def->type != NodeType__last; ++input_ptr, ++input_def)
			*input_ptr = 0;
		#endif
	
		free(self->inputs);
	}

	// Deallocate parameters
	NodeParameterValue* param = self->parameters;
	const NodeParameterDefinition* param_def = self->delegate->parameter_defs;
	for( ; param_def->type != NodeParameterType__last; ++param, ++param_def) {
		if (param_def->type == NodeParameterType__string)
			String_destroy(&(param->string_value));
	}

	// Deallocate parameter array
	if (NodeDelegate_has_parameters(self->delegate))
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
	const NodeParameterDefinition** param_def_ptr,
	NodeParameterValue** param_value_ptr
) {
	assert(self != 0);
	assert(name != 0);
	assert(name->data != 0);

	NodeParameterValue* param_value = self->parameters;
	const NodeParameterDefinition* param_def = self->delegate->parameter_defs;
	
	for( ; param_def->type != NodeParameterType__last; ++param_value, ++param_def)
		if (String_equals(name, &(param_def->name))) {
			if (param_def_ptr)
				*param_def_ptr = param_def;

			if (param_value_ptr)
				*param_value_ptr = param_value;

			return true;
		}
		
	return false;
}



bool
Node_set_input_by_name(
	Node* self,
	const String* name,
	Node* other
) {
	assert(self != 0);
	assert(other != 0);
	assert(name != 0);
	assert(name->data != 0);

	// Scan inputs to find one with matching name and types
	Node** input_ptr = self->inputs;
	const NodeInputDefinition* input_def = self->delegate->input_defs;
	for( ; input_def->type != NodeType__last; ++input_ptr, ++input_def)
		if (String_equals(name, &(input_def->name)) && 
		    (other->delegate->type == input_def->type)) {
			*input_ptr = other;
			return true;
		}

	return false;
}


bool
Node_is_complete(
	const Node* self
) {
	assert(self != 0);

	Node* const* input_ptr = self->inputs;
	const NodeInputDefinition* input_def = self->delegate->input_defs;
	for( ; input_def->type != NodeType__last; ++input_ptr, ++input_def)
		if (*input_ptr == 0)
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


NodeOutput
Node_get(
	Node* self
) {
	assert(self != 0);
	assert(self->delegate != 0);
	assert(self->delegate->methods.get != 0);

	return self->delegate->methods.get(self);
}