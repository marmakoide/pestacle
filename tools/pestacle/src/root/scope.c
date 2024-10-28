#include "window/scope.h"
#include "root/matrix/scope.h"
#include "root/rgb_surface/scope.h"


// --- Interface --------------------------------------------------------------

static bool
root_scope_setup(
	Scope* self
);


static const ScopeDelegate*
scope_delegate_list[] = {
	&window_scope_delegate,
	0
}; // scope_delegate_list


static const ScopeDelegate*
scope_instance_delegate_list[] = {
	&root_matrix_scope_delegate,
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
			 0,
			 scope_delegate_list,
			 scope_instance_delegate_list
		);
}
