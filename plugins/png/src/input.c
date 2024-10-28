#include <pestacle/memory.h>

#include "picture.h"
#include "input.h"


// --- Interface --------------------------------------------------------------

static bool
input_node_setup(
	Node* self
);


static void
input_node_destroy(
	Node* self
);


static NodeOutput
input_node_output(
	const Node* self
);


static const NodeInputDefinition
input_inputs[] = {
	NODE_INPUT_DEFINITION_END
};


#define PATH_PARAMETER 0

static const ParameterDefinition
input_parameters[] = {
	{
		ParameterType__string,
		"path",
		{ .string_value = "" }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
input_node_delegate = {
	"input",
	NodeType__rgb_surface,
	input_inputs,
	input_parameters,
	{
		input_node_setup,
		input_node_destroy,
		0,
		input_node_output
	},
};


// --- Implementation ---------------------------------------------------------

static bool
input_node_setup(
	Node* self
) {
	const char* path = self->parameters[PATH_PARAMETER].string_value;

	// Load the picture
	SDL_Surface* rgb_surface = load_png(path);
	if (!rgb_surface)
		return false;

	// Setup node type metadata
	self->metadata.rgb_surface.width = rgb_surface->w;
	self->metadata.rgb_surface.height = rgb_surface->h;

	// Job done
	self->data = rgb_surface;
	return true;
}


static void
input_node_destroy(
	Node* self
) {
	SDL_Surface* rgb_surface = (SDL_Surface*)self->data;
	if (rgb_surface)
		SDL_FreeSurface(rgb_surface);
}


static NodeOutput
input_node_output(
	const Node* self
) {
	NodeOutput ret = { .rgb_surface = (SDL_Surface*)self->data };
	return ret;
}
