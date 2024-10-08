#include <assert.h>
#include <pestacle/memory.h>
#include <pestacle/window_manager.h>


// --- WindowEventListener implementation -------------------------------------

static void
WindowEventListener_init(
	WindowEventListener* self,
	void* caller,
	WindowEventListener__on_event callback
) {
	self->next = 0;
	self->caller = caller;
	self->callback = callback;
}


// --- Window implementation --------------------------------------------------

static void
Window_destroy(
	Window* self
) {
	assert(self);

	#ifdef DEBUG
	self->next = 0;
	#endif

	// Destroy the listeners
	if (self->listener_head) {
		for(WindowEventListener* listener = self->listener_head; listener != 0; ) {
			WindowEventListener* next_listener = listener->next;
			free(listener);
			listener = next_listener;
		}
	
		#ifdef DEBUG
		self->listener_head = 0;
		#endif
	}

	// Destroy the renderer
	if (self->renderer) {
		SDL_DestroyRenderer(self->renderer);
		#ifdef DEBUG
		self->renderer = 0;
		#endif
	}

	// Destroy the window
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
	assert(self);
	assert(title);

	self->next = 0;
	self->window = 0;
	self->surface = 0;
	self->renderer = 0;
	self->listener_head = 0;

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


static void
Window_update(
	Window* self
) {
	assert(self);
	assert(self->window);

	SDL_UpdateWindowSurface(self->window);
}


void
Window_set_bordered(
	Window* self,
	bool bordered
) {
	assert(self);

	SDL_SetWindowBordered(self->window, bordered);
}


void
Window_add_event_listener(
	Window* self,
	void* caller,
	WindowEventListener__on_event callback
) {
	assert(self);

	// Allocation
	WindowEventListener* listener =
		(WindowEventListener*)checked_malloc(sizeof(WindowEventListener));

	// Initialisation
	WindowEventListener_init(listener, caller, callback);

	// Update list of windows
	listener->next = self->listener_head;
	self->listener_head = listener;
}


static void
Window_dispatch_event(
	Window* self,
	SDL_Event* event
) {
	assert(self);

	// Select the event
	Uint32 windowID = SDL_GetWindowID(self->window);
	switch(event->type) {
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			if (event->button.windowID != windowID)
				return;
			break;

		case SDL_MOUSEMOTION:
			if (event->motion.windowID != windowID)
				return;
			break;

		case SDL_MOUSEWHEEL:
			if (event->wheel.windowID != windowID)
				return;
			break;

		case SDL_KEYUP:
		case SDL_KEYDOWN:
			if (event->key.windowID != windowID)
				return;
			break;

		case SDL_FINGERMOTION:
		case SDL_FINGERDOWN:
		case SDL_FINGERUP:
			if (event->tfinger.windowID != windowID)
				return;
			break;

		case SDL_WINDOWEVENT:
			if (event->window.windowID != windowID)
				return;
			break;

		default:
			return;
	}

	// Dispatch the event
	for(WindowEventListener* listener = self->listener_head; listener != 0; listener = listener->next)
		listener->callback(listener->caller, event);
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
		free(window);
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
	assert(self);
	assert(title);

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
	assert(self);
	assert(window);

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


void
WindowManager_update_windows(
	WindowManager* self
) {
	assert(self);

	for(Window* window = self->head; window != 0; window = window->next)
		Window_update(window);
}


void
WindowManager_dispatch_event(
	WindowManager* self,
	SDL_Event* event
) {
	assert(self);
	assert(event);

	for(Window* window = self->head; window != 0; window = window->next)
		Window_dispatch_event(window, event);
}
