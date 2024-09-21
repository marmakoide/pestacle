#include <stdlib.h>
#include <assert.h>
#include "errors.h"
#include "memory.h"
#include "string_list.h"
#include "parser/parser.h"


// --- ParserContext implementation ------------------------------------------

typedef struct {
	Lexer* lexer;
	Scope* scope;
	WindowManager* window_manager;
} ParseContext;


static void
ParseContext_init(
	ParseContext* self,
	Lexer* lexer,
	Scope* scope,
	WindowManager* window_manager
) {
	assert(self != 0);
	assert(lexer != 0);
	assert(scope != 0);
	assert(window_manager != 0);

	self->lexer = lexer;
	self->scope = scope;
	self->window_manager = window_manager;
}


#ifdef DEBUG
static void
ParseContext_destroy(
	ParseContext* self
) {
	assert(self != 0);

	self->lexer = 0;
	self->scope = 0;
	self->window_manager = 0;
}
#endif


// --- Parser utilities -------------------------------------------------------

static ScopeMember*
Parser_get_scope_member(
	ParseContext* context,
	const String* path,
	size_t path_len
) {
	ScopeMember* member =
		Scope_get_member(context->scope, path, path_len);

	if (!member)
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : invalid path\n",
			context->lexer->token.location.line + 1
		);

	return member;
}


static Node*
Parser_get_node(
	ParseContext* context,
	const String* path,
	size_t path_len
) {
	ScopeMember* member =
		Parser_get_scope_member(context, path, path_len);

	if (!member)
		return 0;

	if (member->type != ScopeMemberType__node) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : path is not a node\n",
			context->lexer->token.location.line + 1
		);
		return 0;
	}

	return member->node;
}


// --- Parser recursive descent implementation --------------------------------

#define MAX_PARSING_ERROR_COUNT 16


bool
Parser_parse_parameter(
	ParseContext* context,
	ScopeMember* member
) {
	assert(
		(member->type == ScopeMemberType__node) ||
		(member->type == ScopeMemberType__scope)
	);

	bool ret = true;

	// Parse an identifier
	if (context->lexer->token.type != TokenType__identifier)
		handle_processing_error(
			&(context->lexer->token.location),
			"expected an identifier, got '%s' instead",
			context->lexer->token.text
		);

	String name_str;
	String_clone(&name_str, Lexer_token_text(context->lexer));

	// Fetch the parameter 
	ParameterValue* param_value = 0;
	ParameterDefinition const* param_def = 0;

	if (member->type == ScopeMemberType__node) {
		Node* node = member->node;
		if (!Node_get_parameter_by_name(node, &name_str, &param_def, &param_value)) {
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"line %d : node '%s' does not have a '%s' parameter\n",
				context->lexer->token.location.line + 1,
				node->name.data,
				name_str.data
			);
			ret = false;
			goto termination;
		}
	}
	else if (member->type == ScopeMemberType__scope) {
		Scope* scope = member->scope;
		if (!Scope_get_parameter_by_name(scope, &name_str, &param_def, &param_value)) {
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"line %d : scope '%s' does not have a '%s' parameter\n",
				context->lexer->token.location.line + 1,
				scope->name.data,
				name_str.data
			);
			ret = false;
			goto termination;
		}
	}

	// Parse '='
	Lexer_next_token(context->lexer);
	if (context->lexer->token.type != TokenType__equal)
		handle_processing_error(
			&(context->lexer->token.location),
			"expected '=', got '%s' instead",
			context->lexer->token.text
		);

	// Assign the value to the node parameter
	Lexer_next_token(context->lexer);
	switch(param_def->type) {
		case ParameterType__bool:
			if (context->lexer->token.type == TokenType__bool)
				param_value->bool_value = context->lexer->token.value.bool_value;
			else {
				ret = false;
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %d : expected a boolean value, got '%s' instead\n",
					context->lexer->token.location.line + 1,
					context->lexer->token.text_data
				);
			}
			break;

		case ParameterType__integer:
			if (context->lexer->token.type == TokenType__integer)
				param_value->int64_value = context->lexer->token.value.int64_value;
			else {
				ret = false;
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %d : expected an integer value, got '%s' instead\n",
					context->lexer->token.location.line + 1,
					context->lexer->token.text_data
				);
			}
			break;

		case ParameterType__real:
			if ((context->lexer->token.type == TokenType__integer) || (context->lexer->token.type == TokenType__real))
				param_value->real_value = context->lexer->token.value.real_value;
			else {
				ret = false;
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %d : expected a numeric value, got '%s' instead",
					context->lexer->token.location.line + 1,
					context->lexer->token.text_data
				);
			}
			break;

		case ParameterType__string:
			if (context->lexer->token.type == TokenType__string) {
				String_destroy(&(param_value->string_value));
				String_clone(&(param_value->string_value), Lexer_token_text(context->lexer));
			}
			else {
				ret = false;
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %d : expected a string constant, got '%s' instead",
					context->lexer->token.location.line + 1,
					context->lexer->token.text_data
				);
			}
			break;

		default:
			assert(0);
	}

