#include <assert.h>
#include <SDL_log.h>
#include <pestacle/strings.h>
#include <pestacle/parser/scope_populate.h>


static ScopeMember*
get_scope_member(
	Scope* scope,
	FileLocation* location,
	const StringListView* str_list
) {
	ScopeMember* member = Scope_get_member(scope, str_list);

	if (!member) {
		char* path_str = StringListView_join(str_list, '.');
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %u : path %s is invalid",
			location->line + 1,
			path_str
		);
		free(path_str);
	}

	return member;
}


static Node*
get_node(
	Scope* scope,
	FileLocation* location,
	const StringListView* str_list
) {
	ScopeMember* member = get_scope_member(scope, location, str_list);

	if (!member)
		return 0;

	if (member->type != ScopeMemberType__node) {
		char* path_str = StringListView_join(str_list, '.');
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %u : path %s is not a node",
			location->line + 1,
			path_str
		);
		free(path_str);
		return 0;
	}

	return member->node;
}


static int
process_parameter(
	const ParameterDefinition* param_def,
	ParameterValue* param_value,
	const AST_Parameter* ast_param
) {
	assert(param_def);
	assert(param_value);
	assert(ast_param);

	int error_count = 0;

	switch(param_def->type) {
		case ParameterType__last:
		case ParameterType__invalid:
			assert(0);
			break;
		
		case ParameterType__bool:
			if (ast_param->value.type == AST_AtomicValueType__bool)
				param_value->bool_value = ast_param->value.bool_value;
			else {
				error_count += 1;
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %u : parameter '%s' should be a boolean value",
					ast_param->location.line + 1,
					param_def->name
				);
			}
			break;
		
		case ParameterType__integer:
			if (ast_param->value.type == AST_AtomicValueType__integer)
				param_value->int64_value = ast_param->value.int64_value;
			else {
				error_count += 1;
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %u : parameter '%s' should be an integer value",
					ast_param->location.line + 1,
					param_def->name
				);
			}
			break;
		
		case ParameterType__real:
			if (ast_param->value.type == AST_AtomicValueType__real)
				param_value->real_value = ast_param->value.real_value;
			else if (ast_param->value.type == AST_AtomicValueType__integer)
				param_value->real_value = ast_param->value.int64_value;
			else {
				error_count += 1;
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %u : parameter '%s' should be a numerical value",
					ast_param->location.line + 1,
					param_def->name
				);
			}
			break;
		
		case ParameterType__string:
			if (ast_param->value.type == AST_AtomicValueType__string)
				param_value->string_value = strclone(ast_param->value.string_value);
			else {
				error_count += 1;
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %u : parameter '%s' should be a string value",
					ast_param->location.line + 1,
					param_def->name
				);
			}
			break;
	}

	return error_count;
}


static int
process_node_parameters(
	Node* node,
	AST_Statement* stat
) {
	assert(node);
	assert(stat);
	assert(stat->type == AST_StatementType__instanciation);

	int error_count = 0;

	// For each parameter
	ParameterValue* param_value = node->parameters;
	const ParameterDefinition* param_def = node->delegate->parameter_defs;

	for( ; param_def->type != ParameterType__last; ++param_value, ++param_def) {
		DictEntry* entry =
			Dict_find(&(stat->instanciation.parameters), param_def->name);
		
		if (entry)
			error_count +=
				process_parameter(
					param_def,
					param_value,
					(const AST_Parameter*)entry->value
				);
	}

	// Job done
	return error_count;
}


static int
process_node_delegate_instanciation(
	Scope* scope,
	ScopeMember* member,
	AST_Statement* stat
) {
	assert(scope);
	assert(stat);
	assert(stat->type == AST_StatementType__instanciation);

	int error_count = 0;

	// Build the node
	Node* node = 
		Node_new(
			StringList_items(&(stat->instanciation.dst.string_list))[0],
			member->node_delegate,
			member->parent
		);

	if (!node) {
		error_count += 1;
		goto termination;
	}

	// Set parameters
	error_count += process_node_parameters(node, stat);

	// Add the node to the root scope
	Scope_add_node(scope, node);

	// Job done
termination:
	return error_count;
}


static int
process_scope_parameters(
	Scope* scope,
	AST_Statement* stat
) {
	assert(scope);
	assert(stat);
	assert(stat->type == AST_StatementType__instanciation);

	int error_count = 0;

	// For each parameter
	ParameterValue* param_value = scope->parameters;
	const ParameterDefinition* param_def = scope->delegate->parameter_defs;

	for( ; param_def->type != ParameterType__last; ++param_value, ++param_def) {
		DictEntry* entry =
			Dict_find(&(stat->instanciation.parameters), param_def->name);
		
		if (entry)
			error_count +=
				process_parameter(
					param_def,
					param_value,
					(const AST_Parameter*)entry->value
				);
	}

	// Job done
	return error_count;
}


