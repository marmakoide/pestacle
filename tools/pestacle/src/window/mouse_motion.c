#include <assert.h>
#include <pestacle/memory.h>

#include "window/scope.h"
#include "window/mouse_motion.h"
#include "window_manager.h"


// --- Interface --------------------------------------------------------------

static bool
node_setup(
	Node* self
);


static void
node_destroy(
	Node* self
);


static void
node_update(
	Node* self
);


static NodeOutput
node_output(
	const Node* self
);


static const NodeInputDefinition
node_inputs[] = {
	NODE_INPUT_DEFINITION_END
};


#define VALUE_PARAMETER  0

static const ParameterDefinition
node_parameters[] = {
	{
		ParameterType__real,
		"value",
		{ .real_value = (real_t)1 }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
mouse_motion_node_delegate = {
	"mouse-motion",
	NodeType__matrix,
	node_inputs,
	node_parameters,
	{
		node_setup,
		node_destroy,
		node_update,
		node_output
	},
};


// --- Implementation ---------------------------------------------------------

typedef struct {
	int i;
	Matrix accumulator[2];
} MouseMotion;


static void
MouseMotion_init(
	MouseMotion* self,
	size_t width,
	size_t height
) {
	self->i = 0;
	for(int i = 0; i < 2; ++i) {
		Matrix_init(&(self->accumulator[i]), height, width);
		Matrix_fill(&(self->accumulator[i]), (real_t)0);
	}
}


static void
MouseMotion_destroy(
	MouseMotion* self
) {
	for(int i = 0; i < 2; ++i)
		Matrix_destroy(&(self->accumulator[i]));
}


static void
MouseMotion_update(
	MouseMotion* self,
	int x,
	int y,
	real_t value
) {
	Matrix_set_coeff(&(self->accumulator[self->i]), y, x, value);
}


static void
MouseMotion_swap(
	MouseMotion* self
) {
	self->i = 1 - self->i;
	Matrix_fill(&(self->accumulator[self->i]), (real_t)0);
}


static Matrix*
MouseMotion_get(
	MouseMotion* self
) {
	return &(self->accumulator[1 - self->i]);
}


static void
mouse_motion_on_event(
	void* listener,
	SDL_Event* event
) {
	Node* node = (Node*)listener;
	MouseMotion* mouse_motion = (MouseMotion*)node->data;

	switch(event->type) {
		case SDL_MOUSEMOTION:
			MouseMotion_update(
				mouse_motion, 
				event->motion.x,
				event->motion.y,
				node->parameters[VALUE_PARAMETER].real_value
			);
			break;

		default:
			break;
	}
}


static bool
node_setup(
	Node* self
) {
	assert(self->delegate_scope->data);

	// Retrieve the window
	Window* window = (Window*)self->delegate_scope->data;

	// Allocate
	MouseMotion* mouse_motion = (MouseMotion*)checked_malloc(sizeof(MouseMotion));

	// Setup node type metadata
	int w, h;
	SDL_GetWindowSize(window->window, &w, &h);
	self->metadata.matrix.width = w;
	self->metadata.matrix.height = h;

	// Initialize
	MouseMotion_init(mouse_motion, w, h);

	// Register to windows events
	Window_add_event_listener(window, self, mouse_motion_on_event);

	// Job done
	self->data = mouse_motion;
	return true;
}


static void
node_destroy(
	Node* self
) {
	MouseMotion* mouse_motion = (MouseMotion*)self->data;
	if (mouse_motion) {
		MouseMotion_destroy(mouse_motion);
		free(mouse_motion);
	}
}


static void
node_update(
	Node* self
) {
	MouseMotion* mouse_motion = (MouseMotion*)self->data;

	// Swap
	MouseMotion_swap(mouse_motion);

	// Retrieve the window
	Window* window = (Window*)self->delegate_scope->data;

	// Get the window position and size
	int win_x, win_y;
	SDL_GetWindowPosition(window->window, &win_x, &win_y);

	int win_w, win_h;
	SDL_GetWindowSize(window->window, &win_w, &win_h);

	// Get the mouse position
	int mouse_x, mouse_y;
	SDL_GetGlobalMouseState(&mouse_x, &mouse_y);

	// If the mouse is inside the window, mark the pixel under it
	if ((mouse_x >= win_x) && (mouse_y >= win_y) && (mouse_x < win_x + win_w) && (mouse_y < win_y + win_h))
		MouseMotion_update(
			mouse_motion,
			mouse_x - win_x,
			mouse_y - win_y,
			self->parameters[VALUE_PARAMETER].real_value
		);
}


static NodeOutput
node_output(
	const Node* self
) {
	MouseMotion* mouse_motion = (MouseMotion*)self->data;
	NodeOutput ret = {  .matrix = MouseMotion_get(mouse_motion) };
	return ret;
}
