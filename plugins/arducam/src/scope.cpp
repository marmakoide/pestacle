#include "scope.h"
#include "tof_camera.h"


// --- Interface --------------------------------------------------------------

static bool
scope_setup(
	Scope* self
);


static const NodeDelegate*
node_delegate_list[] = {
	&arducam_tof_camera_node_delegate,
	0
}; // node_delegate_list


static const ParameterDefinition
scope_parameters[] = {
	PARAMETER_DEFINITION_END
};


static const ScopeDelegate
scope_delegate = {
	"arducam",
	scope_parameters,
	{
		scope_setup,
		0
	}
}; // scope delegate


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


// --- Plugin entry point -----------------------------------------------------

const ScopeDelegate*
get_scope_delegate() {
	return &scope_delegate;
}