static int
process_scope_delegate_instanciation(
	Scope* scope,
	ScopeMember* member,
	AST_Statement* stat
) {
	assert(scope);
	assert(stat);
	assert(stat->type == AST_StatementType__instanciation);

	int error_count = 0;

	// Build the scope
	Scope* new_scope =
		Scope_new(
			StringList_items(&(stat->instanciation.dst.string_list))[0],
			member->scope_delegate, 
			member->parent
		);

	if (!new_scope) {
		error_count += 1;
		goto termination;
	}

	// Set parameters
	error_count += process_scope_parameters(new_scope, stat);

	// Add the newly build scope
	if (!Scope_setup(new_scope)) {
		error_count += 1;
		goto termination;
	}

	Scope_add_scope(scope, new_scope);

	// Job done
termination:
	return error_count;
}


static int
process_instanciation(
	Scope* scope,
	AST_Statement* stat
) {
	assert(scope);
	assert(stat);
	assert(stat->type == AST_StatementType__instanciation);

	int error_count = 0;

	// Retrieve source
	StringListView src_path;
	StringListView_init(&src_path, &(stat->instanciation.src.string_list));

	ScopeMember* member = Scope_get_member(scope, &src_path);
	if (!member) {
		error_count += 1;
		char* path_str = StringListView_join(&src_path, '.');
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %u : path %s is not a valid path",
			stat->instanciation.src.location.line + 1,
			path_str
		);
		free(path_str);
	}

	// Check that the src path leads to a builder
	if (member) {
		switch(member->type) {
			case ScopeMemberType__node_delegate:
				error_count +=
					process_node_delegate_instanciation(
						scope, member, stat
					);
				break;

			case ScopeMemberType__scope_delegate:
				error_count += 
					process_scope_delegate_instanciation(
						scope, member, stat
					);
				break;

			default:
				error_count += 1;
				char* path_str = StringListView_join(&src_path, '.');
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %u : path %s is not an instanciable entity",
					stat->instanciation.src.location.line + 1,
					path_str
				);
				free(path_str);
		}
	}

	// Job done
	return error_count;
}


static int
process_slot_assignment(
	Scope* scope,
	AST_Statement* stat
) {
	assert(scope);
	assert(stat);
	assert(stat->type == AST_StatementType__slot_assignment);

	int error_count = 0;

	// Retrieve source node
	StringListView src_path;
	StringListView_init(&src_path, &(stat->slot_assignment.src.string_list));

	Node* src_node = 0;

	if (StringListView_length(&src_path) != 1) {
		error_count += 1;
		char* path_str = StringListView_join(&src_path, '.');
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %u : source path %s is not a path to a node instance",
			stat->slot_assignment.src.location.line + 1,
			path_str
		);
		free(path_str);
	}
	else {
		src_node = get_node(
			scope,
			&(stat->slot_assignment.src.location),
			&src_path
		);

		if (!src_node)
			error_count += 1;
	}

	// Retrieve destination node and parameter
	StringListView dst_path;
	StringListView_init(&dst_path, &(stat->slot_assignment.dst.string_list));

	StringListView dst_path_head;
	StringListView_init(&dst_path, &(stat->slot_assignment.dst.string_list));
	StringListView_head(&dst_path_head, StringListView_length(&dst_path) - 1);

	Node* dst_node = get_node(
		scope,
		&(stat->slot_assignment.dst.location),
		&dst_path_head
	);

	if (!dst_node)
		error_count += 1;

	// Assign the input to the dst node
	if (src_node && dst_node) {
		const char* dst_input_name =
			StringListView_at(&dst_path, StringListView_length(&dst_path) - 1);

		if (!Node_set_input_by_name(
			dst_node,
			dst_input_name,
			src_node)) {
			error_count += 1;
			char* path_str = StringListView_join(&dst_path, '.');
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"line %u : destination path %s is not a path to a node's input",
				stat->slot_assignment.dst.location.line + 1,
				path_str
			);
		free(path_str);
		}
	}

	// Job done
	return error_count;
}


bool
Scope_populate_from_AST(
	Scope* self,
	AST_Unit* unit
) {
	assert(self);
	assert(unit);

	int error_count = 0;

	// For each instanciation statement
	for(AST_Statement* stat = unit->head; stat; stat = stat->next)
		if (stat->type == AST_StatementType__instanciation)
			error_count += process_instanciation(self, stat);

	// For each slot assignment
	for(AST_Statement* stat = unit->head; stat; stat = stat->next)
		if (stat->type == AST_StatementType__slot_assignment)
			error_count += process_slot_assignment(self, stat);

	// Job done
	return error_count == 0;
}