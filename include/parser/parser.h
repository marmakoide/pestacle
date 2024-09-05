#ifndef PESTACLE_PARSER_PARSER_H
#define PESTACLE_PARSER_PARSER_H

#include "graph.h"
#include "domain.h"
#include "parser/lexer.h"


typedef struct {
	Domain* domain;
	Graph* graph;
} ParseContext;


extern bool
Parser_parse(
	Lexer* lexer,
	Domain* domain,
	Graph* graph
);


#endif /* PESTACLE_PARSER_PARSER_H */