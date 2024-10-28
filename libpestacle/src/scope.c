#include <assert.h>
#include <pestacle/memory.h>
#include <pestacle/strings.h>
#include <pestacle/scope.h>


// --- ScopeMember ------------------------------------------------------------

static void
ScopeMember_destroy(
	ScopeMember* self
) {
	assert(self);

	switch(self->type) {
		case ScopeMemberType__node:
			Node_destroy(self->node);
			free(self->node);
			break;

		case ScopeMemberType__scope:
			Scope_destroy(self->scope);
			free(self->scope);
			break;

		case ScopeMemberType__node_delegate:
		case ScopeMemberType__scope_delegate:
			break;

		default:
			assert(0);
	}
}


static void
ScopeMember_print_node_delegate(
	ScopeMember* self,
	FILE* out
) {
	const NodeDelegate* node_delegate = self->node_delegate;

	fprintf(
		out,
		": node-delegate %s",
		node_delegate->name
	);
}


static void
ScopeMember_print_scope_delegate(
	ScopeMember* self,
	FILE* out
) {
	const ScopeDelegate* scope_delegate = self->scope_delegate;

	fprintf(
		out,
		": scope-delegate %s",
		scope_delegate->name
	);
}


static void
ScopeMember_print_node(
	ScopeMember* self,
	FILE* out
) {
	Node* node = self->node;

	fprintf(
		out,
		": node %s (node-delegate %s)",
		node->name,
		node->delegate->name
	);
}


static void
ScopeMember_print(
	ScopeMember* self,
	FILE* out,
	size_t i
);


static void
ScopeMember_print_scope(
	ScopeMember* self,
	FILE* out,
	size_t i
) {
	Scope* scope = self->scope;

	DictIterator it;
	DictIterator_init(&it, &(scope->members));
	for( ; DictIterator_has_next(&it); DictIterator_next(&it)) {
		for(size_t j = 4 * i; j != 0; --j)
			fputc(' ', out);

		ScopeMember* member = (ScopeMember*)it.entry->value;
		fprintf(
			out,
			"- %s",
			it.entry->key
		);

		if (member->type == ScopeMemberType__scope)
			fputc('\n', out);

		ScopeMember_print(member, out, i + 1);

		if (member->type != ScopeMemberType__scope)
			fputc('\n', out);
	}
}


static void
ScopeMember_print(
	ScopeMember* self,
	FILE* out,
	size_t i
) {
	assert(self);

	switch(self->type) {
		case ScopeMemberType__node:
			ScopeMember_print_node(self, out);
			break;

		case ScopeMemberType__scope:
			ScopeMember_print_scope(self, out, i);
			break;


		case ScopeMemberType__node_delegate:
			ScopeMember_print_node_delegate(self, out);
			break;

		case ScopeMemberType__scope_delegate:
			ScopeMember_print_scope_delegate(self, out);
			break;

		default:
			assert(0);
	}
}


// --- Scope ------------------------------------------------------------------

Scope*
Scope_new(
	const char* name,
	const ScopeDelegate* delegate,
	Scope* delegate_scope
) {
	assert(name);
	assert(delegate);

	// Allocate
	Scope* ret = (Scope*)checked_malloc(sizeof(Scope));

	// Setup
	ret->data = 0;
	ret->name = strclone(name);
	ret->delegate = delegate;
	ret->delegate_scope = delegate_scope;

	// Allocate members dictionary
	Dict_init(&(ret->members));

	// Setup parameters array
	ret->parameters = ParameterValue_new(delegate->parameter_defs);

	// Job done
	return ret;
}


void
Scope_destroy(
	Scope* self
) {
	assert(self);
	assert(self->delegate);
	
	if (self->delegate->methods.destroy)
		self->delegate->methods.destroy(self);

	// Deallocate members
	DictIterator it;
	DictIterator_init(&it, &(self->members));
	for( ; DictIterator_has_next(&it); DictIterator_next(&it)) {
		ScopeMember* member = (ScopeMember*)it.entry->value;
		ScopeMember_destroy(member);
		free(member);
	}

	// Deallocate members dictionary
	Dict_destroy(&(self->members));

	// Deallocate parameters
	if (self->parameters) {
		ParameterValue_destroy(self->parameters, self->delegate->parameter_defs);
		free(self->parameters);
	}

	// Deallocate the name name
	free(self->name);

	#ifdef DEBUG
	self->data = 0;
	self->name = 0;
	self->delegate = 0;
	self->delegate_scope = 0;	
	self->parameters = 0;
	#endif
}


bool
Scope_setup(
	Scope* self
) {
	assert(self);
	assert(self->delegate);

	if (self->delegate->methods.setup)
		return self->delegate->methods.setup(self);

	return true;
}


