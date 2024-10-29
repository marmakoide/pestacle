#include "root/matrix/gradient_map.h"
#include "root/matrix/heat_diffusion.h"
#include "root/matrix/resize.h"

#include "root/matrix/scope.h"


// --- Interface --------------------------------------------------------------

static bool
scope_setup(
	Scope* self
);


static const NodeDelegate*
node_delegate_list[] = {
	&root_matrix_gradient_map_node_delegate,
	&root_matrix_heat_diffusion_node_delegate,
	&root_matrix_resize_node_delegate,
	0
}; // node_delegate_list


static const ParameterDefinition
scope_parameters[] = {
	PARAMETER_DEFINITION_END
};


const ScopeDelegate
root_matrix_scope_delegate = {
	"matrix",
	scope_parameters,
	{
		scope_setup,
		0
	}
};


// --- Implementation ---------------------------------------------------------

bool
scope_setup(
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