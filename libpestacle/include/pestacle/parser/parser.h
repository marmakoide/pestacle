#ifndef PESTACLE_PARSER_PARSER2_H
#define PESTACLE_PARSER_PARSER2_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pestacle/parser/AST.h>
#include <pestacle/parser/lexer.h>


extern AST_Unit*
parse(
	Lexer* lexer
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_PARSER_PARSER2_H */
