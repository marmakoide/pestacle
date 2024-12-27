#include <SDL.h>
#include <stdlib.h>
#include <pestacle/errors.h>


void
handle_input_read_error() {
	SDL_LogError(
		SDL_LOG_CATEGORY_SYSTEM,
		"Read error while reading input"
	);
	exit(EXIT_FAILURE);
}


void
handle_out_of_memory_error() {
	SDL_LogError(
		SDL_LOG_CATEGORY_SYSTEM,
		"Out of memory"
	);
	exit(EXIT_FAILURE);
}


void 
log_error(
	const char* format,
	...
) {
	va_list args;
	va_start(args, format);
	SDL_LogMessageV(
		SDL_LOG_CATEGORY_SYSTEM,
		SDL_LOG_PRIORITY_ERROR,
		format,
		args
	);
	va_end(args);
}
