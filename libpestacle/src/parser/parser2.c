#include <assert.h>
#include <pestacle/errors.h>
#include <pestacle/parser/parser2.h>



// --- Recursive descent parsing ---------------------------------------------

#define MAX_PARSING_ERROR_COUNT 16


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
	StringList_append(&(path->str_list), Lexer_token_text(lexer));
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
		StringList_append(&(path->str_list), Lexer_token_text(lexer));
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
				&src,
				&dst
			);

		if (!parse_AST_node_instanciation(lexer, stat))
			ret = false;

		Lexer_next_token(context->lexer);
	}
	// Slot assignment
	else
		AST_Statement* stat =
			AST_Unit_append_slot_assignment(
				unit,
				&src,
				&dst
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
			case TokenType__eof:
			case TokenType__invalid:
				done = true;
				continue;

			default:
				if (!parse_AST_Statement(lexer, unit))
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


// --- Entry point -----------------------------------------------------------

AST_Unit*
parse(
	Lexer* lexer
) {
	assert(lexer);

	// Allocation
	AST_Unit* ret = (AST_Unit*)checked_malloc(sizeof(AST_Unit*));
	AST_Unit_init(ret);

	// Parsing
	if (!parse_AST_Unit(lexer, ret)) {
		AST_Unit_destroy(ret);
		ret = 0;
	}

	// Job done
	return ret;
}