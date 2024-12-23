#ifndef PESTACLE_PARSER_SCOPE_POPULATE_H
#define PESTACLE_PARSER_SCOPE_POPULATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pestacle/scope.h>
#include <pestacle/parser/AST.h>


extern bool
Scope_populate_from_AST(
	Scope* self,
	AST_Unit* unit
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_PARSER_SCOPE_POPULATE_H */
