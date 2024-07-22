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


const NodeDelegate
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
	Matrix accumulator;
} MouseMotionData;



static int
mouse_motion_node_setup(
	Node* self,
	int width,
	int height
) {
	// Allocate
	MouseMotionData* data =
		(MouseMotionData*)malloc(sizeof(MouseMotionData));

	if (!data)
		return 0;

	// Setup the accumulator matrix
	Matrix_init(&(data->accumulator), height, width);
	Matrix_fill(&(data->accumulator), (real_t)0);

	// Job done
	self->data = data;
	return 1;
}


static void
mouse_motion_node_destroy(
	Node* self
) {
	MouseMotionData* data = (MouseMotionData*)self->data;

	Matrix_destroy(&(data->accumulator));

	free(data);
}


static void
mouse_motion_node_update(
	Node* self	
) {
	MouseMotionData* data = (MouseMotionData*)self->data;

	Matrix_fill(&(data->accumulator), (real_t)0);
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
				&(data->accumulator),
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
	
	return &(data->accumulator);
}
