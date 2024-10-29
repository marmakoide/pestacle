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

static void
mouse_motion_on_event(
	void* listener,
	SDL_Event* event
) {
	Node* node = (Node*)listener;
	Matrix* accumulator = (Matrix*)node->data;

	switch(event->type) {
		case SDL_MOUSEMOTION:
			Matrix_set_coeff(
				accumulator,
				event->motion.y,
				event->motion.x,
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
	Matrix* accumulator = (Matrix*)checked_malloc(sizeof(Matrix));

	// Setup node type metadata
	int w, h;
	SDL_GetWindowSize(window->window, &w, &h);
	self->metadata.matrix.width = w;
	self->metadata.matrix.height = h;

	// Setup the accumulator matrix
	Matrix_init(accumulator, h, w);
	Matrix_fill(accumulator, (real_t)0);

	// Register to windows events
	Window_add_event_listener(window, self, mouse_motion_on_event);

	// Job done
	self->data = accumulator;
	return true;
}


static void
node_destroy(
	Node* self
) {
	Matrix* accumulator = (Matrix*)self->data;
	if (accumulator != 0) {
		Matrix_destroy(accumulator);
		free(accumulator);
	}
}


static void
node_update(
	Node* self
) {
	// Retrieve the accumulator
	Matrix* accumulator = (Matrix*)self->data;

	// Empty the accumulator
	Matrix_fill(accumulator, (real_t)0);

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
		Matrix_set_coeff(
			accumulator,
			mouse_y - win_y,
			mouse_x - win_x,
			self->parameters[VALUE_PARAMETER].real_value
		);
}


static NodeOutput
node_output(
	const Node* self
) {
	Matrix* accumulator = (Matrix*)self->data;
	NodeOutput ret = { .matrix = accumulator };
	return ret;
}
