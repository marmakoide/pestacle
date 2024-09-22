#include <assert.h>
#include "memory.h"
#include "scope.h"


// --- ScopeMember ------------------------------------------------------------

static void
ScopeMember_destroy(
	ScopeMember* self
) {
	assert(self != 0);

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
		node_delegate->name.data
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
		scope_delegate->name.data
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
		node->name.data,
		node->delegate->name.data
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
			it.entry->key->data
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
	assert(self != 0);

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
	const String* name,
	const ScopeDelegate* delegate,
	Scope* delegate_scope
) {
	assert(name != 0);
	assert(delegate != 0);

	// Allocate
	Scope* ret = (Scope*)checked_malloc(sizeof(Scope));

	// Setup
	ret->data = 0;
	String_clone(&(ret->name), name);
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
	assert(self != 0);
	assert(self->delegate != 0);
	
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

	// Destroy name
	String_destroy(&(self->name));

	#ifdef DEBUG
	self->data = 0;
	self->name.data = 0;
	self->delegate = 0;
	self->delegate_scope = 0;	
	self->parameters = 0;
	#endif
}


bool
Scope_setup(
	Scope* self,
	WindowManager* window_manager
) {
	assert(self != 0);
	assert(self->delegate != 0);

	if (self->delegate->methods.setup)
		return self->delegate->methods.setup(self, window_manager);

	return true;
}


bool
Scope_get_parameter_by_name(
	Scope* self,
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


extern ScopeMember*
Scope_get_member(
	Scope* self,
	const String* path,
	size_t path_len
) {
	assert(self != 0);
	assert(path != 0);

	Scope* current = self;
	ScopeMember* member = 0;
	const String* name_ptr = path;
	for(size_t i = path_len; i != 0; --i, ++name_ptr) {
		DictEntry* entry = Dict_find(&(current->members), name_ptr);
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
	const String* name,
	const ScopeMember* member
) {
	assert(self != 0);
	assert(name != 0);
	assert(member != 0);

	// Insert the new entry
	DictEntry* entry = Dict_insert(&(self->members), name);
	if (!entry) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"scope '%s' already have a member named '%s'",
			self->name.data,
			name->data
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
	assert(self != 0);
	assert(node != 0);

	ScopeMember member = {
		ScopeMemberType__node,
		{ .node = node },
		self
	};

	if (!Scope_add_member(self, &(node->name), &member))
		return false;

	return true;
}


bool
Scope_add_scope(
	Scope* self,
	Scope* scope
) {
	assert(self != 0);
	assert(scope != 0);

	ScopeMember member = {

		ScopeMemberType__scope,
		{ .scope = scope },
		self
	};

	if (!Scope_add_member(self, &(scope->name), &member))
		return false;

	return true;
}


bool
Scope_add_node_delegate(
	Scope* self,
	const NodeDelegate* node_delegate
) {
	assert(self != 0);

	ScopeMember member = {
		ScopeMemberType__node_delegate,
		{ .node_delegate = node_delegate },
		self
	};

	return
		Scope_add_member(self, &(node_delegate->name), &member);
}


bool
Scope_add_scope_delegate(
	Scope* self,
	const ScopeDelegate* scope_delegate
) {
	assert(self != 0);

	ScopeMember member = {
		ScopeMemberType__scope_delegate,
		{ .scope_delegate = scope_delegate },
		self
	};

	return
		Scope_add_member(self, &(scope_delegate->name), &member);
}


void
Scope_print(
	Scope* self,
	FILE* out
) {
	assert(self != 0);

	ScopeMember root = {
		ScopeMemberType__scope,
		{ .scope = self },
		0
	};

	ScopeMember_print(&root, out, 0);
}
