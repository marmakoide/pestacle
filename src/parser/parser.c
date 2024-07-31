#include <stdlib.h>
#include <assert.h>
#include "errors.h"
#include "memory.h"
#include "parser/parser.h"


#define MAX_PARSING_ERROR_COUNT 16


static Node*
Parser_get_node(
	ParseContext* context,
	Lexer* lexer,
	const String* name_str
) {
	Node* node = Graph_get_node(context->graph, name_str);
	if (!node)
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node '%s' is not defined\n",
			lexer->token.location.line + 1,
			name_str->data
		);

	return node;
}


bool
Parser_parse_parameter(
	ParseContext* context,
	Lexer* lexer,
	Node* node
) {
	bool ret = true;

	// Parse an identifier
	if (lexer->token.type != TokenType__identifier)
		handle_processing_error(
			&(lexer->token.location),
			"expected an identifier, got '%s' instead",
			lexer->token.text
		);

	String name_str;
	String_clone(&name_str, Lexer_token_text(lexer));

	// Fetch the parameter 
	NodeParameterValue* param_value = 0;
	NodeParameterDefinition const* param_def = 0;

	if (!Node_get_parameter_by_name(node, &name_str, &param_def, &param_value)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node '%s' does not have a '%s' parameter\n",
			lexer->token.location.line + 1,
			node->name.data,
			name_str.data
		);
		ret = false;
		goto termination;
	}

	// Parse '='
	Lexer_next_token(lexer);
	if (lexer->token.type != TokenType__equal)
		handle_processing_error(
			&(lexer->token.location),
			"expected '=', got '%s' instead",
			lexer->token.text
		);

	// Assign the value to the node parameter
	Lexer_next_token(lexer);
	switch(param_def->type) {
		case NodeParameterType__integer:
			if (lexer->token.type != TokenType__integer) {
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %d : expected an integer value, got '%s' instead\n",
					lexer->token.location.line + 1,
					lexer->token.text_data
				);
				ret = false;
				goto termination;
			}		
			param_value->int64_value = lexer->token.value.int64_value;
			break;
		case NodeParameterType__real:
			if ((lexer->token.type != TokenType__integer) && (lexer->token.type != TokenType__real)) {
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %d : expected a numeric value, got '%s' instead",
					lexer->token.location.line + 1,
					lexer->token.text_data
				);
				ret = false;
				goto termination;
			}
			param_value->real_value = lexer->token.value.real_value;
			break;
		case NodeParameterType__string:
			if (lexer->token.type != TokenType__string) {
				SDL_LogError(
					SDL_LOG_CATEGORY_SYSTEM,
					"line %d : expected a string constant, got '%s' instead",
					lexer->token.location.line + 1,
					lexer->token.text_data
				);
				ret = false;
				goto termination;
			}
			
			String_destroy(&(param_value->string_value));
			String_clone(&(param_value->string_value), Lexer_token_text(lexer));
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
	Lexer* lexer,
	Node* node
) {
	bool ret = true;

	// Parse '('
	Lexer_next_token(lexer);
	if (lexer->token.type != TokenType__pth_open)
		handle_processing_error(
			&(lexer->token.location),
			"expected '(', got '%s' instead",
			lexer->token.text
		);

	// Parse each parameter one by one
	bool first = true;
	for(bool done = false; !done; first = false) {
		Lexer_next_token(lexer);
		switch(lexer->token.type) {
			case TokenType__pth_close:
				done = true;
				break;

			case TokenType__eof:
				done = true;
				ret = false;
				break;

			default:
				if (!first) {
					if (lexer->token.type != TokenType__comma)
						handle_processing_error(
							&(lexer->token.location),
							"expected ')', got '%s' instead",
							lexer->token.text
						);
					Lexer_next_token(lexer);
				}
				
				if (!Parser_parse_parameter(context, lexer, node))
					ret = false;
		}
	}
	
	// Parse ')'
	if (lexer->token.type != TokenType__pth_close)
		handle_processing_error(
			&(lexer->token.location),
			"expected ')', got '%s' instead",
			lexer->token.text
		);

	// Job done
	return ret;
}


