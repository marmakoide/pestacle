#include "scopes/window.h"
#include "memory.h"


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
	{ NodeType__last }
}; // display_inputs


static const ParameterDefinition
display_parameters[] = {
	{ ParameterType__last }
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


// --- display Node implementation --------------------------------------------

static void
display_node_update(
	Node* self
) {
	Window* window = (Window*)self->data;

	// Retrieve the input
	SDL_Surface* src =
		Node_output(self->inputs[SOURCE_INPUT]).rgb_surface;

	// Blit to the window surface
	SDL_BlitSurface(src, 0, window->surface, 0);
}


// --- window Scope interface ------------------------------------------------

static bool
window_scope_setup(
	Scope* self,
	WindowManager* window_manager
);


static void
window_scope_destroy(
	Scope* self
);


// --- window Scope implementation -------------------------------------------


#define WIDTH_PARAMETER  0
#define HEIGHT_PARAMETER 1
#define TITLE_PARAMETER  2

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
	{ ParameterType__last }
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
	Scope* self,
	WindowManager* window_manager
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;
	const String* title = &(self->parameters[TITLE_PARAMETER].string_value);

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

	// Add the display node
	Node* display_node = Node_new(
		&(display_node_delegate.name),
		&display_node_delegate
	);
	display_node->data = window;

	if (!display_node)
		goto failure;

	if (!Scope_add_node(self, display_node))
		goto failure;

	// Job done
	return true;

failure:
	if (window)
		WindowManager_remove_window(window_manager, window);

	return false;
}


static void
window_scope_destroy(
	Scope* self
) {
	
}