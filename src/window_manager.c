#include <assert.h>
#include "memory.h"
#include "window_manager.h"


// --- Window implementation --------------------------------------------------

static void
Window_destroy(
	Window* self
) {
	assert(self != 0);

	#ifdef DEBUG
	self->next = 0;
	#endif

	if (self->renderer) {
		SDL_DestroyRenderer(self->renderer);
		#ifdef DEBUG
		self->renderer = 0;
		#endif
	}

	if (self->window) {
		SDL_DestroyWindow(self->window);
		#ifdef DEBUG
		self->window = 0;
		#endif
	}
}


static bool
Window_init(
	Window* self,
	const char* title,
	int width,
	int height
) {
	assert(self != 0);
	assert(title != 0);

	self->next = 0;
	self->window = 0;
	self->surface = 0;
	self->renderer = 0;

	self->window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		0
	);
	
	if (!self->window) {
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Could not create window: %s\n", SDL_GetError());
		goto failure;
	}

	// Create a renderer
	self->surface = SDL_GetWindowSurface(self->window);
	self->renderer = SDL_CreateSoftwareRenderer(self->surface);

	if (!self->renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Could not create SDL renderer : %s\n", SDL_GetError());
		goto failure;
	}

	// Job done successfully
	return true;

	// Job failed
failure:
	Window_destroy(self);

	return false;
}


// --- WindowManager implementation -------------------------------------------

void
WindowManager_init(
	WindowManager* self
) {
	assert(self);

	self->head = 0;
}


void
WindowManager_destroy(
	WindowManager* self
) {
	assert(self);

	for(Window* window = self->head; window != 0; ) {
		Window* next_window = window->next;
		Window_destroy(window);
		window = next_window;
	}

	#ifdef DEBUG
	self->head = 0;
	#endif
}


static bool
WindowManager_find_window_before(
	WindowManager* self,
	Window* window,
	Window** before_window
) {
	assert(self);
	assert(window);
	assert(before_window);

	if (self->head == window) {
		*before_window = 0;
		return true;
	}

	for(*before_window = self->head; *before_window != 0; *before_window = (*before_window)->next)
		if ((*before_window)->next == window)
			return true;

	return false;
}


Window*
WindowManager_add_window(
	WindowManager* self,
	const char* title,
	int width,
	int height
) {
	assert(self != 0);
	assert(title != 0);

	// Allocate
	Window* ret = (Window*)checked_malloc(sizeof(Window));
	if (!ret)
		return 0;

	// Initialisation
	if (!Window_init(ret, title, width, height)) {
		free(ret);
		return 0;
	}

	// Update list of windows
	ret->next = self->head;
	self->head = ret;
		
	// Job done
	return ret;
}


bool
WindowManager_remove_window(
	WindowManager* self,
	Window* window
) {
	assert(self != 0);
	assert(window != 0);

	// Look for the window linked before window of interest
	Window* before_window;
	if (!WindowManager_find_window_before(self, window, &before_window)) {
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
		"Could not find window instance\n");
		return false;
	}

	// Update the windows linked list
	if (!before_window)
		self->head = window->next;
	else
		before_window->next = window->next;

	// Destroy the window
	Window_destroy(window);

	// Job done
	return true;
}