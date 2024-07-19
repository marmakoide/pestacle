#ifndef PESTACLE_EVENT_H
#define PESTACLE_EVENT_H

#include <SDL.h>
#include "array_ops.h"


struct Display {
	uint32_t physical_width;
	uint32_t physical_height;
	uint32_t emulated_width;
	uint32_t emulated_height;
	uint32_t scaling_ratio_inv;
	SDL_Rect visible_area;
}; // struct Display


extern void
display_init(
	struct Display* display,
	uint32_t physical_width,
	uint32_t physical_height,
	uint32_t emulated_width,
	uint32_t emulated_height
);


enum EventType {
	EventType_MouseButtonUp = 0x01,
	EventType_MouseButtonDown,
	EventType_MouseMotion
};


struct MouseButtonEvent {
    enum EventType type;
    uint32_t timestamp;
    uint32_t which;
    uint8_t button;
    uint8_t state;
    uint8_t clicks;
    real_t x;
    real_t y;
}; // struct MouseButtonEvent


struct MouseMotionEvent {
    enum EventType type;
    uint32_t timestamp;
    uint32_t which;
    uint32_t state;
	real_t x;
	real_t y;
	real_t xrel;
	real_t yrel;
}; // struct MouseMotionEvent


union Event {
	enum EventType type;
	struct MouseButtonEvent mouse_button;
	struct MouseMotionEvent mouse_motion;
}; // union Event


extern int
cast_mouse_button_event(
	const struct Display* display,
	const SDL_Event* src,
	union Event* dst
);


extern int
cast_mouse_motion_event(
	const struct Display* display,
	const SDL_Event* src,
	union Event* dst
);


#endif /* PESTACLE_EVENT_H */