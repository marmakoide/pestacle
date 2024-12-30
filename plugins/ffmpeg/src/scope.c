#include "scope.h"
#include "load.h"


// --- Interface --------------------------------------------------------------

static bool
scope_setup(
	Scope* self
);


static const NodeDelegate*
node_delegate_list[] = {
	&ffmpeg_load_node_delegate,
	0
}; // node_delegate_list


static const ParameterDefinition
scope_parameters[] = {
	PARAMETER_DEFINITION_END
};


static const ScopeDelegate
scope_delegate = {
	"ffmpeg",
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


// --- Plugin entry point -----------------------------------------------------

const ScopeDelegate*
get_scope_delegate() {
	return &scope_delegate;
}
