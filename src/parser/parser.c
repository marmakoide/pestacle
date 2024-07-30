#include <stdlib.h>
#include <assert.h>
#include "errors.h"
#include "memory.h"
#include "parser/parser.h"


#define MAX_PARSING_ERROR_COUNT 16


static Node*
Parser_parse_get_node_instance(
	ParseContext* context,
	Lexer* lexer,
	const String* instance_name_str
) {
	Node* node = Graph_get_node_instance(
		context->graph,
		instance_name_str
	);

	if (!node)
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node instance '%s' have not been defined\n",
			lexer->token.location.line + 1,
			instance_name_str->data
		);

	return node;
}


static bool
Parser_parse_instance_creation(
	ParseContext* context,
	Lexer* lexer,
	const String* instance_name_str
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
	if (!Graph_add_node_instance(
			context->graph,
			instance_name_str,
			delegate
		)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node instance '%s' is already defined\n",
			lexer->token.location.line + 1,
			instance_name_str->data
		);
		return false;
	}

	// Job done
	return true;
}


static bool
Parser_parse_parameter_assignment(
	ParseContext* context,
	Lexer* lexer,
	const String* instance_name_str
) {
	bool ret = true;

	// Parse an identifier
	if (lexer->token.type != TokenType__identifier)
		handle_processing_error(
			&(lexer->token.location),
			"expected an identifier, got '%s' instead",
			lexer->token.text
		);

	String parameter_name_str;
	String_clone(&parameter_name_str, Lexer_token_text(lexer));

	// Parse '='
	Lexer_next_token(lexer);
	if (lexer->token.type != TokenType__equal)
		handle_processing_error(
			&(lexer->token.location),
			"expected '=', got '%s' instead",
			lexer->token.text
		);

	// Fetch the node instance
	Node* node = Parser_parse_get_node_instance(context, lexer, instance_name_str);
	if (!node) {
		ret = false;
		Lexer_next_token(lexer);
		goto termination;
	}

	// Fetch the parameter 
	NodeParameterValue* param_value = 0;
	NodeParameterDefinition const* param_def = 0;

	if (!Node_get_parameter_by_name(node, &parameter_name_str, &param_def, &param_value)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"line %d : node instance '%s' does not have a '%s' parameter\n",
			lexer->token.location.line + 1,
			instance_name_str->data,
			parameter_name_str.data
		);
		ret = false;
		goto termination;
	}

	// Assign the parameter of the instance
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
	String_destroy(&parameter_name_str);

	// Job done
	return ret;
}


static bool
Parser_parse_input_assignment(
	ParseContext* context,
	Lexer* lexer,
	const String* src_instance_name_str
) {
	bool ret = true;

	// Parse an identifier
	if (lexer->token.type != TokenType__identifier)
		handle_processing_error(
			&(lexer->token.location),
			"expected an identifier, got '%s' instead",
			lexer->token.text
		);

	String dst_instance_name_str;
	String_clone(&dst_instance_name_str, Lexer_token_text(lexer));

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

	// Assign the input to the instance
	Node* src_node = Parser_parse_get_node_instance(context, lexer, src_instance_name_str);
	if (!src_node) {
		ret = false;
		goto termination;
	}

	Node* dst_node = Parser_parse_get_node_instance(context, lexer, &dst_instance_name_str);
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
			"line %d : node instance '%s' does not have an input '%s' with matching type\n",
			lexer->token.location.line + 1,
			dst_instance_name_str.data,
			input_name_str.data
		);
		ret = false;
		goto termination;
	}

termination:
	// Free ressources
	String_destroy(&dst_instance_name_str);
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
		case TokenType__colon:
			Lexer_next_token(lexer);
			if (!Parser_parse_instance_creation(context, lexer, &identifier_str))
				ret = false;
			break;

		case TokenType__dot:
			Lexer_next_token(lexer);
			if (!Parser_parse_parameter_assignment(context, lexer, &identifier_str))
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
				"expected '.', ':' or '->' got '%s' instead",
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
