#include "nodes/mouse_motion.h"


// --- Interface --------------------------------------------------------------

static int
mouse_motion_node_setup(
	Node* self,
	int width,
	int height
);


static void
mouse_motion_node_destroy(
	Node* self
);


static void
mouse_motion_node_update(
	Node* self	
);


static void
mouse_motion_node_handle_event(
	Node* self,
	const Event* event
);


static const Matrix*
mouse_motion_node_get(
	const Node* self	
);


#define VALUE_PARAMETER 0

static const NodeParameterDefinition
mouse_motion_parameters[] = {
	{
		{ "value", 6 },
		(real_t)1
	}
};


static const NodeDelegate
mouse_motion_node_delegate = {
	{ "mouse-motion", 13 },


	0, 0,

	1, mouse_motion_parameters,

	{
		mouse_motion_node_setup,
		mouse_motion_node_destroy,
		mouse_motion_node_update,
		mouse_motion_node_handle_event,
		mouse_motion_node_get
	},
};


// --- Implementation ---------------------------------------------------------

typedef struct {
	Matrix U;
} MouseMotionData;



static int
mouse_motion_node_setup(
	Node* self,
	int width,
	int height
) {
	MouseMotionData* data = (MouseMotionData*)self->data;

	// Setup the accumulator matrix
	Matrix_init(&(data->U), height, width);
	Matrix_fill(&(data->U), (real_t)0);

	// Job done
	return 1;
}


static void
mouse_motion_node_destroy(
	Node* self
) {
	MouseMotionData* data =
		(MouseMotionData*)self->data;

	Matrix_destroy(&(data->U));
}


static void
mouse_motion_node_update(
	Node* self	
) {
	MouseMotionData* data = (MouseMotionData*)self->data;

	Matrix_fill(&(data->U), (real_t)0);
}


static void
mouse_motion_node_handle_event(
	Node* self,
	const Event* event
) {
	MouseMotionData* data = (MouseMotionData*)self->data;

	switch(event->type) {
		case EventType_MouseMotion:
			Matrix_set_coeff(
				&(data->U),
				(size_t)floorf(event->mouse_motion.y),
				(size_t)floorf(event->mouse_motion.x),
				self->parameters[VALUE_PARAMETER].value
			);
			break;

		default:
			break;
	}
}


static const Matrix*
mouse_motion_node_get(
	const Node* self
) {
	const MouseMotionData* data = (const MouseMotionData*)self->data;
	
	return &(data->U);
}


Node*
mouse_motion_node_new() {
	// Allocation
	Node* ret = node_allocate();
	if (!ret)
		return ret;

	MouseMotionData* data =
		(MouseMotionData*)malloc(sizeof(MouseMotionData));

	// Initialization
	node_init(ret, &mouse_motion_node_delegate, data);

	// Job done
	return ret;
}