#include <stdlib.h>
#include <assert.h>
#include "errors.h"
#include "memory.h"
#include "string_list.h"
#include "parser/parser.h"


// --- ParserContext implementation ------------------------------------------

typedef struct {
	Lexer* lexer;
	Domain* domain;
	Graph* graph;
	Dict entity_dict;
} ParseContext;


static void
ParseContext_init(
	ParseContext* self,
	Lexer* lexer,
	Domain* domain,
	Graph* graph
) {
	assert(self != 0);
	assert(lexer != 0);
	assert(domain != 0);
	assert(graph != 0);

	self->lexer = lexer;
	self->domain = domain;
	self->graph = graph;

	Dict_init(&(self->entity_dict));
}


static void
ParseContext_destroy(
	ParseContext* self
) {
	assert(self != 0);

	#ifdef DEBUG
	self->lexer = 0;
	self->domain = 0;
	self->graph = 0;
	#endif

	Dict_destroy(&(self->entity_dict));
}


/*
static void
ParseContext_add_entity(
	ParseContext* self,
	const String* name,
	const StringList* entity_path
) {
	assert(self != 0);
	assert(name != 0);
	assert(entity_path != 0);
}
*/


// --- Parser utilities -------------------------------------------------------

static Node*
Parser_get_node(
	ParseContext* context,
	const String* name_str
) {
	Node* node = Graph_get_node(context->graph, name_str);
	if (!node)
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node '%s' is not defined\n",
			context->lexer->token.location.line + 1,
			name_str->data
		);

	return node;
}


// --- Parser recursive descent implementation --------------------------------

#define MAX_PARSING_ERROR_COUNT 16


bool
Parser_parse_parameter(
	ParseContext* context,
	Node* node
) {
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
		case ParameterType__integer:
			if (context->lexer->token.type != TokenType__integer) {
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %d : expected an integer value, got '%s' instead\n",
					context->lexer->token.location.line + 1,
					context->lexer->token.text_data
				);
				ret = false;
				goto termination;
			}		
			param_value->int64_value = context->lexer->token.value.int64_value;
			break;
		case ParameterType__real:
			if ((context->lexer->token.type != TokenType__integer) && (context->lexer->token.type != TokenType__real)) {
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %d : expected a numeric value, got '%s' instead",
					context->lexer->token.location.line + 1,
					context->lexer->token.text_data
				);
				ret = false;
				goto termination;
			}
			param_value->real_value = context->lexer->token.value.real_value;
			break;
		case ParameterType__string:
			if (context->lexer->token.type != TokenType__string) {
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %d : expected a string constant, got '%s' instead",
					context->lexer->token.location.line + 1,
					context->lexer->token.text_data
				);
				ret = false;
				goto termination;
			}
			
			String_destroy(&(param_value->string_value));
			String_clone(&(param_value->string_value), Lexer_token_text(context->lexer));
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
	Node* node
) {
	bool ret = true;

	// Parse '('
	Lexer_next_token(context->lexer);
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
				
				if (!Parser_parse_parameter(context, node))
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
Parser_parse_node_creation(
	ParseContext* context,
	const String* name_str
) {
	// Parse an identifier
	if (context->lexer->token.type != TokenType__identifier)
		handle_processing_error(
			&(context->lexer->token.location),
			"expected an identifier, got '%s' instead",
			context->lexer->token.text
		);

	const String* type_str = Lexer_token_text(context->lexer);

	// Check that the node delegate exists
	const DomainMember* member = 
		Domain_get_member_by_name(context->domain, type_str);

	if (!member) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node delegate '%s' is not defined\n",
			context->lexer->token.location.line + 1,
			type_str->data
		);
		return false;
	}

	if (member->type != DomainMemberType__node_delegate) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : '%s' is not defined as a node delegate\n",
			context->lexer->token.location.line + 1,
			type_str->data
		);
		return false;
	}

	const NodeDelegate* delegate = member->node_delegate;

	// Create the node instance
	Node* node = Graph_add_node(context->graph, name_str, delegate);
	if (!node) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node '%s' is already defined\n",
			context->lexer->token.location.line + 1,
			name_str->data
		);
		return false;
	}

	// Parse parameters
	if (!Parser_parse_parameter_list(context, node))
		return false;

	// Job done
	return true;
}


static bool
Parser_parse_input_assignment(
	ParseContext* context,
	const StringList* left_identifier_list
) {
	bool ret = true;

	// Parse an identifier path
	StringList right_identifier_list;
	StringList_init(&right_identifier_list);

	if (!Parser_parse_identifier_path(context, &right_identifier_list)) {
		ret = false;
		goto termination;
	}

	// Assign the input to the dst node
	const String* src_name = StringList_at(&right_identifier_list, 0);
	Node* src_node = Parser_get_node(context, src_name);
	if (!src_node) {
		ret = false;
		goto termination;
	}

	const String* dst_name = StringList_at(left_identifier_list, 0);
	Node* dst_node = Parser_get_node(context, dst_name);
	if (!dst_node) {
		ret = false;
		goto termination;
	}

	const String* dst_input_name = StringList_at(left_identifier_list, 1);

	if (!Node_set_input_by_name(
		dst_node,
		dst_input_name,
		src_node)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node '%s' does not have an input '%s' with matching type\n",
			context->lexer->token.location.line + 1,
			dst_name->data,
			dst_input_name->data
		);
		ret = false;
		goto termination;
	}

termination:
	// Free ressources
	StringList_destroy(&right_identifier_list);

	// Job done
	return ret;
}


static bool
Parser_parse_statement(
	ParseContext* context
) {
	int ret = true;

	// Parse an identifier path
	StringList left_identifier_list;
	StringList_init(&left_identifier_list);

	if (!Parser_parse_identifier_path(context, &left_identifier_list)) {
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

	// Node creation statement
	if (StringList_length(&left_identifier_list) == 1) {
		if (!Parser_parse_node_creation(context, StringList_at(&left_identifier_list, 0)))
			ret = false;

		Lexer_next_token(context->lexer);
	}
	// Node input assignment
	else {
		if (!Parser_parse_input_assignment(context, &left_identifier_list))
			ret = false;
	}

termination:
	// Free ressources
	StringList_destroy(&left_identifier_list);

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
	Domain* domain,
	Graph* graph
) {
	ParseContext context;
	ParseContext_init(&context, lexer, domain, graph);

	bool ret = Parser_parse_statement_list(&context);

	ParseContext_destroy(&context);
	return ret;
}
