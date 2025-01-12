#include <pestacle/memory.h>

#include "window/scope.h"
#include "window/display.h"
#include "window_manager.h"


// --- Interface --------------------------------------------------------------

static bool
node_setup(
	Node* self
);


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
	node_inputs,
	node_parameters,
	{
		node_setup,
		0,
		node_update,
		0
	},
}; // display_node_delegate


// --- Implementation ---------------------------------------------------------

static bool
node_setup(
	Node* self
) {
	Window* window = (Window*)self->delegate_scope->data;

	// Retrieve input data descriptor
	const DataDescriptor* in_descriptor =
		&(self->inputs[SOURCE_INPUT]->out_descriptor);

	// Check input descriptor validity
	if
		((in_descriptor->matrix.width != (size_t)window->surface->w) ||
		 (in_descriptor->matrix.height != (size_t)window->surface->h) ) {
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"input should have the same dimension as the display"
			);
		return false;
	}

	return true;
}


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
