#include <stdlib.h>

#include <stdbool.h>
#include <SDL.h>
#include "graph.h"
#include "event.h"
#include "scope.h"
#include "root_scope.h"
#include "parser/parser.h"
#include "window_manager.h"


// --- Main entry point -------------------------------------------------------

bool quit = false;

//static SDL_mutex* graph_state_mutex = 0;

Graph graph;
Scope* root_scope = 0;
WindowManager window_manager;

/*
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
*/


static bool
load_config() {
	Lexer lexer;
	Lexer_init(&lexer, stdin);
	return Parser_parse(&lexer, root_scope, &window_manager);
}


int
main(int argc, char* argv[]) {
	//SDL_Surface* framebuffer_native = 0;

	//SDL_TimerID graph_update_timer = 0;
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
	WindowManager_init(&window_manager);

	// Initialize root scope
	root_scope = Scope_new(
		&(root_scope_delegate.name),
		&root_scope_delegate
	);

	if (!Scope_setup(root_scope, &window_manager))
		goto termination;

	// Load configuraiton
	if (!load_config())
		goto termination;

	// Initialize the graph
	if (!Graph_init(&graph, root_scope))
		goto termination;

	if (!Graph_setup(&graph))
		goto termination;

	/*
	// Initialize the animation state mutex
	graph_state_mutex = SDL_CreateMutex();
	if (!graph_state_mutex) {
		exit_code = EXIT_FAILURE;
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to create state mutex: %s", SDL_GetError());
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
	*/

	// Main processing loop
	Uint64 performance_refresh_period = SDL_GetPerformanceFrequency() / 60;

	for(quit = false; !quit; ) {
		Uint64 start_time = SDL_GetPerformanceCounter();

		// Event processing
		SDL_Event src_event;
		//Event dst_event;

		while (SDL_PollEvent(&src_event)) {
			switch(src_event.type) {
				case SDL_QUIT:
					//SDL_LockMutex(graph_state_mutex);
					quit = true;
					//SDL_UnlockMutex(graph_state_mutex);
					break;

				/*
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
				*/

				default:
					break;
			}
		}

		// Graph update
		Graph_update(&graph);

		// Update all windows
		WindowManager_update_windows(&window_manager);

		// Sleep
		Uint64 end_time = SDL_GetPerformanceCounter();
		Uint64 time_delta = end_time - start_time;
		if (time_delta < performance_refresh_period)
			SDL_Delay((performance_refresh_period - time_delta) / (1e-3f * SDL_GetPerformanceFrequency()));
	}

	// Wait for the timers to stop
	//SDL_Delay(100);

	// Free ressources
termination:
	//if (graph_update_timer)
	//	SDL_RemoveTimer(graph_update_timer);

	Scope_destroy(root_scope);

	Graph_destroy(&graph);

	WindowManager_destroy(&window_manager);
	
	//if (graph_state_mutex)
	//	SDL_DestroyMutex(graph_state_mutex);
	
	SDL_Quit();

	// Job done
	return exit_code;
}