static bool
Parser_parse_node_creation(
	ParseContext* context,
	Lexer* lexer,
	const String* name_str
) {
	// Parse an identifier
	if (lexer->token.type != TokenType__identifier)
		handle_processing_error(
			&(lexer->token.location),
			"expected an identifier, got '%s' instead",
			lexer->token.text
		);

	const String* type_str = Lexer_token_text(lexer);

	// Check that the node type exists
	const NodeDelegate* delegate = get_node_delegate_by_name(type_str);
	if (!delegate) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node type '%s' is not defined\n",
			lexer->token.location.line + 1,
			type_str->data
		);
		return false;
	}

	// Create the node instance
	Node* node = Graph_add_node(context->graph, name_str, delegate);
	if (!node) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node '%s' is already defined\n",
			lexer->token.location.line + 1,
			name_str->data
		);
		return false;
	}

	// Parse parameters
	if (!Parser_parse_parameter_list(context, lexer, node))
		return false;

	// Job done
	return true;
}


static bool
Parser_parse_input_assignment(
	ParseContext* context,
	Lexer* lexer,
	const String* src_name_str
) {
	bool ret = true;

	// Parse an identifier
	if (lexer->token.type != TokenType__identifier)
		handle_processing_error(
			&(lexer->token.location),
			"expected an identifier, got '%s' instead",
			lexer->token.text
		);

	String dst_name_str;
	String_clone(&dst_name_str, Lexer_token_text(lexer));

	// Parse '.'
	Lexer_next_token(lexer);
	if (lexer->token.type != TokenType__dot)
		handle_processing_error(
			&(lexer->token.location),
			"expected '.', got '%s' instead",
			lexer->token.text
		);

	// Parse an identifier
	Lexer_next_token(lexer);
	if (lexer->token.type != TokenType__identifier)
		handle_processing_error(
			&(lexer->token.location),
			"expected an identifier, got '%s' instead",
			lexer->token.text
		);

	String input_name_str;
	String_clone(&input_name_str, Lexer_token_text(lexer));

	// Assign the input to the dst node
	Node* src_node = Parser_get_node(context, lexer, src_name_str);
	if (!src_node) {
		ret = false;
		goto termination;
	}

	Node* dst_node = Parser_get_node(context, lexer, &dst_name_str);
	if (!dst_node) {
		ret = false;
		goto termination;
	}

	if (!Node_set_input_by_name(
		dst_node,
		&input_name_str,
		src_node)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node '%s' does not have an input '%s' with matching type\n",
			lexer->token.location.line + 1,
			dst_name_str.data,
			input_name_str.data
		);
		ret = false;
		goto termination;
	}

termination:
	// Free ressources
	String_destroy(&dst_name_str);
	String_destroy(&input_name_str);

	// Job done
	return ret;
}


static bool
Parser_parse_declaration(
	ParseContext* context, 
	Lexer* lexer
) {
	int ret = true;

	// Parse an identifier
	if (lexer->token.type != TokenType__identifier)
		handle_processing_error(
			&(lexer->token.location),
			"expected an identifier, got '%s' instead",
			lexer->token.text
		);

	String identifier_str;
	String_clone(&identifier_str, Lexer_token_text(lexer));

	// Call the proper parsing routine
    Lexer_next_token(lexer);
	switch(lexer->token.type) {
		case TokenType__equal:
			Lexer_next_token(lexer);
			if (!Parser_parse_node_creation(context, lexer, &identifier_str))
				ret = false;
			break;

		case TokenType__left_arrow:
			Lexer_next_token(lexer);
			if (!Parser_parse_input_assignment(context, lexer, &identifier_str))
				ret = false;
			break;

		default:
			handle_processing_error(
				&(lexer->token.location),
				"expected '=' or '->' got '%s' instead",
				lexer->token.text
			);
	}

	// Free ressources
	String_destroy(&identifier_str);

	// Job done
	return ret;
}


static bool
Parser_parse_declaration_list(
	ParseContext* context,
	Lexer* lexer
) {
	int error_count = 0;
	for(bool done = false; (!done) && (error_count < MAX_PARSING_ERROR_COUNT); ) {
		Lexer_next_token(lexer);
		switch(lexer->token.type) {
			case TokenType__eof:
			case TokenType__invalid:
				done = true;
				continue;

			default:
				if (!Parser_parse_declaration(context, lexer))
					error_count += 1;
		}
	}

	if (lexer->token.type == TokenType__invalid)
		handle_processing_error(
			&(lexer->token.location),
			"invalid token '%s'",
			lexer->token.text
		);

	// Job done
	return error_count == 0;
}


// --- Main entry point -------------------------------------------------------

bool
Parser_parse(
	Lexer* lexer,
	Graph* graph
) {
	ParseContext context;
	context.graph = graph;
	return Parser_parse_declaration_list(&context, lexer);
}
