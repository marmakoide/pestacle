#ifndef PESTACLE_PARSER_LEXER_H
#define PESTACLE_PARSER_LEXER_H

/******************************************************************************
  Return tokens from an input file
 *****************************************************************************/


#include <stdint.h>
#include "array_ops.h"
#include "strings.h"
#include "input_buffer.h"
#include "file_location.h"


enum TokenType {
	TokenType__invalid = 0, // debugging help
	TokenType__identifier,  // identifier
	TokenType__bool,        // boolean constant
	TokenType__integer,     // integer constant
	TokenType__real,        // real constant
	TokenType__string,      // string constant
	TokenType__comma,       // ,
	TokenType__pth_open,    // (
	TokenType__pth_close,   // )
	TokenType__dot,         // .
	TokenType__equal,       // =
	TokenType__eof          // end of file
}; // enum TokenType


#define LEXER_TOKEN_TEXT_MAX_SIZE 1024

typedef union {
	bool bool_value;
	int64_t int64_value;
	real_t real_value;
} TokenValue;


typedef struct {
	enum TokenType type;
	FileLocation location;
	char text_data[LEXER_TOKEN_TEXT_MAX_SIZE];
	char* text_end;
	String text;
	TokenValue value;
} TokenData;


typedef struct {
	InputBuffer input_buffer;
	TokenData token;
	FileLocation location;
} Lexer;


extern void
Lexer_next_token(
	Lexer* self
);


extern const String*
Lexer_token_text(
	Lexer* self
);


extern void
Lexer_init(
	Lexer* self,
	FILE* input_file
);


#ifdef DEBUG
extern void
Lexer_test();
#endif // DEBUG


#endif /* PESTACLE_PARSER_LEXER_H */