termination:
	// Free ressources
	String_destroy(&name_str);

	// Job done
	return ret;
}


bool
Parser_parse_parameter_list(
	ParseContext* context,
	ScopeMember* member
) {
	bool ret = true;

	// Parse '('
	if (context->lexer->token.type != TokenType__pth_open)
		handle_processing_error(
			&(context->lexer->token.location),
			"expected '(', got '%s' instead",
			context->lexer->token.text
		);

	// Parse each parameter one by one
	bool first = true;
	for(bool done = false; !done; first = false) {
		Lexer_next_token(context->lexer);
		switch(context->lexer->token.type) {
			case TokenType__pth_close:
				done = true;
				break;

			case TokenType__eof:
				done = true;
				ret = false;
				break;

			default:
				if (!first) {
					if (context->lexer->token.type != TokenType__comma)
						handle_processing_error(
							&(context->lexer->token.location),
							"expected ')', got '%s' instead",
							context->lexer->token.text
						);
					Lexer_next_token(context->lexer);
				}

				if (!Parser_parse_parameter(context, member))
					ret = false;
		}
	}

	// Parse ')'
	if (context->lexer->token.type != TokenType__pth_close)
		handle_processing_error(
			&(context->lexer->token.location),
			"expected ')', got '%s' instead",
			context->lexer->token.text
		);

	// Job done
	return ret;
}


static bool
Parser_parse_identifier_path(
	ParseContext* context,
	StringList* str_list
) {
	bool ret = true;

	// Parse first identifier
	if (context->lexer->token.type != TokenType__identifier) {
		handle_processing_error(
			&(context->lexer->token.location),
			"expected an identifier, got '%s' instead",
			context->lexer->token.text
		);	
	}
	StringList_append(str_list, Lexer_token_text(context->lexer));
	Lexer_next_token(context->lexer);

	// Parse the following identifiers
	while(context->lexer->token.type == TokenType__dot) {
		Lexer_next_token(context->lexer);
		if (context->lexer->token.type != TokenType__identifier) {
			handle_processing_error(
				&(context->lexer->token.location),
				"expected an identifier, got '%s' instead",
				context->lexer->token.text
			);
		}
		StringList_append(str_list, Lexer_token_text(context->lexer));
		Lexer_next_token(context->lexer);
	}
	
	return ret;
}


static bool
Parser_parse_node_delegate_instanciation(
	ParseContext* context,
	const String* name,
	ScopeMember* member
) {
	// Build the node
	Node* node = Node_new(
		name,
		member->node_delegate,
		member->parent
	);

	// Parse parameters
	ScopeMember constructor_output = {
		ScopeMemberType__node,
		{ .node = node },
		0
	};

	if (!Parser_parse_parameter_list(context, &constructor_output))
		return false;

	// Add the node to the root scope
	Scope_add_node(context->scope, node);


	// Job done
	return true;
}


static bool
Parser_parse_scope_delegate_instanciation(
	ParseContext* context,
	const String* name,
	const ScopeMember* member
) {
	// Build the scope
	Scope* scope = Scope_new(
		name,
		member->scope_delegate,
		member->parent
	);

	if (!scope)
		return false;

	// Parse parameters
	ScopeMember constructor_output = {
		ScopeMemberType__scope,
		{ .scope = scope },
		0
	};

	if (!Parser_parse_parameter_list(context, &constructor_output))
		return false;

	// Add the newly build scope
	if (!Scope_setup(scope, context->window_manager))
		return false;

	Scope_add_scope(context->scope, scope);

	// Job done
	return true;
}


