#include "scope.h"
#include "tof_camera.h"


// --- Interface --------------------------------------------------------------

static bool
scope_setup(
	Scope* self
);


#define NODE_DELEGATE_LIST_END 0

static const NodeDelegate*
node_delegate_list[] = {
	&tof_camera_node_delegate,
	NODE_DELEGATE_LIST_END
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
	const NodeDelegate** node_delegate_ptr = node_delegate_list;
	for( ; *node_delegate_ptr != NODE_DELEGATE_LIST_END; ++node_delegate_ptr)
		if (!Scope_add_node_delegate(self, *node_delegate_ptr))
			return false;

	return true;
}


// --- Plugin entry point -----------------------------------------------------

const ScopeDelegate*
get_scope_delegate() {
	return &scope_delegate;
}
