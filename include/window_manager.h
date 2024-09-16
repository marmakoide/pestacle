#ifndef PESTACLE_WINDOW_MANAGER_H
#define PESTACLE_WINDOW_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <SDL.h>


// --- Window definitions -----------------------------------------------------

struct s_Window;

typedef struct s_Window Window;


struct s_Window {
	Window* next;
	SDL_Window* window;
	SDL_Surface* surface;
	SDL_Renderer* renderer;
}; // struct s_Window


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


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_WINDOW_MANAGER_H */