static bool
Parser_parse_instanciation(
	ParseContext* context,
	const StringList* left_path
) {
	bool ret = true;

	// Parse an identifier path
	StringList right_path;
	StringList_init(&right_path);

	// Check that the identifier is not taken already
	if (!Parser_parse_identifier_path(context, &right_path)) {
		ret = false;
		goto termination;
	}

	// Check that the name is not taken already
	if (Scope_get_member(
		context->scope,
		StringList_items(left_path),
		StringList_length(left_path)
	)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : path to an already defined item\n",
			context->lexer->token.location.line + 1
		);
		ret = false;
		goto termination;
	}
	
	// Evaluate the path
	ScopeMember* member = 
		Scope_get_member(
			context->scope,
			StringList_items(&right_path),
			StringList_length(&right_path)
		);

	if (!member) { // TODO : invalid path should be printed
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : invalid path\n",
			context->lexer->token.location.line + 1
		);
		ret = false;
		goto termination;
	}
	
	// Check that the path leads to a builder
	switch(member->type) {
		case ScopeMemberType__node_delegate:
			ret = Parser_parse_node_delegate_instanciation(
				context,
				StringList_at(left_path, 0),
				member
			);
			break;

		case ScopeMemberType__scope_delegate:
			ret = Parser_parse_scope_delegate_instanciation(
				context,
				StringList_at(left_path, 0),
				member
			);
			break;

		default:
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"line %d : path is not a node, a node delegate or a scope delegate\n",
				context->lexer->token.location.line + 1
			);
			ret = false;
	}

termination:
	// Free ressources
	StringList_destroy(&right_path);

	// Job done
	return ret;
}


static bool
Parser_parse_input_assignment(
	ParseContext* context,
	const StringList* left_path
) {
	bool ret = true;

	// Parse an identifier path
	StringList right_path;
	StringList_init(&right_path);

	if (!Parser_parse_identifier_path(context, &right_path)) {
		ret = false;
		goto termination;
	}

	// Retrieve destination node
	Node* dst_node = 
		Parser_get_node(
			context,
			StringList_items(left_path),
			StringList_length(left_path) - 1
		);

	if (!dst_node) {
		ret = false;
		goto termination;
	}

	// Retrieve source node
	Node* src_node = 
		Parser_get_node(
			context,
			StringList_items(&right_path),
			StringList_length(&right_path)
		);

	if (!src_node) {
		ret = false;
		goto termination;
	}

	// Assign the input to the dst node
	const String* dst_param_name =
		StringList_at(
			left_path,
			StringList_length(left_path) - 1
		);

	if (!Node_set_input_by_name(
		dst_node,
		dst_param_name,
		src_node)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node '%s' does not have an input '%s' with matching type\n",
			context->lexer->token.location.line + 1,
			dst_node->name.data,
			dst_param_name->data
		);
		ret = false;
		goto termination;
	}

termination:
	// Free ressources
	StringList_destroy(&right_path);

	// Job done
	return ret;
}


static bool
Parser_parse_statement(
	ParseContext* context
) {
	int ret = true;

	// Parse an identifier path
	StringList left_path;
	StringList_init(&left_path);

	if (!Parser_parse_identifier_path(context, &left_path)) {
		ret = false;
		goto termination;
	}

	// Parse '='
	if (context->lexer->token.type != TokenType__equal)
		handle_processing_error(
			&(context->lexer->token.location),
			"expected '=', got '%s' instead",
			context->lexer->token.text
		);
	 Lexer_next_token(context->lexer);

	// Determine the type of statement
	if (StringList_length(&left_path) == 1) {
		if (!Parser_parse_instanciation(context, &left_path))
			ret = false;

		Lexer_next_token(context->lexer);
	}
	// Node input assignment
	else {
		if (!Parser_parse_input_assignment(context, &left_path))
			ret = false;
	}

termination:
	// Free ressources
	StringList_destroy(&left_path);

	// Job done
	return ret;
}


static bool
Parser_parse_statement_list(
	ParseContext* context
) {
	Lexer_next_token(context->lexer);

	int error_count = 0;
	for(bool done = false; (!done) && (error_count < MAX_PARSING_ERROR_COUNT); ) {
		switch(context->lexer->token.type) {
			case TokenType__eof:
			case TokenType__invalid:
				done = true;
				continue;

			default:
				if (!Parser_parse_statement(context))
					error_count += 1;
		}
	}

	if (context->lexer->token.type == TokenType__invalid)
		handle_processing_error(
			&(context->lexer->token.location),
			"invalid token '%s'",
			context->lexer->token.text
		);

	// Job done
	return error_count == 0;
}


// --- Main entry point -------------------------------------------------------

bool
Parser_parse(
	Lexer* lexer,
	Scope* scope,
	WindowManager* window_manager
) {
	ParseContext context;
	ParseContext_init(&context, lexer, scope, window_manager);

	bool ret = Parser_parse_statement_list(&context);

	#ifdef DEBUG
	ParseContext_destroy(&context);
	#endif

	return ret;
}
