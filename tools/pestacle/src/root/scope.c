#include "window/scope.h"
#include "root/matrix/scope.h"
#include "root/rgb_surface/scope.h"


// --- Interface --------------------------------------------------------------

static bool
scope_setup(
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
}; // scope_instance_delegate_list


static const ParameterDefinition
scope_parameters[] = {
	PARAMETER_DEFINITION_END
};


const ScopeDelegate
root_scope_delegate = {
	"root",
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
			 0,
			 scope_delegate_list,
			 scope_instance_delegate_list
		);
}
