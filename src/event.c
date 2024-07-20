#include <assert.h>
#include "event.h"


void
display_init(
	Display* self,
	uint32_t physical_width,
	uint32_t physical_height,
	uint32_t emulated_width,
	uint32_t emulated_height
) {
	assert(self != 0);
	assert(physical_width >= emulated_width);
	assert(physical_height >= emulated_height);

	self->physical_width = physical_width;
	self->physical_height = physical_height;
	self->emulated_width = emulated_width;
	self->emulated_height = emulated_height;

	// Compute the maximum integer inverse scaling ratio
	real_t x_scaling_ratio_inv =
		((real_t)(physical_width)) / ((real_t)(emulated_width));
	real_t y_scaling_ratio_inv =
		((real_t)(physical_height)) / ((real_t)(emulated_height));

	self->scaling_ratio_inv =
		(uint32_t)floorf(fminf(x_scaling_ratio_inv, y_scaling_ratio_inv));

	// Compute the visible area on the physical display
	self->visible_area.w = self->scaling_ratio_inv * self->emulated_width;
	self->visible_area.h = self->scaling_ratio_inv * self->emulated_height;
	self->visible_area.x = (physical_width - self->visible_area.w) / 2;
	self->visible_area.y = (physical_height - self->visible_area.h) / 2;
}


static int
point_in_rect(
	const SDL_Rect* rect,
	int x,
	int y) {
	return
		(x >= (rect->x)) &&
		(y >= (rect->y)) &&
		(x < (rect->x + rect->w)) &&
		(y < (rect->y + rect->h));
}


int
cast_mouse_button_event(
	const Display* display,
	const SDL_Event* src,
	Event* dst
) {	
	assert(display != 0);
	assert(src != 0);
	assert(dst != 0);

	if (!point_in_rect(&(display->visible_area), src->button.x, src->button.y))
		return 0;

	switch(src->type) {
		case SDL_MOUSEBUTTONUP:
			dst->mouse_button.type = EventType_MouseButtonUp;
			break;

		case SDL_MOUSEBUTTONDOWN:
			dst->mouse_button.type = EventType_MouseButtonDown;
			break;

		default:
			assert(0);
	}
	
	dst->mouse_button.timestamp = src->button.timestamp;
	dst->mouse_button.which = src->button.which;
	dst->mouse_button.button = src->button.button;
	dst->mouse_button.state = src->button.state;
	dst->mouse_button.clicks = src->button.clicks;
	dst->mouse_button.x = ((real_t)(src->button.x - display->visible_area.x)) / display->scaling_ratio_inv;
	dst->mouse_button.y = ((real_t)(src->button.y - display->visible_area.y)) / display->scaling_ratio_inv;

	return 1;
}


int
cast_mouse_motion_event(
	const Display* display,
	const SDL_Event* src,
	Event* dst
) {
	assert(display != 0);
	assert(src != 0);
	assert(dst != 0);

	if (!point_in_rect(&(display->visible_area), src->motion.x, src->motion.y))
		return 0;
	
	dst->mouse_motion.type = EventType_MouseMotion;
	dst->mouse_motion.timestamp = src->motion.timestamp;
	dst->mouse_motion.which = src->motion.which;
	dst->mouse_motion.state = src->motion.state;
	dst->mouse_motion.x = ((real_t)(src->motion.x - display->visible_area.x)) / display->scaling_ratio_inv;
	dst->mouse_motion.y = ((real_t)(src->motion.y - display->visible_area.y)) / display->scaling_ratio_inv;
	dst->mouse_motion.xrel = ((real_t)src->motion.xrel) / display->scaling_ratio_inv;
	dst->mouse_motion.yrel = ((real_t)src->motion.yrel) / display->scaling_ratio_inv;

	return 1;
}