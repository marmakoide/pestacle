#ifndef PESTACLE_PARSER_PARSER_H
#define PESTACLE_PARSER_PARSER_H

#include <pestacle/scope.h>
#include <pestacle/parser/lexer.h>


extern bool
Parser_parse(
	Lexer* lexer,
	Scope* scope
);


#endif /* PESTACLE_PARSER_PARSER_H */
