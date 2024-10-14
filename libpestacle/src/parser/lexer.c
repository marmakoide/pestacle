#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <pestacle/errors.h>
#include <pestacle/parser/lexer.h>


static const char*
str_true = "true";

static const size_t
str_true_len = 4;

static const char*
str_false = "false";

static const size_t
str_false_len = 5;


void
Lexer_init(
	Lexer* self,
	FILE* input_file
) {
	InputBuffer_init(&(self->input_buffer), input_file);

	self->location.line = 0;

	self->token.location.line = 0;
	self->token.type = TokenType__invalid;
	self->token.text = self->token.text_data;
	self->token.text_len = 0;
	self->token.value.int64_value = 0;

	// Initialize the token text buffer
	memset(self->token.text_data, 0, LEXER_TOKEN_TEXT_MAX_SIZE);
	self->token.text_end = self->token.text_data;
}


static size_t
Lexer_token_text_len(const Lexer* self) {
	return self->token.text_end - self->token.text_data + 1;
}


const char*
Lexer_token_text(Lexer* self) {
	self->token.text_len = Lexer_token_text_len(self);
	return self->token.text;
}


static void
Lexer_clear_token_text(Lexer* self) {
	self->token.text_end = self->token.text_data;
	*(self->token.text_data) = '\0';
}


static void
Lexer_accept(Lexer* self, InputBuffer_char c) {
    if (Lexer_token_text_len(self) == LEXER_TOKEN_TEXT_MAX_SIZE)
		handle_processing_error(
			&(self->location),
			"token '%s%c' too long (more than %d characters)",
			self->token.text_data,
			c,
			LEXER_TOKEN_TEXT_MAX_SIZE - 1
		);
    
	*(self->token.text_end) = (char)c;
	self->token.text_end += 1;
	*(self->token.text_end) = '\0';
}


static void
Lexer_skip_char(Lexer* self) {
	InputBuffer_next(&(self->input_buffer));
}


static void
Lexer_accept_char(Lexer* self) {
	Lexer_accept(self, InputBuffer_get(&(self->input_buffer)));
}


static void
Lexer_accept_and_next_char(Lexer* self) {
	Lexer_accept_char(self);
	InputBuffer_next(&(self->input_buffer));
}


static void
Lexer_parse_real(Lexer* self) {
	char* end_ptr = 0;
	self->token.value.real_value = strtof(self->token.text_data, &end_ptr);
}


static void
Lexer_parse_decimal_integer(Lexer* self) {
	uint32_t value = 0;

	for(const char* str = self->token.text_data; *str != '\0'; ++str) {
		if ((value >= 429496729) && (*str > '5'))
			handle_processing_error(
				&(self->location),
				"decimal integer %s is too large, does not fit in 32 bits",
				self->token.text
			);

		value *= 10;
		value += *str - '0';
	}

	self->token.value.int64_value = value;
}


static uint32_t
hex_char_to_digit[23] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15
}; // hex_char_to_digit[23]


static void
Lexer_parse_hexadecimal_integer(Lexer* self) {
	uint32_t value = 0; 

	for(const char* str = self->token.text_data + 2; *str != '\0'; ++str) {
		if ((value & 0x10000000) != 0)
			handle_processing_error(
				&(self->location),
				"hexadecimal integer %s is too large, does not fit in 32 bits",
				self->token.text
			);

		value <<= 4;
		value |= hex_char_to_digit[toupper(*str) - '0'];
	}

	self->token.value.int64_value = value;
}


enum LexerParsingState {
	LexerParsingState__init,
	LexerParsingState__identifier,
	LexerParsingState__slash,
	LexerParsingState__dot,
	LexerParsingState__string,
	LexerParsingState__string_escape,
	LexerParsingState__single_line_comment,
	LexerParsingState__multi_line_comment,
	LexerParsingState__multi_line_comment_end,
	LexerParsingState__zero_digit,
	LexerParsingState__decimal_integer,
	LexerParsingState__hexadecimal_integer,
	LexerParsingState__real,
}; // enum LexerParsingState


