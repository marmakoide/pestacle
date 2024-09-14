#include <assert.h>
#include "memory.h"
#include "domain.h"


// --- DomainMember -----------------------------------------------------------

static void
DomainMember_print_node_delegate(
	DomainMember* self,
	FILE* out,
	size_t i
) {
	const NodeDelegate* node_delegate = self->node_delegate;

	fprintf(
		out,
		": node-delegate %s",
		node_delegate->name.data
	);
}


static void
DomainMember_print_domain_delegate(
	DomainMember* self,
	FILE* out,
	size_t i
) {
	const DomainDelegate* domain_delegate = self->domain_delegate;

	fprintf(
		out,
		": domain-delegate %s",
		domain_delegate->name.data
	);
}


static void
DomainMember_print_node(
	DomainMember* self,
	FILE* out,
	size_t i
) {
	Node* node = self->node;

	fprintf(
		out,
		": node %s (node-delegate %s)",
		node->name.data,
		node->delegate->name.data
	);
}


static void
DomainMember_print(
	DomainMember* self,
	FILE* out,
	size_t i
);


static void
DomainMember_print_domain(
	DomainMember* self,
	FILE* out,
	size_t i
) {
	Domain* domain = self->domain;

	DictIterator it;
	DictIterator_init(&it, &(domain->members));
	for( ; DictIterator_has_next(&it); DictIterator_next(&it)) {
		for(size_t j = 4 * i; j != 0; --j)
			fputc(' ', out);

		DomainMember* member = (DomainMember*)it.entry->value;
		fprintf(
			out,
			"- %s",
			it.entry->key->data
		);

		if (member->type == DomainMemberType__domain)
			fputc('\n', out);

		DomainMember_print(member, out, i + 1);

		if (member->type != DomainMemberType__domain)
			fputc('\n', out);
	}
}


static void
DomainMember_print(
	DomainMember* self,
	FILE* out,
	size_t i
) {
	assert(self != 0);

	switch(self->type) {
		case DomainMemberType__node:
			DomainMember_print_node(self, out, i);
			break;

		case DomainMemberType__domain:
			DomainMember_print_domain(self, out, i);
			break;

		case DomainMemberType__node_delegate:
			DomainMember_print_node_delegate(self, out, i);
			break;

		case DomainMemberType__domain_delegate:
			DomainMember_print_domain_delegate(self, out, i);
			break;

		default:
			assert(0);
	}
}


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


bool
Domain_get_parameter_by_name(
	Domain* self,
	const String* name,
	const ParameterDefinition** param_def_ptr,
	ParameterValue** param_value_ptr
) {
	assert(self != 0);
	assert(name != 0);
	assert(name->data != 0);

	ParameterValue* param_value = self->parameters;
	const ParameterDefinition* param_def = self->delegate->parameter_defs;
	
	for( ; param_def->type != ParameterType__last; ++param_value, ++param_def)
		if (String_equals(name, &(param_def->name))) {
			if (param_def_ptr)
				*param_def_ptr = param_def;

			if (param_value_ptr)
				*param_value_ptr = param_value;

			return true;
		}
		
	return false;
}


extern DomainMember*
Domain_get_member(
	Domain* self,
	const String* path,
	size_t path_len
) {
	assert(self != 0);
	assert(path != 0);

	Domain* current = self;
	DomainMember* member = 0;
	const String* name_ptr = path;
	for(size_t i = path_len; i != 0; --i, ++name_ptr) {
		DictEntry* entry = Dict_find(&(current->members), name_ptr);
		if (!entry)
			return 0;
			
		member = (DomainMember*)entry->value;
		if ((i > 1) && (member->type != DomainMemberType__domain))
			return 0;

		current = member->domain;
	}

	return member;
}



static bool
Domain_add_member(
	Domain* self,
	const String* name,
	const DomainMember* member
) {
	assert(self != 0);
	assert(name != 0);
	assert(member != 0);

	// Insert the new entry
	DictEntry* entry = Dict_insert(&(self->members), name);
	if (!entry) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"domain '%s' already have a member named '%s'",
			self->name.data,
			name->data
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
	assert(node != 0);

	DomainMember member = {
		DomainMemberType__node,
		{ .node = node }
	};

	return
		Domain_add_member(self, &(node->name), &member);
}


bool
Domain_add_domain(
	Domain* self,
	Domain* domain
) {
	assert(self != 0);
	assert(domain != 0);

	DomainMember member = {
		DomainMemberType__domain,
		{ .domain = domain }
	};

	return
		Domain_add_member(self, &(domain->name), &member);
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

	return
		Domain_add_member(self, &(node_delegate->name), &member);
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

	return
		Domain_add_member(self, &(domain_delegate->name), &member);
}


void
Domain_print(
	Domain* self,
	FILE* out
) {
	assert(self != 0);

	DomainMember root = {
		DomainMemberType__domain,
		{ .domain = self }
	};

	DomainMember_print(&root, out, 0);
}
