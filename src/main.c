#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <SDL.h>
#include "graph.h"
#include "scope.h"
#include "scopes/root.h"
#include "parser/parser.h"
#include "window_manager.h"
#include "argtable3.h"


// --- Command-line arguments -------------------------------------------------

struct arg_lit *help;
struct arg_file *file;
struct arg_end *end;


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
load_script(const char* path) {
	// Open input file
	FILE* fp = fopen(path, "r");
	if (!fp) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to open file '%s': %s",
			path,
			strerror(errno)
		);
		return false;
	}

	// Parse the file
	Lexer lexer;
	Lexer_init(&lexer, fp);
	bool ret = Parser_parse(&lexer, root_scope, &window_manager);

	// Job done
	fclose(fp);
	return ret;
}


int
main(int argc, char* argv[]) {
	int exit_code = EXIT_SUCCESS;
	char prog_name[] = "pestacle";  

	// Command-line parsing
	void* argtable[] = {
		help    = arg_litn(NULL, "help", 0, 1, "display this help and exit"),
		file    = arg_filen(NULL, NULL, "<file>", 1, 1, "input script"),
		end     = arg_end(20),
	};

	int cmd_line_error_count = arg_parse(argc, argv, argtable);

	if (help->count > 0) {
		printf("Usage: %s", prog_name);
		arg_print_syntax(stdout, argtable, "\n");
		printf("Runs a pestacle script.\n\n");
		arg_print_glossary(stdout, argtable, "  %-25s %s\n");

		exit_code = EXIT_FAILURE;
		goto termination;
	}

	if (cmd_line_error_count > 0) {
		arg_print_errors(stdout, end, prog_name);
		printf("Try '%s --help' for more information.\n", prog_name);

		goto termination;
	}

	//SDL_TimerID graph_update_timer = 0;

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

	// Load script
	if (!load_script(file->filename[0]))
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
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					quit = true;
					break;

				default:
					WindowManager_dispatch_event(&window_manager, &event);
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

	// Free ressources
termination:
	//if (graph_update_timer)
	//	SDL_RemoveTimer(graph_update_timer);

	if (root_scope)
		Scope_destroy(root_scope);

	Graph_destroy(&graph);

	WindowManager_destroy(&window_manager);
	
	//if (graph_state_mutex)
	//	SDL_DestroyMutex(graph_state_mutex);
	
	SDL_Quit();

	arg_freetable(
		argtable,
		sizeof(argtable) / sizeof(argtable[0])
	);

	// Job done
	return exit_code;
}
