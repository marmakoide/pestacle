#ifndef PESTACLE_WINDOW_MANAGER_H
#define PESTACLE_WINDOW_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <SDL.h>


// --- Window definitions -----------------------------------------------------

struct s_Window;

typedef struct s_Window Window;
typedef struct s_WindowEventListener WindowEventListener;

typedef void (*WindowEventListener__on_event)(
	void* listener,
	SDL_Event* event
);


struct s_WindowEventListener {
	WindowEventListener* next;
	void* caller;
	WindowEventListener__on_event callback;
}; 


struct s_Window {
	Window* next;
	SDL_Window* window;
	SDL_Surface* surface;
	SDL_Renderer* renderer;
	WindowEventListener* listener_head;
}; // struct s_Window


extern void
Window_set_bordered(
	Window* self,
	bool bordered
);


extern void
Window_add_event_listener(
	Window* self,
	void* caller,
	WindowEventListener__on_event callback
);


// --- Window manager definitions ---------------------------------------------

typedef struct {
	Window* head;
} WindowManager;


extern void
WindowManager_init(
	WindowManager* self
);


extern void
WindowManager_destroy(
	WindowManager* self
);


extern Window*
WindowManager_add_window(
	WindowManager* self,
	const char* title,
	int width,
	int height
);


extern bool
WindowManager_remove_window(
	WindowManager* self,
	Window* window
);


extern void
WindowManager_update_windows(
	WindowManager* self
);


extern void
WindowManager_dispatch_event(
	WindowManager* self,
	SDL_Event* event
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_WINDOW_MANAGER_H */
