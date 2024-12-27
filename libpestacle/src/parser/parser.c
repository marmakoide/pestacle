#include <assert.h>
#include <stdlib.h>
#include <SDL_log.h>
#include <pestacle/memory.h>
#include <pestacle/errors.h>
#include <pestacle/strings.h>
#include <pestacle/parser/parser.h>


// --- Recursive descent parsing ---------------------------------------------

#define MAX_PARSING_ERROR_COUNT 16


static int
parse_token(
	Lexer* lexer,
	enum TokenType token_type
) {
	assert(lexer);

	int error_count = 0;

	while (lexer->token.type == TokenType__error) {
		error_count += 1;
		handle_parsing_error(
			&(lexer->token.location),
			"expected %s, got '%s' instead",
			TokenType_get_description(token_type),
			lexer->token.text
		);
		Lexer_next_token(lexer);
	}

	if (lexer->token.type == token_type) 
		Lexer_next_token(lexer);
	else {
		error_count += 1;
		handle_parsing_error(
			&(lexer->token.location),
			"expected %s, got '%s' instead",
			TokenType_get_description(token_type),
			lexer->token.text
		);
	}

	return error_count;
}


static int
parse_AST_instanciation_parameter(
	Lexer* lexer,
	AST_Statement* stat
) {
	assert(lexer);
	assert(stat);
	assert(stat->type == AST_StatementType__instanciation);

	int error_count = 0;

	// Create parameter
	AST_Parameter* parameter =
		(AST_Parameter*)checked_malloc(sizeof(AST_Parameter));

	// Parse an identifier
	if (lexer->token.type == TokenType__identifier) {
		AST_Parameter_init(parameter, Lexer_token_text(lexer));
	}
	else {
		error_count += 1;
		handle_parsing_error(
			&(lexer->token.location),
			"expected an identifier, got '%s' instead",
			lexer->token.text
		);
		AST_Parameter_init(parameter, "");
	}

	parameter->location = lexer->token.location;
	Lexer_next_token(lexer);

	// Parse '='
	error_count += parse_token(lexer, TokenType__equal);

	// Parse atomic value
	switch(lexer->token.type) {
		case TokenType__bool:
			AST_AtomicValue_init_bool(
				&(parameter->value),
				lexer->token.value.bool_value
			);
			Lexer_next_token(lexer);
			break;

		case TokenType__integer:
			AST_AtomicValue_init_int64(
				&(parameter->value),
				lexer->token.value.int64_value
			);
			Lexer_next_token(lexer);
			break;

		case TokenType__real:
			AST_AtomicValue_init_real(
				&(parameter->value),
				lexer->token.value.real_value
			);
			Lexer_next_token(lexer);
			break;

		case TokenType__string:
			AST_AtomicValue_init_string(
				&(parameter->value),
				Lexer_token_text(lexer)
			);
			Lexer_next_token(lexer);
			break;

		default:
			error_count += 1;
			if (lexer->token.type != TokenType__error)
				handle_parsing_error(
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
	if (parameter && (!AST_Instanciation_add_parameter(&(stat->instanciation), parameter))) {
		error_count += 1;
		handle_parsing_error(
			&(parameter->location),
			"parameter '%s' assigned more than once",
			parameter->name
		);
		AST_Parameter_destroy(parameter);
		free(parameter);
		parameter = 0;
	}

	// Job done
	return error_count;
}


static int
parse_AST_instanciation(
	Lexer* lexer,
	AST_Statement* stat
) {
	assert(lexer);
	assert(stat);
	assert(stat->type == AST_StatementType__instanciation);

	int error_count = 0;

	// Parse '('
	error_count += parse_token(lexer, TokenType__pth_open);

	// Parse each parameter one by one
	bool first = true;
	for(bool done = false; !done; first = false) {
		switch(lexer->token.type) {
			case TokenType__pth_close:
				Lexer_next_token(lexer);
				done = true;
				break;

			case TokenType__eof:
				done = true;
				error_count += 1;
				break;

			default:
				// Parse ','
				if (!first)
					error_count += parse_token(lexer, TokenType__comma);
				
				// Parse parameter
				error_count += parse_AST_instanciation_parameter(lexer, stat);

		}
	}

	// Job done
	return error_count;
}


static int
parse_AST_Path(
	Lexer* lexer,
	AST_Path* path
) {
	int error_count = 0;

	// Set the location
	path->location = lexer->token.location;

	// Parse first identifier
	if (lexer->token.type == TokenType__identifier)
		StringList_append(&(path->string_list), Lexer_token_text(lexer));
	else {
		error_count += 1;
		handle_parsing_error(
			&(lexer->token.location),
			"expected an identifier, got '%s' instead",
			lexer->token.text
		);
	}
	Lexer_next_token(lexer);

	// Parse the following identifiers as long as '.' tokens are seen
	while(lexer->token.type == TokenType__dot) {
		Lexer_next_token(lexer);
		if (lexer->token.type == TokenType__identifier)
			StringList_append(&(path->string_list), Lexer_token_text(lexer));
		else {
			error_count += 1;
			handle_parsing_error(
				&(lexer->token.location),
				"expected an identifier, got '%s' instead",
				lexer->token.text
			);
			return false;
		}
		Lexer_next_token(lexer);
	}
	
	// Job done
	return error_count;
}


static int
parse_AST_Statement(
	Lexer* lexer,
	AST_Unit* unit
) {
	assert(lexer);
	assert(unit);

	int error_count = 0;

	// Allocation
	AST_Path src_path;
	AST_Path_init(&src_path);

	AST_Path dst_path;
	AST_Path_init(&dst_path);

	// Parse destination path
	error_count += parse_AST_Path(lexer, &dst_path);

	// Parse '='
	error_count += parse_token(lexer, TokenType__equal);

	// Parse source path
	error_count += parse_AST_Path(lexer, &src_path);

	// Determine the type of statement
	if (lexer->token.type == TokenType__pth_open) {
		AST_Statement* stat =
			AST_Unit_append_instanciation(
				unit,
				&src_path,
				&dst_path
			);

		error_count += parse_AST_instanciation(lexer, stat);
	}
	// Slot assignment
	else
		AST_Unit_append_slot_assignment(
			unit,
			&src_path,
			&dst_path
		);

	// Free ressources
	AST_Path_destroy(&src_path);
	AST_Path_destroy(&dst_path);

	// Job done
	return error_count;	
}


static int
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
			case TokenType__error:
			case TokenType__invalid:
				error_count += 1;
				handle_parsing_error(
					&(lexer->token.location),
					"unexpected '%s'",
					lexer->token.text
				);
				Lexer_next_token(lexer);
				break;

			// Assume we are about to parse a statement
			default:
				error_count += parse_AST_Statement(lexer, unit);
		}
	}

	// Job done
	return error_count;
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
	if (parse_AST_Unit(lexer, ret) != 0) {
		AST_Unit_destroy(ret);
		ret = 0;
	}

	// Job done
	return ret;
}