#include <stdlib.h>
#include <errno.h>
#include <SDL.h>

#include <pestacle/macros.h>
#include <pestacle/graph.h>
#include <pestacle/scope.h>
#include <pestacle/memory.h>
#include <pestacle/plugin_manager.h>
#include <pestacle/window_manager.h>
#include <pestacle/parser/parser.h>

#include "cmdline.h"
#include "scopes/root.h"


static void
initialization_log() {
	static const char* bool_str[2] = {
		"no", "yes"
	};

	// SDL version information
	SDL_version sdl_version_data;

	SDL_GetVersion(&sdl_version_data);
	SDL_Log(
		"current SDL version %u.%u.%u",
		sdl_version_data.major,
		sdl_version_data.minor,
		sdl_version_data.patch
	);

	SDL_VERSION(&sdl_version_data);
	SDL_Log(
		"targeted SDL version %u.%u.%u",
		sdl_version_data.major,
		sdl_version_data.minor,
		sdl_version_data.patch
	);

	// Platform informations
	SDL_Log(
		"%s platform",
		SDL_GetPlatform()
	);

	// CPU informations
	SDL_Log(
		"%d CPU(s) detected",
		SDL_GetCPUCount()
	);

	SDL_Log(
		"  MMX     => %s",
		bool_str[SDL_HasMMX()]
	);

	SDL_Log(
		"  3DNow   => %s",
		bool_str[SDL_Has3DNow()]
	);

	SDL_Log(
		"  SSE     => %s",
		bool_str[SDL_HasSSE()]
	);

	SDL_Log(
		"  SSE2    => %s",
		bool_str[SDL_HasSSE2()]
	);

	SDL_Log(
		"  SSE3    => %s",
		bool_str[SDL_HasSSE3()]
	);

	SDL_Log(
		"  SSE41   => %s",
		bool_str[SDL_HasSSE41()]
	);

	SDL_Log(
		"  SSE42   => %s",
		bool_str[SDL_HasSSE42()]
	);

	SDL_Log(
		"  AVX     => %s",
		bool_str[SDL_HasAVX()]
	);

	SDL_Log(
		"  AVX2    => %s",
		bool_str[SDL_HasAVX2()]
	);

	SDL_Log(
		"  AVX512F => %s",
		bool_str[SDL_HasAVX512F()]
	);

	SDL_Log(
		"  NEON    => %s",
		bool_str[SDL_HasNEON()]
	);

	// Display informations
	int display_count = SDL_GetNumVideoDisplays();
	SDL_Log(
		"%d display(s) found",
		display_count
	);

	for(int i = 0; i < display_count; ++i) {
		SDL_DisplayMode mode;
		SDL_GetDesktopDisplayMode(i, &mode);
		
		SDL_Log(
			"display #%d => %dx%d %dsbpp %dHz",
			i,
			mode.w,
			mode.h,
			SDL_BITSPERPIXEL(mode.format),
			mode.refresh_rate
		);
	}
}


static bool
load_plugins(
	Scope* root_scope,
	WindowManager* window_manager
) {
	bool ret = true;
	char* base_path = 0;
	void* shared_obj = 0;
	Scope* scope = 0;

	// Retrieve the base path
	base_path = SDL_GetBasePath();
	if (!base_path) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to retrieve base path: %s",
			SDL_GetError()
		);
		ret = false;
		goto termination;
	}

	// Build the plugin path
	char plugin_path[1024];
	ssize_t len = snprintf(plugin_path, sizeof(plugin_path), "%splugins/ffmpeg.so", base_path);
	if ((len < 0) || (len >= (ssize_t)sizeof(plugin_path))) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to build plugin path"
		);
		ret = false;
		goto termination;
	}

	// Load the dynamic library
	shared_obj = SDL_LoadObject(plugin_path);
	if (!shared_obj) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to load plugin %s : %s",
			plugin_path,
			SDL_GetError()
		);
		ret = false;
		goto termination;
	}

	// Retrieve the "get_scope_delegate" function
	PluginEntryPoint entry_point = SDL_LoadFunction(shared_obj, "get_scope_delegate");
	if (!entry_point) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to find plugin %s entry point: %s",
			plugin_path,
			SDL_GetError()
		);
		ret = false;
		goto termination;
	}

	// Check that scope delegate does not have parameters
	const ScopeDelegate* delegate = entry_point();
	if (ParameterDefinition_has_parameters(delegate->parameter_defs)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"plugin %s is invalid: scope delegate with parameters",
			plugin_path
		);
		ret = false;
		goto termination;
	}

	// Build the scope
	scope = Scope_new(&(delegate->name), delegate, 0);
	if (!scope) {
		ret = false;
		goto termination;
	}

	// Setup the scope
	if (!Scope_setup(scope, window_manager)) {
		ret = false;
		goto termination;
	}

	// Add the scope to the root scope
	if (!Scope_add_scope(root_scope, scope)) {
		ret = false;
		goto termination;
	}

	// Log on the success
	SDL_Log("loaded plugin %s from %s", scope->name.data, plugin_path);

	// Job done
termination:
	if ((scope) && (!ret)) {
		Scope_destroy(scope);
		free(scope);
	}

	if ((shared_obj) && (!ret))
		SDL_UnloadObject(shared_obj);

	if (base_path)
		free(base_path);

	return ret;
}


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

	// Log initialization infos
	initialization_log();

	// Initialize the window manager
	window_manager = (WindowManager*)checked_malloc(sizeof(WindowManager));
	WindowManager_init(window_manager);

	// Initialize root scope
	root_scope = Scope_new(
		&(root_scope_delegate.name),
		&root_scope_delegate,
		0
	);

	if (!Scope_setup(root_scope, window_manager)) {
		exit_code = EXIT_FAILURE;
		goto termination;
	}

	// Load the plugins
	if (!load_plugins(root_scope, window_manager)) {
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
