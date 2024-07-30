#include "nodes/gradient_map.h"
#include "nodes/heat_diffusion.h"
#include "nodes/mouse_motion.h"
#include "nodes/picture.h"
#include "nodes/surface_blend.h"


static const NodeDelegate*
node_delegate_list[] = {
	&gradient_map_node_delegate,
	&heat_diffusion_node_delegate,
	&mouse_motion_node_delegate,
	&picture_node_delegate,
	&surface_blend_node_delegate,
	0 // Last one should be always 0
};


const NodeDelegate*
get_node_delegate_by_name(const String* name) {
	const NodeDelegate** delegate_ptr = node_delegate_list;
	for( ; *delegate_ptr != 0; ++delegate_ptr)
		if (String_equals(name, &((*delegate_ptr)->name)))
			return *delegate_ptr;

	return 0;
}