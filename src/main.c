#include <stdlib.h>


#include <stdbool.h>
#include <SDL.h>
#include "animation.h"


// Dimensions of the display, in term of real displayed pixels
const unsigned int DISPLAY_WIDTH  = 1000;
const unsigned int DISPLAY_HEIGHT = 500;

// Dimensions of the simulated screeen
const unsigned int SCREEN_WIDTH  = 200;
const unsigned int SCREEN_HEIGHT = 100;


// --- Main entry point -------------------------------------------------------

static SDL_Window* window = 0;
static SDL_Surface* window_surface = 0;
static SDL_Renderer* renderer = 0;
static SDL_Surface* framebuffer_rgb24 = 0;
static SDL_Surface* framebuffer_native = 0;

static SDL_mutex* animation_state_mutex = 0;

struct Animation animation;


static Uint32
animation_state_update_callback(Uint32 interval, void* param) {
	// Update the animation statex
	SDL_LockMutex(animation_state_mutex);
	animation_update(&animation);
	SDL_UnlockMutex(animation_state_mutex);

	// Job done
	return interval;
}


static Uint32
framebuffer_update_callback(Uint32 interval, void* param) {
	// Render
	SDL_LockMutex(animation_state_mutex);
	animation_render(&animation, framebuffer_rgb24);
	SDL_UnlockMutex(animation_state_mutex);
	
	// Update the framebuffer
	SDL_BlitSurface(framebuffer_rgb24, 0, framebuffer_native, 0);
	SDL_BlitScaled(framebuffer_native, 0, window_surface, 0);
	SDL_UpdateWindowSurface(window);

	// Job done
	return interval;
}


int
main(int argc, char* argv[]) {
	SDL_TimerID animation_state_update_timer = 0;
	SDL_TimerID framebuffer_update_timer = 0;
	int exit_code = EXIT_SUCCESS;

	// SDL logging settings
	 SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
	 
	// SDL initialization
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize SDL: %s", SDL_GetError());
		goto termination;
	}

	// Initialize animation
	if (!animation_init(
		&animation,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		DISPLAY_WIDTH,
		DISPLAY_HEIGHT)
	)
		goto termination;

	// Initialize the animation state mutex
	animation_state_mutex = SDL_CreateMutex();
	if (!animation_state_mutex) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to create state mutex: %s", SDL_GetError());
		goto termination;
	}

	// Create a window
	window = SDL_CreateWindow(
		"pestacle",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		DISPLAY_WIDTH,
		DISPLAY_HEIGHT,
		0
	);
	
	if (!window) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Could not create window: %s\n", SDL_GetError());
		goto termination;
	}

	// Disable mouse cursor
	SDL_ShowCursor(SDL_DISABLE);

	// Create a renderer
	window_surface = SDL_GetWindowSurface(window);
	renderer = SDL_CreateSoftwareRenderer(window_surface);
	if (!renderer) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Could not create SDL renderer : %s\n", SDL_GetError());
		goto termination;
	}
	
	// Create a RGB framebuffer
	framebuffer_rgb24 =
		SDL_CreateRGBSurfaceWithFormat(
			0,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			24,
			SDL_PIXELFORMAT_RGB24
		);

	if (!framebuffer_rgb24) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Could not create SDL surface : %s\n", SDL_GetError());
		goto termination;
	}

	// Create a framebuffer with same format as window surface
	framebuffer_native =
		SDL_CreateRGBSurfaceWithFormat(
			0,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			window_surface->format->BitsPerPixel,
			window_surface->format->format
		);

	if (!framebuffer_native) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Could not create SDL surface : %s\n", SDL_GetError());
		goto termination;
	}

	// Setup a 60Hz timer for the framebuffer update
	framebuffer_update_timer = 
		SDL_AddTimer(
			15,
			framebuffer_update_callback,
			0
		);
	
	if (!framebuffer_update_timer) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Could not create SDL timer for framebuffer update : %s\n", SDL_GetError());
		goto termination;
	}

	// Setup a 20Hz for the animation logic update
	animation_state_update_timer = SDL_AddTimer(
		50,
		animation_state_update_callback,
		0
	);
	
	if (!animation_state_update_timer) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Could not create SDL timer for state update : %s\n", SDL_GetError());
		goto termination;
	}

	// Event processing loop
	for(bool quit = false; !quit; ) {
		SDL_Event event;
		if (SDL_WaitEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					quit = true;
					break;

				default:
					SDL_LockMutex(animation_state_mutex);
					animation_handle_event(&animation, &event);
					SDL_UnlockMutex(animation_state_mutex);
					break;
			}
		}
	}

	// Free ressources
termination:
	animation_destroy(&animation);

	if (animation_state_mutex)
		SDL_DestroyMutex(animation_state_mutex);

	if (animation_state_update_timer)
		SDL_RemoveTimer(animation_state_update_timer);

	if (framebuffer_update_timer)
		SDL_RemoveTimer(framebuffer_update_timer);
	
	if (framebuffer_rgb24)
		SDL_FreeSurface(framebuffer_rgb24);
	
	if (framebuffer_native)
		SDL_FreeSurface(framebuffer_native);

	if (renderer)
		SDL_DestroyRenderer(renderer);

	if (window)
		SDL_DestroyWindow(window);
	
	SDL_Quit();

	// Job done
	return exit_code;
}

