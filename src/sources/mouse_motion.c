#include "sources/mouse_motion.h"


typedef struct {
	Matrix U;
	real_t value;
} MouseMotionData;



int
mouse_motion_source_setup(
	Source* self,
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


void
mouse_motion_source_destroy(
	Source* self
) {
	MouseMotionData* data =
		(MouseMotionData*)self->data;

	Matrix_destroy(&(data->U));
}


void
mouse_motion_source_update(
	Source* self	
) {
	MouseMotionData* data = (MouseMotionData*)self->data;

	Matrix_fill(&(data->U), (real_t)0);
}


void
mouse_motion_source_handle_event(
	Source* self,
	const Event* event
) {
	MouseMotionData* data = (MouseMotionData*)self->data;

	switch(event->type) {
		case EventType_MouseMotion:
			Matrix_set_coeff(
				&(data->U),
				(size_t)floorf(event->mouse_motion.y),
				(size_t)floorf(event->mouse_motion.x),
				data->value
			);
			break;

		default:
			break;
	}
}


const Matrix*
mouse_motion_source_get(
	const Source* self	
) {
	const MouseMotionData* data = (const MouseMotionData*)self->data;
	
	return &(data->U);
}


static const SourceDelegate
mouse_motion_source_delegate = {
	{ "mouse-motion", 13 },


	0, 0,

	{
		mouse_motion_source_setup,
		mouse_motion_source_destroy,
		mouse_motion_source_update,
		mouse_motion_source_handle_event,
		mouse_motion_source_get
	},
};



Source*
mouse_motion_source_new(
	real_t value
) {
	// Allocation
	Source* ret = source_allocate();
	if (!ret)
		return ret;

	MouseMotionData* data =
		(MouseMotionData*)malloc(sizeof(MouseMotionData));
	data->value = value;

	// Initialization
	source_init(ret, &mouse_motion_source_delegate, data);

	// Job done
	return ret;
}