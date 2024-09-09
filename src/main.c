#include <stdlib.h>

#include <stdbool.h>
#include <SDL.h>
#include "event.h"
#include "domain.h"
#include "root_domain.h"
#include "parser/parser.h"
#include "window_manager.h"


const unsigned int DISPLAY_WIDTH  = 1000;
const unsigned int DISPLAY_HEIGHT = 500;

const unsigned int EMULATED_DISPLAY_WIDTH  = 200;
const unsigned int EMULATED_DISPLAY_HEIGHT = 100;


/*
const unsigned int DISPLAY_WIDTH  = 640;
const unsigned int DISPLAY_HEIGHT = 360;

const unsigned int EMULATED_DISPLAY_WIDTH  = 640;
const unsigned int EMULATED_DISPLAY_HEIGHT = 360;
*/

/*
const unsigned int DISPLAY_WIDTH  = 1000;
const unsigned int DISPLAY_HEIGHT = 750;

const unsigned int EMULATED_DISPLAY_WIDTH  = 200;
const unsigned int EMULATED_DISPLAY_HEIGHT = 150;
*/

// --- Main entry point -------------------------------------------------------

bool quit = false;

static SDL_mutex* graph_state_mutex = 0;

Graph graph;
Domain* root_domain = 0;


static Uint32
graph_update_callback(Uint32 interval, void* param) {
	Uint32 ret = 0;

	// Update the graph state
	SDL_LockMutex(graph_state_mutex);
	if (!quit) {
		Graph_update(&graph);
		ret = interval;
	}
	SDL_UnlockMutex(graph_state_mutex);

	// Job done
	return ret;
}


static bool
load_graph() {
	Lexer lexer;
	Lexer_init(&lexer, stdin);
	return Parser_parse(&lexer, root_domain, &graph);
}


int
main(int argc, char* argv[]) {
	SDL_Surface* framebuffer_native = 0;

	SDL_TimerID graph_update_timer = 0;
	int exit_code = EXIT_SUCCESS;

	// SDL logging settings
	 SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
	 
	// SDL initialization
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS)) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize SDL: %s", SDL_GetError());
		goto termination;
	}

	// Initialize the window manager
	WindowManager window_manager;
	WindowManager_init(&window_manager);

	// Initialize root domain
	root_domain = Domain_new(
		&(root_domain_delegate.name),
		&root_domain_delegate
	);

	if (!Domain_setup(root_domain, &window_manager))
		goto termination;

	// Initialize and setup the graph
	Graph_init(&graph);

	if (!load_graph())
		goto termination;

	if (!Graph_setup(&graph))
		goto termination;

	// Fluff
	Display display;

	Display_init(
		&display,
		DISPLAY_WIDTH,
		DISPLAY_HEIGHT,
		EMULATED_DISPLAY_WIDTH,
		EMULATED_DISPLAY_HEIGHT
	);

	// Initialize the animation state mutex
	graph_state_mutex = SDL_CreateMutex();
	if (!graph_state_mutex) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to create state mutex: %s", SDL_GetError());
		goto termination;
	}

	// Create a window
	Window* window = WindowManager_add_window(
		&window_manager,
		"pestacle",
		DISPLAY_WIDTH,
		DISPLAY_HEIGHT
	);

	if (!window) {
		exit_code = EXIT_FAILURE;
		goto termination;
	}

	// Create a framebuffer with same format as window surface
	framebuffer_native =
		SDL_CreateRGBSurfaceWithFormat(
			0,
			EMULATED_DISPLAY_WIDTH,
			EMULATED_DISPLAY_HEIGHT,
			window->surface->format->BitsPerPixel,
			window->surface->format->format
		);
	
	if (!framebuffer_native) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Could not create SDL surface : %s\n", SDL_GetError());
		goto termination;
	}

	// Setup a 20Hz for the animation logic update
	graph_update_timer = SDL_AddTimer(
		50,
		graph_update_callback,
		0
	);
	
	if (!graph_update_timer) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Could not create SDL timer for state update : %s\n", SDL_GetError());
		goto termination;
	}

	// Main processing loop
	Uint64 performance_screen_refresh_period = SDL_GetPerformanceFrequency() / 60;

	for(quit = false; !quit; ) {
		Uint64 start_time = SDL_GetPerformanceCounter();

		// Event processing
		SDL_Event src_event;
		Event dst_event;

		while (SDL_PollEvent(&src_event)) {
			switch(src_event.type) {
				case SDL_QUIT:
					SDL_LockMutex(graph_state_mutex);
					quit = true;
					SDL_UnlockMutex(graph_state_mutex);
					break;

				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:
					if (cast_mouse_button_event(&display, &src_event, &dst_event)) {
						SDL_LockMutex(graph_state_mutex);
						Graph_handle_event(&graph, &dst_event);
						SDL_UnlockMutex(graph_state_mutex);
					}
					break;

				case SDL_MOUSEMOTION:
					if (cast_mouse_motion_event(&display, &src_event, &dst_event)) {
						SDL_LockMutex(graph_state_mutex);
						Graph_handle_event(&graph, &dst_event);
						SDL_UnlockMutex(graph_state_mutex);
					}
					break;

				default:
					break;
			}
		}

		// Display update
		SDL_LockMutex(graph_state_mutex);
		SDL_Surface* out = Graph_output(&graph);
		SDL_UnlockMutex(graph_state_mutex);

		SDL_BlitSurface(out, 0, framebuffer_native, 0);
		SDL_BlitScaled(framebuffer_native, 0, window->surface, &(display.visible_area));
		SDL_UpdateWindowSurface(window->window);

		// Sleep
		Uint64 end_time = SDL_GetPerformanceCounter();
		Uint64 time_delta = end_time - start_time;
		if (time_delta < performance_screen_refresh_period)
			SDL_Delay((performance_screen_refresh_period - time_delta) / (1e-3f * SDL_GetPerformanceFrequency()));
	}

	// Wait for the timers to stop
	SDL_Delay(100);

	// Free ressources
termination:
	if (graph_update_timer)
		SDL_RemoveTimer(graph_update_timer);

	Domain_destroy(root_domain);

	Graph_destroy(&graph);

	if (framebuffer_native)
		SDL_FreeSurface(framebuffer_native);

	WindowManager_destroy(&window_manager);
	
	if (graph_state_mutex)
		SDL_DestroyMutex(graph_state_mutex);
	
	SDL_Quit();

	// Job done
	return exit_code;
}