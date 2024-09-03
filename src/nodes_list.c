#include "nodes/gradient_map.h"
#include "nodes/heat_diffusion.h"
#include "nodes/lightness.h"
#include "nodes/matrix_resize.h"
#include "nodes/mouse_motion.h"
#include "nodes/picture.h"
#include "nodes/surface_blend.h"
#include "nodes/surface_resize.h"


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
	NODE_DELEGATE_LIST_END
};


const NodeDelegate*
get_node_delegate_by_name(const String* name) {
	const NodeDelegate** delegate_ptr = node_delegate_list;
	for( ; *delegate_ptr != NODE_DELEGATE_LIST_END; ++delegate_ptr)
		if (String_equals(name, &((*delegate_ptr)->name)))
			return *delegate_ptr;

	return 0;
}