#include "root/rgb_surface/blend.h"
#include "root/rgb_surface/overlay.h"
#include "root/rgb_surface/resize.h"

#include "root/rgb_surface/scope.h"


// --- Interface --------------------------------------------------------------

static bool
rgb_surface_scope_setup(
	Scope* self
);


static const NodeDelegate*
node_delegate_list[] = {
	&root_rgb_surface_blend_node_delegate,
	&root_rgb_surface_overlay_node_delegate,
	&root_rgb_surface_resize_node_delegate,
	0
}; // node_delegate_list


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
	return 
		Scope_populate(
			self,
			 node_delegate_list,
			 0,
			 0
		);
}
