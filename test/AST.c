#include <stdlib.h>

#include <SDL.h>
#include <pestacle/macros.h>
#include <pestacle/parser/parser.h>


int
main(
    ATTRIBUTE_UNUSED int argc,
    ATTRIBUTE_UNUSED char *argv[]
) {
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

    Lexer lexer;
    Lexer_init(&lexer, stdin);

    AST_Unit* unit = parse(&lexer);

    if (unit) {
        AST_Unit_print(unit, stdout);
        AST_Unit_destroy(unit);
        free(unit);
    }

    return EXIT_SUCCESS;
}