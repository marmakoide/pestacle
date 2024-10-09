#ifndef PESTACLE_PLUGIN_MANAGER_H
#define PESTACLE_PLUGIN_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pestacle/scope.h>


// --- Plugin entry point definition ------------------------------------------

typedef const ScopeDelegate* (*PluginEntryPoint)();


// --- Plugin definitions -----------------------------------------------------

struct s_Plugin;
typedef struct s_Plugin Plugin;

struct s_Plugin {
	Plugin* next;
	void* shared_obj;
	const ScopeDelegate* delegate;
}; // struct s_Plugin


// --- Plugin manager definitions ---------------------------------------------

typedef struct {
	Plugin* head;
	char* plugin_path;
} PluginManager;


extern bool
PluginManager_init(
	PluginManager* self
);


extern void
PluginManager_destroy(
	PluginManager* self
);


extern bool
PluginManager_load_plugins(
	PluginManager* self
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_PLUGIN_MANAGER_H */
