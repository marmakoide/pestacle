#include <SDL.h>
#include <stdlib.h>
#include "errors.h"


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
		"Out of memory\n"
	);
	exit(EXIT_FAILURE);
}


void 
handle_processing_error(
	const FileLocation* location,
	const char* format,
	...) {
	SDL_LogError(
		SDL_LOG_CATEGORY_SYSTEM,
		"line %d ", location->line + 1	
   	);
	
    va_list args;
	va_start(args, format);
	SDL_LogMessageV(
		SDL_LOG_CATEGORY_SYSTEM,
		SDL_LOG_PRIORITY_ERROR,
		format,
		args
	);
    va_end(args);

	exit(EXIT_FAILURE);
}