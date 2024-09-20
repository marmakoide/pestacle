#include "macros.h"

#include "nodes/gradient_map.h"
#include "nodes/heat_diffusion.h"
#include "nodes/lightness.h"
#include "nodes/matrix_resize.h"
#include "nodes/picture.h"
#include "nodes/surface_blend.h"
#include "nodes/surface_resize.h"
#include "nodes/video.h"

#include "scopes/window.h"


// --- Interface --------------------------------------------------------------

static bool
root_scope_setup(
	Scope* self,
	WindowManager* window_manager
);


#define NODE_DELEGATE_LIST_END 0

static const NodeDelegate*
node_delegate_list[] = {
	&gradient_map_node_delegate,
	&heat_diffusion_node_delegate,
	&lightness_node_delegate,
	&matrix_resize_node_delegate,
	&picture_node_delegate,
	&surface_blend_node_delegate,
	&surface_resize_node_delegate,
	&video_node_delegate,
	NODE_DELEGATE_LIST_END
}; // node_delegate_list


#define SCOPE_DELEGATE_LIST_END 0

static const ScopeDelegate*
scope_delegate_list[] = {
	&window_scope_delegate,
	SCOPE_DELEGATE_LIST_END
}; // scope_delegate_list


static const ParameterDefinition
root_scope_parameters[] = {
	PARAMETER_DEFINITION_END
};


const ScopeDelegate root_scope_delegate = {
	{ "root", 5 },
	root_scope_parameters,
	{
		root_scope_setup,
		0
	}
};


// --- Implementation ---------------------------------------------------------

bool
root_scope_setup(
	Scope* self,
	ATTRIBUTE_UNUSED WindowManager* window_manager
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
