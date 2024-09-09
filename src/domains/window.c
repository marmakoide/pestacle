#include "domains/window.h"
#include "memory.h"


// --- display Node interface -------------------------------------------------

static NodeOutput
display_node_output(
	const Node* self
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
		0,
		0,
		display_node_output
	},
}; // display_node_delegate


// --- display Node implementation --------------------------------------------

static NodeOutput
display_node_output(
	const Node* self
) {
	NodeOutput ret = { .rgb_surface = (SDL_Surface*)self->data };
	return ret;
}


// --- window Domain interface ------------------------------------------------

static bool
window_domain_setup(
	Domain* self,
	WindowManager* window_manager
);


static void
window_domain_destroy(
	Domain* self
);


// --- window Domain implementation -------------------------------------------


#define WIDTH_PARAMETER  0
#define HEIGHT_PARAMETER 1
#define TITLE_PARAMETER  2

static const ParameterDefinition
window_domain_parameters[] = {
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
}; // window_domain_parameters


const DomainDelegate
window_domain_delegate = {
	{ "window", 7 },
	window_domain_parameters,
	{
		window_domain_setup,
		window_domain_destroy
	},
}; // window_domain_delegate


static bool
window_domain_setup(
	Domain* self,
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

	self->data = window;

	// Add the display node
	Node* display_node = Node_new(
		&(display_node_delegate.name),
		& display_node_delegate
	);

	if (!display_node)
		goto failure;

	if (!Domain_add_node(self, display_node))
		goto failure;

	// Job done
	return true;

failure:
	if (window)
		WindowManager_remove_window(window_manager, window);

	return false;
}


static void
window_domain_destroy(
	Domain* self
) {
	
}