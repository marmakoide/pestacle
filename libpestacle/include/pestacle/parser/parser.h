#ifndef PESTACLE_PARSER_PARSER_H
#define PESTACLE_PARSER_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pestacle/scope.h>
#include <pestacle/parser/lexer.h>


extern bool
Parser_parse(
	Lexer* lexer,
	Scope* scope
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_PARSER_PARSER_H */
