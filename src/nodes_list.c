#include "nodes/heat_diffusion.h"
#include "nodes/mouse_motion.h"



const NodeDelegate*
node_delegate_list[] = {
	&heat_diffusion_node_delegate,
	&mouse_motion_node_delegate,
	0 // Last one should be always 0
};