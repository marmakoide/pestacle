#include <errno.h>
#include <dirent.h>

#include <SDL.h>
#include <assert.h>
#include <pestacle/memory.h>
#include <pestacle/plugin_manager.h>


// --- Plugin implementation --------------------------------------------------

static bool
Plugin_init(
	Plugin* self,
	const char* absolute_path
) {
	assert(self);
	assert(absolute_path);

	bool ret = true;

	// Acquire the shared object
	self->shared_obj = SDL_LoadObject(absolute_path);
	if (!self->shared_obj) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to load plugin %s : %s",
			absolute_path,
			SDL_GetError()
		);
		ret = false;
		goto termination;
	}

	// Retrieve the "get_scope_delegate" function
	PluginEntryPoint entry_point =
		SDL_LoadFunction(self->shared_obj, "get_scope_delegate");

	if (!entry_point) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to find plugin %s entry point: %s",
			absolute_path,
			SDL_GetError()
		);
		ret = false;
		goto termination;
	}

	// Check that scope delegate does not have parameters
	self->delegate = entry_point();
	if (ParameterDefinition_has_parameters(self->delegate->parameter_defs)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"plugin %s is invalid: scope delegate with parameters",
			absolute_path
		);
		ret = false;
		goto termination;
	}

	// Job done
termination:
	if ((self->shared_obj) && (!ret)) {
		SDL_UnloadObject(self->shared_obj);
		self->shared_obj = 0;
		self->delegate = 0;
	}

	return ret;
}


static void
Plugin_destroy(
	Plugin* self
) {
	assert(self);
	assert(self->shared_obj);

	SDL_UnloadObject(self->shared_obj);

	#ifdef DEBUG
	self->next = 0;
	self->shared_obj = 0;
	self->delegate = 0;
	#endif
}


// --- PluginManager implementation --------------------------------------------

#define PLUGIN_PATH_LENGTH 1024

bool
PluginManager_init(
	PluginManager* self
) {
	assert(self);

	bool ret = true;
	char* base_path = 0;

	self->head = 0;
	self->plugin_path = 0;

	// Retrieve base path
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

	// Buid plugin path
	self->plugin_path =
		(char*)checked_malloc(PLUGIN_PATH_LENGTH);

	ssize_t len = snprintf(
		self->plugin_path,
		PLUGIN_PATH_LENGTH,
		"%splugins/", 
		base_path
	);

	if ((len < 0) || (len >= (ssize_t)PLUGIN_PATH_LENGTH)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to build plugin path"
		);
		ret = false;
		goto termination;
	}

termination:
	if (base_path)
		free(base_path);

	return ret;
}


extern void
PluginManager_destroy(
	PluginManager* self
) {
	assert(self);

	free(self->plugin_path);

	for(Plugin* plugin = self->head; plugin != 0; ) {
		Plugin* next_plugin = plugin->next;
		Plugin_destroy(plugin);
		free(plugin);
		plugin = next_plugin;
	}

	#ifdef DEBUG
	self->head = 0;
	self->plugin_path = 0;
	#endif
}


static Plugin*
PluginManager_add_plugin(
	PluginManager* self,
	const char* relative_path
) {
	assert(self);
	assert(relative_path);

	// Allocate
	Plugin* ret = (Plugin*)checked_malloc(sizeof(Plugin));
	if (!ret)
		return 0;

	// Initialisation
	char* absolute_path =
		(char*)checked_malloc(PLUGIN_PATH_LENGTH);

	ssize_t len = snprintf(
		absolute_path,
		PLUGIN_PATH_LENGTH,
		"%s%s", 
		self->plugin_path,
		relative_path
	);

	if ((len < 0) || (len >= (ssize_t)PLUGIN_PATH_LENGTH)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to build plugin path %s%s",
			self->plugin_path,
			relative_path
		);
		ret = false;
		goto termination;
	}

	if (!Plugin_init(ret, absolute_path)) {
		free(ret);
		return 0;
	}

	// Update list of windows
	ret->next = self->head;
	self->head = ret;

termination:
	if (absolute_path)
		free(absolute_path);

	// Job done
	return ret;
}


bool
PluginManager_load_plugins(
	PluginManager* self
) {
	assert(self);

	#if defined(__linux__)
	static const char* plugin_name_suffix = ".so";
	#elif defined(__MINGW32__)
	static const char* plugin_name_suffix = ".dll";
	#endif
 
	size_t plugin_name_suffix_len = strlen(plugin_name_suffix);

	bool exit_code = true;
	DIR* dir = 0;

	// Open the plugin directory
	dir = opendir(self->plugin_path);
	if (!dir) {
		exit_code = false;
		goto termination;
	}

	// Scan the directory
	struct dirent* entry = readdir(dir);
	for( ; entry != NULL; entry = readdir(dir)) {
		// Track names that end with proper suffix
		size_t entry_name_len = strlen(entry->d_name);
		
		if (
			(entry_name_len > plugin_name_suffix_len) && 
			(strcmp(entry->d_name + entry_name_len - plugin_name_suffix_len, plugin_name_suffix) == 0)
			) {
			if (PluginManager_add_plugin(self, entry->d_name))
				SDL_Log("loaded plugin %s", entry->d_name);
			else
				exit_code = false;
		}
	}

	// Job done
termination:
	if (dir) {
		if (closedir(dir))
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"Unable to close directory entry %s: %s",
				self->plugin_path,
				strerror(errno)
			);
	}

	return exit_code;
}
