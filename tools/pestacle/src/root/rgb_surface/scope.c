#include "root/rgb_surface/blend.h"
#include "root/rgb_surface/overlay.h"
#include "root/rgb_surface/resize.h"

#include "root/rgb_surface/scope.h"


// --- Interface --------------------------------------------------------------

static bool
rgb_surface_scope_setup(
	Scope* self
);


#define NODE_DELEGATE_LIST_END 0

static const NodeDelegate*
node_delegate_list[] = {
	&root_rgb_surface_blend_node_delegate,
	&root_rgb_surface_overlay_node_delegate,
	&root_rgb_surface_resize_node_delegate,
	NODE_DELEGATE_LIST_END
}; // node_delegate_list


#define SCOPE_DELEGATE_LIST_END 0

static const ScopeDelegate*
scope_delegate_list[] = {
	SCOPE_DELEGATE_LIST_END
}; // scope_delegate_list


static const ParameterDefinition
rgb_surface_scope_parameters[] = {
	PARAMETER_DEFINITION_END
};


const ScopeDelegate
root_rgb_surface_scope_delegate = {
	"rgb-surface",
	rgb_surface_scope_parameters,
	{
		rgb_surface_scope_setup,
		0
	}
};


// --- Implementation ---------------------------------------------------------

bool
rgb_surface_scope_setup(
	Scope* self
) {
	const NodeDelegate** node_delegate_ptr = node_delegate_list;
	for( ; *node_delegate_ptr != NODE_DELEGATE_LIST_END; ++node_delegate_ptr)
		if (!Scope_add_node_delegate(self, *node_delegate_ptr))
			return false;

	const ScopeDelegate** scope_delegate_ptr = scope_delegate_list;
	for( ; *scope_delegate_ptr != SCOPE_DELEGATE_LIST_END; ++scope_delegate_ptr)
		if (!Scope_add_scope_delegate(self, *scope_delegate_ptr))
			return false;

	return true;
}
