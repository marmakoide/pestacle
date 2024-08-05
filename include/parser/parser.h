#ifndef PESTACLE_PARSER_PARSER_H
#define PESTACLE_PARSER_PARSER_H

#include "graph.h"
#include "parser/lexer.h"


typedef struct {
	Graph* graph;
} ParseContext;


extern bool
Parser_parse(
	Lexer* lexer,
	Graph* graph
);


#endif /* PESTACLE_PARSER_PARSER_H */