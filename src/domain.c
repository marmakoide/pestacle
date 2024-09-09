#include <assert.h>
#include "memory.h"
#include "domain.h"


// --- DomainDelegate ---------------------------------------------------------

static bool
DomainDelegate_has_parameters(
	const DomainDelegate* self
) {
	assert(self != 0);

	return self->parameter_defs->type == ParameterType__last;
}


static size_t
DomainDelegate_parameter_count(
	const DomainDelegate* self
) {
	assert(self != 0);

	size_t count = 0;
	const ParameterDefinition* param_def = self->parameter_defs;
	for( ; param_def->type != ParameterType__last; ++param_def, ++count);

	return count;
}


// --- Domain -----------------------------------------------------------------

Domain*
Domain_new(
	const String* name,
	const DomainDelegate* delegate
) {
	assert(delegate != 0);

	// Allocate
	Domain* ret = (Domain*)checked_malloc(sizeof(Domain));

	// Setup
	ret->data = 0;
	ret->delegate = delegate;
	String_clone(&(ret->name), name);

	// Allocate members dictionary
	Dict_init(&(ret->members));

	// Setup parameters array
	if (DomainDelegate_has_parameters(delegate))
		ret->parameters = 0;
	else {
		size_t parameter_count = DomainDelegate_parameter_count(delegate);
		
		ret->parameters =
			(ParameterValue*)checked_malloc(parameter_count * sizeof(ParameterValue));

		ParameterValue* param = ret->parameters;
		const ParameterDefinition* param_def = delegate->parameter_defs;
		for( ; param_def->type != ParameterType__last; ++param, ++param_def)
			ParameterValue_copy(
				param,
				&(param_def->default_value),
				param_def->type
			);
	}

	// Job done
	return ret;
}


void
Domain_destroy(
	Domain* self
) {
	assert(self != 0);
	assert(self->delegate != 0);

	if (self->delegate->methods.destroy)
		self->delegate->methods.destroy(self);

	String_destroy(&(self->name));

	// Deallocate members
	DictIterator it;
	DictIterator_init(&it, &(self->members));
	for( ; DictIterator_has_next(&it); DictIterator_next(&it)) {
		DomainMember* member = (DomainMember*)it.entry->value;
		free(member);
	}

	// Deallocate members dictionary
	Dict_destroy(&(self->members));

	// Deallocate parameters
	ParameterValue* param = self->parameters;
	const ParameterDefinition* param_def = self->delegate->parameter_defs;
	for( ; param_def->type != ParameterType__last; ++param, ++param_def) {
		if (param_def->type == ParameterType__string)
			String_destroy(&(param->string_value));
	}

	if (DomainDelegate_has_parameters(self->delegate))
		free(self->parameters);

	#ifdef DEBUG
	self->data = 0;
	self->delegate = 0;
	self->name.data = 0;
	self->parameters = 0;
	#endif
}


bool
Domain_setup(
	Domain* self,
	WindowManager* window_manager
) {
	assert(self != 0);
	assert(self->delegate != 0);

	if (self->delegate->methods.setup)
		return self->delegate->methods.setup(self, window_manager);

	return true;
}


const DomainMember*
Domain_get_member_by_name(
	Domain* self,
	const String* name
) {
	assert(self != 0);
	assert(name != 0);

	DictEntry* entry = Dict_find(&(self->members), name);
	if (!entry)
		return 0;

	return (const DomainMember*)entry->value;
}


static bool
Domain_add_member(
	Domain* self,
	const DomainMember* member
) {
	assert(self != 0);
	assert(member != 0);

	// Get the entry name
	const String* entry_name;
	switch(member->type) {
		case DomainMemberType__node:
			entry_name = &(member->node->name);
			break;

		case DomainMemberType__node_delegate:
			entry_name = &(member->node_delegate->name);
			break;

		case DomainMemberType__domain_delegate:
			entry_name = &(member->domain_delegate->name);
			break;

		default:
			assert(0);
	}

	// Insert the new entry
	DictEntry* entry = Dict_insert(&(self->members), entry_name);
	if (!entry) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"domain '%s' already have a member named '%s'",
			self->name.data,
			entry_name->data
		);
		return false;
	}

	DomainMember* member_copy =
		(DomainMember*)checked_malloc(sizeof(DomainMember));
	*member_copy = *member;

	entry->value = member_copy;

	// Job done
	return true;
}


bool
Domain_add_node(
	Domain* self,
	Node* node
) {
	assert(self != 0);

	DomainMember member = {
		DomainMemberType__node,
		{ .node = node }
	};

	return Domain_add_member(self, &member);
}


bool
Domain_add_node_delegate(
	Domain* self,
	const NodeDelegate* node_delegate
) {
	assert(self != 0);

	DomainMember member = {
		DomainMemberType__node_delegate,
		{ .node_delegate = node_delegate }
	};

	return Domain_add_member(self, &member);
}


bool
Domain_add_domain_delegate(
	Domain* self,
	const DomainDelegate* domain_delegate
) {
	assert(self != 0);

	DomainMember member = {
		DomainMemberType__domain_delegate,
		{ .domain_delegate = domain_delegate }
	};

	return Domain_add_member(self, &member);
}
