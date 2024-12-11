#include <stdlib.h>

#include <pestacle/macros.h>
#include <pestacle/parser/parser2.h>


int
main(ATTRIBUTE_UNUSED int argc, ATTRIBUTE_UNUSED char *argv[]) {
    Lexer lexer;
    Lexer_init(&lexer, stdin);

    AST_Unit* unit = parse(&lexer);

    if (unit) {
        AST_Unit_print(unit, stdout);
        //AST_Unit_destroy(unit);
        //free(unit);
    }

    return EXIT_SUCCESS;
}