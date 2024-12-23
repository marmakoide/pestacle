#include <assert.h>
#include <pestacle/node.h>
#include <pestacle/memory.h>
#include <pestacle/strings.h>


// --- NodeDelegate -----------------------------------------------------------

static bool
NodeDelegate_has_inputs(
	const NodeDelegate* self
) {
	assert(self);

	return self->input_defs->type != NodeType__last;
}


static size_t
NodeDelegate_input_count(
	const NodeDelegate* self
) {
	assert(self);

	size_t count = 0;
	const NodeInputDefinition* input_def = self->input_defs;
	for( ; input_def->type != NodeType__last; ++input_def, ++count);

	return count;
}


// --- Node -------------------------------------------------------------------

Node*
Node_new(
	const char* name,
	const NodeDelegate* delegate,
	struct s_Scope* delegate_scope
) {
	assert(name);
	assert(delegate);
	assert(delegate->type != NodeType__invalid);
	assert(delegate->type != NodeType__last);

	// Allocate
	Node* ret = (Node*)checked_malloc(sizeof(Node));

	// Setup
	ret->data = 0;
	ret->name = strclone(name);
	ret->delegate = delegate;
	ret->delegate_scope = delegate_scope;

	// Setup inputs array
	if (NodeDelegate_has_inputs(delegate)) {
		size_t input_count = NodeDelegate_input_count(delegate);
		ret->inputs = (Node**)checked_malloc(input_count * sizeof(Node*));

		Node** input_ptr = ret->inputs;
		const NodeInputDefinition* input_def = ret->delegate->input_defs;
		for( ; input_def->type != NodeType__last; ++input_ptr, ++input_def)
			*input_ptr = 0;
	}
	else
		ret->inputs = 0;
	
	// Setup parameters array
	ret->parameters = ParameterValue_new(delegate->parameter_defs);
	
	// Job done
	return ret;
}


void
Node_destroy(
	Node* self
) {
	assert(self);
	assert(self->delegate);

	if (self->delegate->methods.destroy)
		self->delegate->methods.destroy(self);

	// Deallocate the name
	free(self->name);

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
	if (self->parameters) {
		ParameterValue_destroy(self->parameters, self->delegate->parameter_defs);
		free(self->parameters);
	}
	
	#ifdef DEBUG
	self->data = 0;
	self->name = 0;
	self->delegate = 0;
	self->delegate_scope = 0;
	self->inputs = 0;
	self->parameters = 0;
	#endif
}


bool
Node_get_parameter_by_name(
	Node* self,
	const char* name,
	const ParameterDefinition** param_def_ptr,
	ParameterValue** param_value_ptr
) {
	assert(self);
	assert(name);

	ParameterValue* param_value = self->parameters;
	const ParameterDefinition* param_def = self->delegate->parameter_defs;

	for( ; param_def->type != ParameterType__last; ++param_value, ++param_def)
		if (strcmp(name, param_def->name) == 0) {
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
	const char* name,
	Node* other
) {
	assert(self);
	assert(other);
	assert(name);

	// Scan inputs to find one with matching name and types
	Node** input_ptr = self->inputs;
	const NodeInputDefinition* input_def = self->delegate->input_defs;
	for( ; input_def->type != NodeType__last; ++input_ptr, ++input_def)
		if ((strcmp(name, input_def->name) == 0) && 
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
	assert(self);

	Node* const* input_ptr = self->inputs;
	const NodeInputDefinition* input_def = self->delegate->input_defs;
	for( ; input_def->type != NodeType__last; ++input_ptr, ++input_def)
		if ((*input_ptr == 0) && (input_def->is_mandatory))
			return false;

	return true;
}


bool
Node_setup(
	Node* self
) {
	assert(self);
	assert(self->delegate);

	if (self->delegate->methods.setup)
		return self->delegate->methods.setup(self);

	return true;
}


void
Node_update(
	Node* self
) {
	assert(self);
	assert(self->delegate);

	if (self->delegate->methods.update)
		self->delegate->methods.update(self);
}


NodeOutput
Node_output(
	Node* self
) {
	assert(self);
	assert(self->delegate);
	assert(self->delegate->methods.output);

	return self->delegate->methods.output(self);
}
