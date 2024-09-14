#ifndef PESTACLE_PARSER_PARSER_H
#define PESTACLE_PARSER_PARSER_H

#include "domain.h"
#include "window_manager.h"
#include "parser/lexer.h"


extern bool
Parser_parse(
	Lexer* lexer,
	Domain* domain,
	WindowManager* window_manager
);


#endif /* PESTACLE_PARSER_PARSER_H */