bool
Scope_get_parameter_by_name(
	Scope* self,
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


extern ScopeMember*
Scope_get_member(
	Scope* self,
	const char** path,
	size_t path_len
) {
	assert(self);
	assert(path);

	Scope* current = self;
	ScopeMember* member = 0;
	const char** name_ptr = path;
	for(size_t i = path_len; i != 0; --i, ++name_ptr) {
		DictEntry* entry = Dict_find(&(current->members), *name_ptr);
		if (!entry)
			return 0;
			
		member = (ScopeMember*)entry->value;
		if ((i > 1) && (member->type != ScopeMemberType__scope))
			return 0;

		current = member->scope;
	}

	return member;
}



static bool
Scope_add_member(
	Scope* self,
	const char* name,
	const ScopeMember* member
) {
	assert(self);
	assert(name);
	assert(member);

	// Insert the new entry
	DictEntry* entry = Dict_insert(&(self->members), name);
	if (!entry) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"scope '%s' already have a member named '%s'",
			self->name,
			name
		);
		return false;
	}

	ScopeMember* member_copy =
		(ScopeMember*)checked_malloc(sizeof(ScopeMember));
	*member_copy = *member;

	entry->value = member_copy;

	// Job done
	return true;
}


bool
Scope_add_node(
	Scope* self,
	Node* node
) {
	assert(self);
	assert(node);

	ScopeMember member = {
		ScopeMemberType__node,
		{ .node = node },
		self
	};

	if (!Scope_add_member(self, node->name, &member))
		return false;

	return true;
}


bool
Scope_add_scope(
	Scope* self,
	Scope* scope
) {
	assert(self);
	assert(scope);

	ScopeMember member = {

		ScopeMemberType__scope,
		{ .scope = scope },
		self
	};

	if (!Scope_add_member(self, scope->name, &member))
		return false;

	return true;
}


bool
Scope_instanciate_scope(
	Scope* self,
	const ScopeDelegate* scope_delegate
) {
	assert(self);
	assert(scope_delegate);

	bool ret = true;
	Scope* scope = 0;

	// Build the scope
	scope = Scope_new(scope_delegate->name, scope_delegate, 0);
	if (!scope) {
		ret = false;
		goto termination;
	}

	// Setup the scope
	if (!Scope_setup(scope)) {
		ret = false;
		goto termination;
	}

	// Add the scope to the root scope
	if (!Scope_add_scope(self, scope)) {
		ret = false;
		goto termination;
	}

	// Job done
termination:
	if ((scope) && (!ret)) {
		Scope_destroy(scope);
		free(scope);
	}

	return ret;
}


bool
Scope_add_node_delegate(
	Scope* self,
	const NodeDelegate* node_delegate
) {
	assert(self);

	ScopeMember member = {
		ScopeMemberType__node_delegate,
		{ .node_delegate = node_delegate },
		self
	};

	return
		Scope_add_member(self, node_delegate->name, &member);
}


bool
Scope_add_scope_delegate(
	Scope* self,
	const ScopeDelegate* scope_delegate
) {
	assert(self);

	ScopeMember member = {
		ScopeMemberType__scope_delegate,
		{ .scope_delegate = scope_delegate },
		self
	};

	return
		Scope_add_member(self, scope_delegate->name, &member);
}


bool
Scope_populate(
	Scope* self,
	const NodeDelegate** node_delegate_list,
	const ScopeDelegate** scope_delegate_list,
	const ScopeDelegate** scope_instance_delegate_list
) {
	assert(self);

	if (node_delegate_list) {
		const NodeDelegate** node_delegate_ptr = node_delegate_list;
		for( ; *node_delegate_ptr != 0; ++node_delegate_ptr)
			if (!Scope_add_node_delegate(self, *node_delegate_ptr))
				return false;
	}

	if (scope_delegate_list) {
		const ScopeDelegate** scope_delegate_ptr = scope_delegate_list;
		for( ; *scope_delegate_ptr != 0; ++scope_delegate_ptr)
			if (!Scope_add_scope_delegate(self, *scope_delegate_ptr))
				return false;
	}

	if (scope_instance_delegate_list) {
		const ScopeDelegate** scope_delegate_ptr = scope_instance_delegate_list;
		for( ; *scope_delegate_ptr != 0; ++scope_delegate_ptr)
			if (!Scope_instanciate_scope(self, *scope_delegate_ptr))
				return false;
	}

	return true;
}


void
Scope_print(
	Scope* self,
	FILE* out
) {
	assert(self);

	ScopeMember root = {
		ScopeMemberType__scope,
		{ .scope = self },
		0
	};

	ScopeMember_print(&root, out, 0);
}
