#include "root/gradient_map.h"
#include "root/heat_diffusion.h"
#include "root/matrix_resize.h"

#include "window/scope.h"
#include "root/rgb_surface/scope.h"


// --- Interface --------------------------------------------------------------

static bool
root_scope_setup(
	Scope* self
);


static const NodeDelegate*
node_delegate_list[] = {
	&gradient_map_node_delegate,
	&heat_diffusion_node_delegate,
	&matrix_resize_node_delegate,
	0
}; // node_delegate_list


static const ScopeDelegate*
scope_delegate_list[] = {
	&window_scope_delegate,
	0
}; // scope_delegate_list


static const ScopeDelegate*
scope_instance_delegate_list[] = {
	&root_rgb_surface_scope_delegate,
	0
}; // scope_list


static const ParameterDefinition
root_scope_parameters[] = {
	PARAMETER_DEFINITION_END
};


const ScopeDelegate
root_scope_delegate = {
	"root",
	root_scope_parameters,
	{
		root_scope_setup,
		0
	}
};


// --- Implementation ---------------------------------------------------------

bool
root_scope_setup(
	Scope* self
) {
	return 
		Scope_populate(
			self,
			 node_delegate_list,
			 scope_delegate_list,
			 scope_instance_delegate_list
		);
}
