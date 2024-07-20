#include "sources/mouse_motion.h"


struct MouseMotionData {
	struct Matrix U;
};



int
mouse_motion_source_setup(
	struct Source* self,
	int width,
	int height
) {
	struct MouseMotionData* data =
		(struct MouseMotionData*)self->data;

	// Setup the accumulator matrix
	Matrix_init(&(data->U), height, width);
	Matrix_fill(&(data->U), (real_t)0);

	// Job done
	return 1;
}


void
mouse_motion_source_destroy(
	struct Source* self
) {
	struct MouseMotionData* data =
		(struct MouseMotionData*)self->data;

	Matrix_destroy(&(data->U));
}


void
mouse_motion_source_update(
	struct Source* self	
) {
	struct MouseMotionData* data =
		(struct MouseMotionData*)self->data;

	Matrix_fill(&(data->U), (real_t)0);
}


void
mouse_motion_source_handle_event(
	struct Source* self,
	const union Event* event
) {
	struct MouseMotionData* data =
		(struct MouseMotionData*)self->data;

	switch(event->type) {
		case EventType_MouseMotion:
			Matrix_set_coeff(
				&(data->U),
				(size_t)floorf(event->mouse_motion.y),
				(size_t)floorf(event->mouse_motion.x),
				(real_t)1);
			break;

		default:
			break;
	}
}


const struct Matrix*
mouse_motion_source_get(
	const struct Source* self	
) {
	const struct MouseMotionData* data =
		(const struct MouseMotionData*)self->data;
	
	return &(data->U);
}


static const struct SourceDelegate
mouse_motion_source_delegate = {
	"mouse-motion",
	mouse_motion_source_setup,
	mouse_motion_source_destroy,
	mouse_motion_source_update,
	mouse_motion_source_handle_event,
	mouse_motion_source_get
};



struct Source*
mouse_motion_source_new() {
	// Allocation
	struct Source* ret = source_allocate();
	if (!ret)
		return ret;

	struct MouseMotionData* data =
		(struct MouseMotionData*)malloc(sizeof(struct MouseMotionData));

	// Delegate setup
	ret->data = data;
	ret->delegate = &mouse_motion_source_delegate;

	// Job done
	return ret;
}