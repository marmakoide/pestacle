#ifndef PESTACLE_PARSER_PARSER_H
#define PESTACLE_PARSER_PARSER_H

#include "scope.h"
#include "window_manager.h"
#include "parser/lexer.h"


extern bool
Parser_parse(
	Lexer* lexer,
	Scope* scope,
	WindowManager* window_manager
);


#endif /* PESTACLE_PARSER_PARSER_H */