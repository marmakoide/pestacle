#ifndef PESTACLE_WINDOW_MANAGER_H
#define PESTACLE_WINDOW_MANAGER_H

#include <SDL.h>


// --- Window definitions -----------------------------------------------------

struct s_Window;

typedef struct s_Window Window;


struct s_Window {
	Window* next;
	SDL_Window* window;
	SDL_Surface* surface;
	SDL_Renderer* renderer;
};


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
	int w,
	int h
);


#endif /* #define PESTACLE_WINDOW_MANAGER_H */
