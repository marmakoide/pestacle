#include <assert.h>

#include <pestacle/macros.h>
#include <pestacle/memory.h>

#include "scopes/window.h"
#include "window_manager.h"


// --- display Node interface & implementation --------------------------------

static void
display_node_update(
	Node* self
);


#define SOURCE_INPUT 0


static const NodeInputDefinition
display_inputs[] = {
	{
		NodeType__rgb_surface,
		{ "source", 7 }
	},
	NODE_INPUT_DEFINITION_END
}; // display_inputs


static const ParameterDefinition
display_parameters[] = {
	PARAMETER_DEFINITION_END
}; // display_parameters


const NodeDelegate
display_node_delegate = {
	{ "display", 8 },
	NodeType__void,
	display_inputs,
	display_parameters,
	{
		0,
		0,
		display_node_update,
		0
	},
}; // display_node_delegate


static void
display_node_update(
	Node* self
) {
	Window* window = (Window*)self->delegate_scope->data;

	// Retrieve the input
	SDL_Surface* src =
		Node_output(self->inputs[SOURCE_INPUT]).rgb_surface;

	// Blit to the window surface
	SDL_BlitSurface(src, 0, window->surface, 0);
}


// --- mouse-motion Node interface & implementation ---------------------------

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


static NodeOutput
mouse_motion_node_output(
	const Node* self
);


static const NodeInputDefinition
mouse_motion_inputs[] = {
	NODE_INPUT_DEFINITION_END
};


#define VALUE_PARAMETER  0

static const ParameterDefinition
mouse_motion_parameters[] = {
	{
		ParameterType__real,
		{ "value", 6 },
		{ .real_value = (real_t)1 }
	},
	PARAMETER_DEFINITION_END
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
		mouse_motion_node_output
	},
};


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
mouse_motion_node_setup(
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
mouse_motion_node_destroy(
	Node* self
) {
	Matrix* accumulator = (Matrix*)self->data;
	if (accumulator != 0) {
		Matrix_destroy(accumulator);
		free(accumulator);
	}
}


static void
mouse_motion_node_update(
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
mouse_motion_node_output(
	const Node* self
) {
	Matrix* accumulator = (Matrix*)self->data;
	NodeOutput ret = { .matrix = accumulator };
	return ret;
}


// --- window Scope interface ------------------------------------------------

static bool
window_scope_setup(
	Scope* self
);


static void
window_scope_destroy(
	Scope* self
);


// --- window Scope implementation -------------------------------------------

#define WIDTH_PARAMETER    0
#define HEIGHT_PARAMETER   1
#define TITLE_PARAMETER    2
#define BORDERED_PARAMETER 3

static const ParameterDefinition
window_scope_parameters[] = {
	{
		ParameterType__integer,
		{ "width", 6 },
		{ .int64_value = 320 }
	},
	{
		ParameterType__integer,
		{ "height", 7 },
		{ .int64_value = 240 }
	},
	{
		ParameterType__string,
		{ "title", 6 },
		{ .string_value = { "pestacle", 9 } }
	},
	{
		ParameterType__bool,
		{ "bordered", 9 },
		{ .bool_value = true }
	},
	PARAMETER_DEFINITION_END
}; // window_scope_parameters


const ScopeDelegate
window_scope_delegate = {
	{ "window", 7 },
	window_scope_parameters,
	{
		window_scope_setup,
		window_scope_destroy
	},
}; // window_scope_delegate


static bool
window_scope_setup(
	Scope* self
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;
	const String* title = &(self->parameters[TITLE_PARAMETER].string_value);
	bool bordered = self->parameters[BORDERED_PARAMETER].bool_value;

	// Retried the window manager
	WindowManager* window_manager =
		(WindowManager*)self->delegate_scope->data;

	// Create the window
	Window* window = 
		WindowManager_add_window(
			window_manager,
			title->data,
			width,
			height
		);

	if (!window)
		goto failure;

	Window_set_bordered(window, bordered);

	// Keep a pointer on the window object
	self->data = window;

	// Add the 'display' node
	Node* display_node = 
		Node_new(&(display_node_delegate.name), &display_node_delegate, self);

	if ((!display_node) || (!Scope_add_node(self, display_node)))
		goto failure;

	// Add the 'mouse-motion' delegate
	Scope_add_node_delegate(self, &mouse_motion_node_delegate);

	// Job done
	return true;

failure:
	if (window)
		WindowManager_remove_window(window_manager, window);

	return false;
}


static void
window_scope_destroy(
	ATTRIBUTE_UNUSED Scope* self
) {
	
}
