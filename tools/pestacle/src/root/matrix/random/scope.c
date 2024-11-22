#include "root/matrix/random/normal.h"
#include "root/matrix/random/uniform.h"

#include "root/matrix/random/scope.h"


// --- Interface --------------------------------------------------------------

static bool
scope_setup(
	Scope* self
);


static const NodeDelegate*
node_delegate_list[] = {
	&root_matrix_random_normal_node_delegate,
	&root_matrix_random_uniform_node_delegate,
	0
}; // node_delegate_list


static const ParameterDefinition
scope_parameters[] = {
	PARAMETER_DEFINITION_END
};


const ScopeDelegate
root_matrix_random_scope_delegate = {
	"random",
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
