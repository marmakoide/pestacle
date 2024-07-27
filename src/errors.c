#include <SDL.h>
#include <stdlib.h>


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