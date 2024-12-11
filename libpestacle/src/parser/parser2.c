#include <assert.h>
#include <stdlib.h>
#include <SDL_log.h>
#include <pestacle/memory.h>
#include <pestacle/errors.h>
#include <pestacle/strings.h>
#include <pestacle/parser/parser2.h>



// --- Error reporting -------------------------------------------------------

void 
handle_parsing_error(
	const FileLocation* location,
	const char* format,
	...
) {
	SDL_LogError(
		SDL_LOG_CATEGORY_SYSTEM,
		"line %d ", location->line + 1	
	);
	
	va_list args;
	va_start(args, format);
	SDL_LogMessageV(
		SDL_LOG_CATEGORY_SYSTEM,
		SDL_LOG_PRIORITY_ERROR,
		format,
		args
	);
	va_end(args);
}


// --- Recursive descent parsing ---------------------------------------------

#define MAX_PARSING_ERROR_COUNT 16


static bool
parse_AST_node_instanciation_parameter(
	Lexer* lexer,
	AST_Statement* stat
) {
	assert(lexer);
	assert(stat);
	assert(stat->type == AST_StatementType__node_instanciation);

	bool ret = true;

	// Parse an identifier
	if (lexer->token.type != TokenType__identifier)
		handle_processing_error(
			&(lexer->token.location),
			"expected an identifier, got '%s' instead",
			lexer->token.text
		);

	// Create parameter
	AST_Parameter* parameter =
		(AST_Parameter*)checked_malloc(sizeof(AST_Parameter));

	AST_Parameter_init(parameter, Lexer_token_text(lexer));
	parameter->location = lexer->token.location;

	// Parse '='
	Lexer_next_token(lexer);
	if (lexer->token.type != TokenType__equal)
		handle_processing_error(
			&(lexer->token.location),
			"expected '=', got '%s' instead",
			lexer->token.text
		);

	// Parse atomic value
	Lexer_next_token(lexer);
	switch(lexer->token.type) {
		case TokenType__bool:
			AST_AtomicValue_init_bool(
				&(parameter->value),
				lexer->token.value.bool_value
			);
			break;

		case TokenType__integer:
			AST_AtomicValue_init_int64(
				&(parameter->value),
				lexer->token.value.int64_value
			);
			break;

		case TokenType__real:
			AST_AtomicValue_init_real(
				&(parameter->value),
				lexer->token.value.real_value
			);
			break;

		case TokenType__string:
			AST_AtomicValue_init_string(
				&(parameter->value),
				Lexer_token_text(lexer)
			);
			break;

		default:
			handle_processing_error(
				&(lexer->token.location),
				"expected a parameter value, got '%s' instead",
				lexer->token.text
			);

			AST_Parameter_destroy(parameter);
			free(parameter);
			parameter = 0;
			break;
	}

	if (parameter)
		parameter->value.location = lexer->token.location;

	// Append the parameter
	if (!AST_NodeInstanciation_add_parameter(&(stat->node_instanciation), parameter)) {
		handle_processing_error(
			&(parameter->location),
			"parameter '%s' assigned more than once",
			parameter->name
		);
		AST_Parameter_destroy(parameter);
		free(parameter);
		parameter = 0;
	}

	// Job done
	return ret;
}


static bool
parse_AST_node_instanciation(
	Lexer* lexer,
	AST_Statement* stat
) {
	assert(lexer);
	assert(stat);
	assert(stat->type == AST_StatementType__node_instanciation);

	bool ret = true;

	// Parse '('
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

				if (!parse_AST_node_instanciation_parameter(lexer, stat))
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
parse_AST_Path(
	Lexer* lexer,
	AST_Path* path
) {
	bool ret = true;

	// Set the location
	path->location = lexer->token.location;

	// Parse first identifier
	if (lexer->token.type != TokenType__identifier) {
		handle_processing_error(
			&(lexer->token.location),
			"expected an identifier, got '%s' instead",
			lexer->token.text
		);	
	}
	StringList_append(&(path->string_list), Lexer_token_text(lexer));
	Lexer_next_token(lexer);

	// Parse the following identifiers
	while(lexer->token.type == TokenType__dot) {
		Lexer_next_token(lexer);
		if (lexer->token.type != TokenType__identifier) {
			handle_processing_error(
				&(lexer->token.location),
				"expected an identifier, got '%s' instead",
				lexer->token.text
			);
		}
		StringList_append(&(path->string_list), Lexer_token_text(lexer));
		Lexer_next_token(lexer);
	}
	
	return ret;
}


static bool
parse_AST_Statement(
	Lexer* lexer,
	AST_Unit* unit
) {
	assert(lexer);
	assert(unit);

	int ret = true;

	// Allocation
	AST_Path src_path;
	AST_Path_init(&src_path);

	AST_Path dst_path;
	AST_Path_init(&dst_path);

	// Parse destination path
	if (!parse_AST_Path(lexer, &dst_path)) {
		ret = false;
		goto termination;
	}

	// Parse '='
	if (lexer->token.type != TokenType__equal)
		handle_processing_error(
			&(lexer->token.location),
			"expected '=', got '%s' instead",
			lexer->token.text
		);
	 Lexer_next_token(lexer);

	// Parse source path
	if (!parse_AST_Path(lexer, &src_path)) {
		ret = false;
		goto termination;
	}

	// Determine the type of statement
	if (lexer->token.type == TokenType__pth_open) {
		AST_Statement* stat =
			AST_Unit_append_node_instanciation(
				unit,
				&src_path,
				&dst_path
			);

		if (!parse_AST_node_instanciation(lexer, stat))
			ret = false;

		Lexer_next_token(lexer);
	}
	// Slot assignment
	else
		AST_Unit_append_slot_assignment(
			unit,
			&src_path,
			&dst_path
		);

termination:
	// Free ressources
	AST_Path_destroy(&src_path);
	AST_Path_destroy(&dst_path);

	// Job done
	return ret;	
}


static bool
parse_AST_Unit(
	Lexer* lexer,
	AST_Unit* unit
) {
	assert(lexer);
	assert(unit);

	// Parse each statement
	Lexer_next_token(lexer);

	int error_count = 0;
	for(bool done = false; (!done) && (error_count < MAX_PARSING_ERROR_COUNT); ) {
		switch(lexer->token.type) {
			// EOF => end of parsing
			case TokenType__eof:
				done = true;
				break;

			// Invalid token => signal the error and move on
			case TokenType__invalid:
				handle_parsing_error(
					&(lexer->token.location),
					"unexpected '%s'",
					lexer->token.text
				);
				error_count += 1;
				Lexer_next_token(lexer);
				break;

			// Assume we are about to parse a statement
			default:
				if (!parse_AST_Statement(lexer, unit))
					error_count += 1;
		}
	}

	// Job done
	return error_count == 0;
}


// --- Entry point -----------------------------------------------------------

AST_Unit*
parse(
	Lexer* lexer
) {
	assert(lexer);

	// Allocation
	AST_Unit* ret = (AST_Unit*)checked_malloc(sizeof(AST_Unit));
	AST_Unit_init(ret);

	// Parsing
	if (!parse_AST_Unit(lexer, ret)) {
		AST_Unit_destroy(ret);
		ret = 0;
	}

	// Job done
	return ret;
}