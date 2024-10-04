#include <stdlib.h>
#include <errno.h>
#include <SDL.h>

#include <pestacle/graph.h>
#include <pestacle/scope.h>
#include <pestacle/memory.h>
#include <pestacle/window_manager.h>
#include <pestacle/parser/parser.h>

#include "cmdline.h"
#include "scopes/root.h"


static bool
load_script(
	const char* path,
	Scope* root_scope,
	WindowManager* window_manager
) {
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
	bool ret = Parser_parse(&lexer, root_scope, window_manager);

	// Job done
	fclose(fp);
	return ret;
}


int
main(int argc, char* argv[]) {
	Scope* root_scope = 0;
	Graph* graph = 0;
	GraphProfile* graph_profile = 0;
	WindowManager* window_manager = 0;

	CmdParameters params;
	int exit_code = EXIT_SUCCESS;

	// SDL logging settings
	 SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

	// Command line parsing
	CmdParameters_init(&params);
	if (!CmdParameters_parse(&params, argc, argv))
		return EXIT_FAILURE;
	
	// SDL initialization
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS)) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to initialize SDL: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	// Initialize the window manager
	window_manager = (WindowManager*)checked_malloc(sizeof(WindowManager));
	WindowManager_init(window_manager);

	// Initialize root scope and graph
	root_scope = Scope_new(
		&(root_scope_delegate.name),
		&root_scope_delegate,
		0
	);

	if (!Scope_setup(root_scope, window_manager)) {
		exit_code = EXIT_FAILURE;
		goto termination;
	}

	// Load script
	if (!load_script(params.input_path, root_scope, window_manager)) {
		exit_code = EXIT_FAILURE;
		goto termination;
	}

	// Initialize the graph
	graph = (Graph*)checked_malloc(sizeof(Graph));

	if (!Graph_init(graph, root_scope)) {
		exit_code = EXIT_FAILURE;
		goto termination;
	}

	if (!Graph_setup(graph)) {
		exit_code = EXIT_FAILURE;
		goto termination;
	}

	// Setup graph profiling if required
	graph_profile = (GraphProfile*)checked_malloc(sizeof(GraphProfile));

	if (params.profile_mode)
		GraphProfile_init(graph_profile, graph);

	// If we are in dry-run mode, terminate now
	if (params.dry_run)
		goto termination;

	// Main processing loop
	Uint64 performance_refresh_period =
		SDL_GetPerformanceFrequency() / params.frames_per_second;
	
	for(bool quit = false; !quit; ) {
		Uint64 start_time = SDL_GetPerformanceCounter();

		// Event processing
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					quit = true;
					break;

				default:
					WindowManager_dispatch_event(window_manager, &event);
					break;
			}
		}

		// Graph update
		if (params.profile_mode)
			Graph_update_with_profile(graph, graph_profile);
		else
			Graph_update(graph);

		// Update all windows
		WindowManager_update_windows(window_manager);

		// Sleep
		Uint64 end_time = SDL_GetPerformanceCounter();
		Uint64 time_delta = end_time - start_time;
		if (time_delta < performance_refresh_period)
			SDL_Delay((1e3f * (performance_refresh_period - time_delta)) / SDL_GetPerformanceFrequency());
	}

	// Print the profiling report if required
	if (params.profile_mode)
		GraphProfile_print_report(graph_profile, graph, stdout);

	// Free ressources
termination:
	if (root_scope) {
		Scope_destroy(root_scope);
		free(root_scope);
	}

	if (graph) {
		Graph_destroy(graph);
		free(graph);
	}

	if (params.profile_mode && graph_profile) {
		GraphProfile_destroy(graph_profile);
		free(graph_profile);
	}

	if (window_manager) {
		WindowManager_destroy(window_manager);
		free(window_manager);
	}

	CmdParameters_destroy(&params);

	SDL_Quit();

	// Job done
	return exit_code;
}
