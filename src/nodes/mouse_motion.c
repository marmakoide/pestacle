#include "nodes/mouse_motion.h"
#include "memory.h"


// --- Interface --------------------------------------------------------------

static bool
mouse_motion_node_setup(
	Node* self
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


static NodeOutput
mouse_motion_node_output(
	const Node* self
);


static const NodeInputDefinition
mouse_motion_inputs[] = {
	{ NodeType__last }
};


#define WIDTH_PARAMETER  0
#define HEIGHT_PARAMETER 1
#define VALUE_PARAMETER  2

static const NodeParameterDefinition
mouse_motion_parameters[] = {
	{
		NodeParameterType__integer,
		{ "width", 6 },
		{ .int64_value = 32 }
	},
	{
		NodeParameterType__integer,
		{ "height", 7 },
		{ .int64_value = 32 }
	},
	{
		NodeParameterType__real,
		{ "value", 6 },
		{ .real_value = (real_t)1 }
	},
	{ NodeParameterType__last }
};


const NodeDelegate
mouse_motion_node_delegate = {
	{ "mouse-motion", 13 },
	NodeType__matrix,
	mouse_motion_inputs,
	mouse_motion_parameters,
	{
		mouse_motion_node_setup,
		mouse_motion_node_destroy,
		mouse_motion_node_update,
		mouse_motion_node_handle_event,
		mouse_motion_node_output
	},
};


// --- Implementation ---------------------------------------------------------

typedef struct {
	Matrix accumulator;
} MouseMotionData;



static bool
mouse_motion_node_setup(
	Node* self
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;

	// Allocate
	MouseMotionData* data =
		(MouseMotionData*)checked_malloc(sizeof(MouseMotionData));

	if (!data)
		return false;

	// Setup the accumulator matrix
	Matrix_init(&(data->accumulator), height, width);
	Matrix_fill(&(data->accumulator), (real_t)0);

	// Job done
	self->data = data;
	return true;
}


static void
mouse_motion_node_destroy(
	Node* self
) {
	MouseMotionData* data = (MouseMotionData*)self->data;

	if (data != 0) {
		Matrix_destroy(&(data->accumulator));
		free(data);
	}
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
				self->parameters[VALUE_PARAMETER].real_value
			);
			break;

		default:
			break;
	}
}


static NodeOutput
mouse_motion_node_output(
	const Node* self
) {
	const MouseMotionData* data = (const MouseMotionData*)self->data;

	NodeOutput ret = { .matrix = &(data->accumulator) };
	return ret;
}
