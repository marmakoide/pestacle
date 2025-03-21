#include "root/matrix/resample/nearest.h"

#include "root/matrix/resample/scope.h"


// --- Interface --------------------------------------------------------------

static bool
scope_setup(
	Scope* self
);


static const NodeDelegate*
node_delegate_list[] = {
	&root_matrix_resample_nearest_node_delegate,
	0
}; // node_delegate_list


static const ParameterDefinition
scope_parameters[] = {
	PARAMETER_DEFINITION_END
};


const ScopeDelegate
root_matrix_resample_scope_delegate = {
	"resample",
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