void
Lexer_next_token(Lexer* self) {
	enum LexerParsingState state = LexerParsingState__init;
	Lexer_clear_token_text(self);
	self->token.value.int64_value = 0;
	self->token.type = TokenType__invalid;
    
	while(self->token.type != TokenType__eof) {
		InputBuffer_char current_char = InputBuffer_get(&(self->input_buffer));
		switch(state) {
			case LexerParsingState__init:
				self->token.location = self->location;
				switch(current_char) {
					case EOF:
						Lexer_clear_token_text(self);
						self->token.type = TokenType__eof;
						return;
					case ' ':
					case '\t':
					case '\r':
						Lexer_skip_char(self);
						break;
					case '\n':
						self->location.line += 1;
						Lexer_skip_char(self);
						break;
					case '"':
						state = LexerParsingState__string;
						Lexer_skip_char(self);
						break;
					case ',':
						self->token.type = TokenType__comma;
						Lexer_accept_and_next_char(self);
						return;
					case '(':
						self->token.type = TokenType__pth_open;
						Lexer_accept_and_next_char(self);
						return;
					case ')':
						self->token.type = TokenType__pth_close;
						Lexer_accept_and_next_char(self);
						return;
					case '.':
						state = LexerParsingState__dot;
						Lexer_accept_and_next_char(self);
						break;
					case '/':
						state = LexerParsingState__slash;
						Lexer_skip_char(self);
						break;
					case '=':
						self->token.type = TokenType__equal;
						Lexer_accept_and_next_char(self);
						return;
					case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
					case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
					case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
					case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
					case 'Y': case 'Z':
					case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
					case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
					case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
					case 's': case 't': case 'u': case 'v': case 'w': case 'x':
					case 'y': case 'z':
						state = LexerParsingState__identifier;
						Lexer_accept_and_next_char(self);
						break;
					case '0':
						state = LexerParsingState__zero_digit;
						Lexer_accept_and_next_char(self);
					break;
					case '1': case '2': case '3': case '4':
					case '5': case '6': case '7': case '8': case '9':
						state = LexerParsingState__decimal_integer;
						Lexer_accept_and_next_char(self);
					break;
					default:
						Lexer_accept_char(self);
						return;
				}
				break;
				// case LexerParsingState__init
			case LexerParsingState__string:
				switch(current_char) {
					case '"':
						self->token.type = TokenType__string;
						Lexer_skip_char(self);
						return;
					case '\n':
						self->location.line += 1;
						Lexer_accept_and_next_char(self);
						break;
					case '\\':
						state = LexerParsingState__string_escape;
						Lexer_skip_char(self);
						break;
					case EOF:
						handle_processing_error(
							&(self->location),
							"unterminated string constant"
						);
						break;
					default:
						Lexer_accept_and_next_char(self);
				}
				break;
				// case LexerParsingState__string
			case LexerParsingState__string_escape:
				switch(current_char) {
					case 'n':
						Lexer_accept(self, '\n');
						break;
					case '\\':
						Lexer_accept(self, '\\');
						break;
					case '\"':
						Lexer_accept(self, '\"');
						break;
					default:
						handle_processing_error(
							&(self->location),
							"unsupported string escape sequence"
						);
						break;
				}
				Lexer_skip_char(self);
				state = LexerParsingState__string;
				break;
			case LexerParsingState__dot:
				if (isdigit(current_char)) {
					state = LexerParsingState__real;
					Lexer_accept_and_next_char(self);
				}
				else {
					self->token.type = TokenType__dot;
					return;
				}
				break;
				// case LexerParsingState__dot
			case LexerParsingState__slash:
				switch(current_char) {
					case '/':
						state = LexerParsingState__single_line_comment;
						Lexer_skip_char(self);
						break;
					case '*':
						state = LexerParsingState__multi_line_comment;
						Lexer_skip_char(self);
						break;
					default:
						return;
				}
				break;
				// case LexerParsingState__slash
			case LexerParsingState__single_line_comment:
				switch(current_char) {
					case '\n':
						state = LexerParsingState__init;
						self->location.line += 1;
						Lexer_skip_char(self);
						break;
					case EOF:
						state = LexerParsingState__init;
						break;
					default:
						Lexer_skip_char(self);
				}
				break;
				// case LexerParsingState__single_line_comment
			case LexerParsingState__multi_line_comment:
				switch(current_char) {
					case '*':
						state = LexerParsingState__multi_line_comment_end;
						Lexer_skip_char(self);
						break;
					case EOF:
						handle_processing_error(
							&(self->location),
							"unterminated multi-line comment"
						);
						break;
					case '\n':
						self->location.line += 1;
						Lexer_skip_char(self);
						break;
					default:
						Lexer_skip_char(self);
				}
				break;
				// case LexerParsingState__multi_line_comment
			case LexerParsingState__multi_line_comment_end:
				switch(current_char) {
					case '/':
						state = LexerParsingState__init;
						Lexer_skip_char(self);
						break;
					default:
						state = LexerParsingState__multi_line_comment;
				}
				break;
				// case LexerParsingState__multi_line_comment_end
			case LexerParsingState__identifier:
				switch(current_char) {
					case '_':case '-':
					case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
					case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
					case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
					case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
					case 'Y': case 'Z':
					case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
					case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
					case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
					case 's': case 't': case 'u': case 'v': case 'w': case 'x':
					case 'y': case 'z':
					case '0': case '1': case '2': case '3': case '4':
					case '5': case '6': case '7': case '8': case '9':
						Lexer_accept_and_next_char(self);
						break;
					default:
						if (strncmp(str_true, self->token.text_data, str_true_len) == 0) {
							self->token.type = TokenType__bool;
							self->token.value.bool_value = true;
						}
						else if (strncmp(str_false, self->token.text_data, str_false_len) == 0) {
							self->token.type = TokenType__bool;
							self->token.value.bool_value = false;
						}
						else
							self->token.type = TokenType__identifier;
						return;
				}
				break;
				// LexerParsingState__identifier
				case LexerParsingState__zero_digit:
					switch(current_char) {
						case '.':
							state = LexerParsingState__real;
							Lexer_accept_and_next_char(self);
							break;
						case 'X':
						case 'x':
							state = LexerParsingState__hexadecimal_integer;
							Lexer_accept_and_next_char(self);
							break;
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
							return;
							break;
						default:
							Lexer_parse_decimal_integer(self);
							self->token.type = TokenType__integer;
							return;
					}
					break;
					// case LexerParsingState__zero_digit
				case LexerParsingState__decimal_integer:
					switch(current_char) {
						case '.':
							state = LexerParsingState__real;
							Lexer_accept_and_next_char(self);
							break;
						case '0': case '1': case '2': case '3': case '4':
						case '5': case '6': case '7': case '8': case '9':
							Lexer_accept_and_next_char(self);
							break;
						default:
							Lexer_parse_decimal_integer(self);
							self->token.type = TokenType__integer;
							return;
					}
					break;
					// case LexerParsingState__decimal_integer
				case LexerParsingState__hexadecimal_integer:
					if (isxdigit(current_char))
						Lexer_accept_and_next_char(self);
					else {
						Lexer_parse_hexadecimal_integer(self);
						self->token.type = TokenType__integer;
						return;
					}
					break;
					// case LexerParsingState__hexadecimal_integer
				case LexerParsingState__real:
					if (isdigit(current_char))
						Lexer_accept_and_next_char(self);
					else {
						Lexer_parse_real(self);
						self->token.type = TokenType__real;
						return;
					}
					break;
		}
	}
}


// --- Testing ----------------------------------------------------------------

#ifdef DEBUG
void
Lexer_test() {
	static Lexer lexer;

	Lexer_init(&lexer, stdin);
	Lexer_next_token(&lexer);
	for( ; (lexer.token.type != TokenType__invalid) && (lexer.token.type != TokenType__eof); Lexer_next_token(&lexer))
		printf("[%s]", lexer.token.text);
	printf("\n");
}
#endif
