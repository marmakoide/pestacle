#include <pestacle/memory.h>

#include "window/scope.h"
#include "window/display.h"
#include "window_manager.h"


// --- Interface --------------------------------------------------------------

static void
node_update(
	Node* self
);


#define SOURCE_INPUT 0


static const NodeInputDefinition
node_inputs[] = {
	{
		"source",
		true
	},
	NODE_INPUT_DEFINITION_END
}; // display_inputs


static const ParameterDefinition
node_parameters[] = {
	PARAMETER_DEFINITION_END
}; // display_parameters


const NodeDelegate
display_node_delegate = {
	"display",
	false,
	node_inputs,
	node_parameters,
	{
		0,
		0,
		node_update,
		0
	},
}; // display_node_delegate


// --- Implementation ---------------------------------------------------------

static void
node_update(
	Node* self
) {
	Window* window = (Window*)self->delegate_scope->data;

	// Retrieve the input
	SDL_Surface* src =
		Node_output(self->inputs[SOURCE_INPUT]).rgb_surface;

	// Blit to the window surface
	SDL_BlitSurface(src, 0, window->surface, 0);
}
