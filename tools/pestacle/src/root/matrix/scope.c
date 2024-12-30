#include "root/matrix/auto_threshold.h"
#include "root/matrix/blend.h"
#include "root/matrix/gaussian.h"
#include "root/matrix/gradient_map.h"
#include "root/matrix/heaviside.h"
#include "root/matrix/heat_diffusion.h"
#include "root/matrix/product.h"
#include "root/matrix/resize.h"
#include "root/matrix/scale.h"
#include "root/matrix/shift.h"
#include "root/matrix/soft_equal.h"
#include "root/matrix/stddev.h"

#include "root/matrix/random/scope.h"

#include "root/matrix/scope.h"


// --- Interface --------------------------------------------------------------

static bool
scope_setup(
	Scope* self
);


static const NodeDelegate*
node_delegate_list[] = {
	&root_matrix_auto_threshold_node_delegate,
	&root_matrix_blend_node_delegate,
	&root_matrix_gaussian_node_delegate,
	&root_matrix_gradient_map_node_delegate,
	&root_matrix_heaviside_node_delegate,
	&root_matrix_heat_diffusion_node_delegate,
	&root_matrix_product_node_delegate,
	&root_matrix_resize_node_delegate,
	&root_matrix_scale_node_delegate,
	&root_matrix_shift_node_delegate,
	&root_matrix_soft_equal_node_delegate,
	&root_matrix_stddev_node_delegate,
	0
}; // node_delegate_list


static const ScopeDelegate*
scope_instance_delegate_list[] = {
	&root_matrix_random_scope_delegate,
	0
}; // scope_instance_delegate_list


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
			 scope_instance_delegate_list
		);
}
