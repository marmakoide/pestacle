#include <pestacle/memory.h>

#include "window/scope.h"
#include "window/display.h"
#include "window_manager.h"


// --- Interface --------------------------------------------------------------

static void
display_node_update(
	Node* self
);


#define SOURCE_INPUT 0


static const NodeInputDefinition
display_inputs[] = {
	{
		NodeType__rgb_surface,
		"source"
	},
	NODE_INPUT_DEFINITION_END
}; // display_inputs


static const ParameterDefinition
display_parameters[] = {
	PARAMETER_DEFINITION_END
}; // display_parameters


const NodeDelegate
display_node_delegate = {
	"display",
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


// --- Implementation ---------------------------------------------------------

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
