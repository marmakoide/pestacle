#include "domain.h"

#include "nodes/gradient_map.h"
#include "nodes/heat_diffusion.h"
#include "nodes/lightness.h"
#include "nodes/matrix_resize.h"
#include "nodes/mouse_motion.h"
#include "nodes/picture.h"
#include "nodes/surface_blend.h"
#include "nodes/surface_resize.h"
#include "nodes/video.h"


// --- Interface --------------------------------------------------------------

static bool
root_domain_setup(
	Domain* self
);


#define NODE_DELEGATE_LIST_END 0


static const NodeDelegate*
node_delegate_list[] = {
	&gradient_map_node_delegate,
	&heat_diffusion_node_delegate,
	&lightness_node_delegate,
	&matrix_resize_node_delegate,
	&mouse_motion_node_delegate,
	&picture_node_delegate,
	&surface_blend_node_delegate,
	&surface_resize_node_delegate,
	&video_node_delegate,
	NODE_DELEGATE_LIST_END
}; // node_delegate_list


static const ParameterDefinition
root_domain_parameters[] = {
	{ ParameterType__last }
};


const DomainDelegate root_domain_delegate = {
	{ "root", 5 },
	root_domain_parameters,
	{
		root_domain_setup,
		0
	}
};


// --- Implementation ---------------------------------------------------------

bool
root_domain_setup(
	Domain* self
) {
	const NodeDelegate** delegate_ptr = node_delegate_list;
	for( ; *delegate_ptr != NODE_DELEGATE_LIST_END; ++delegate_ptr)
		if (!Domain_add_node_delegate(self, *delegate_ptr))
			return false;

	return true;	
}