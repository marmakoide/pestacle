#include <pestacle/memory.h>

#include "picture.h"
#include "load.h"


// --- Interface --------------------------------------------------------------

static bool
node_setup(
	Node* self
);


static void
node_destroy(
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


#define PATH_PARAMETER 0

static const ParameterDefinition
node_parameters[] = {
	{
		ParameterType__string,
		"path",
		{ .string_value = "" }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
png_load_node_delegate = {
	"load",
	node_inputs,
	node_parameters,
	{
		node_setup,
		node_destroy,
		0,
		node_output
	},
};


// --- Implementation ---------------------------------------------------------

static bool
node_setup(
	Node* self
) {
	const char* path = self->parameters[PATH_PARAMETER].string_value;

	// Load the picture
	SDL_Surface* rgb_surface = load_png(path);
	if (!rgb_surface)
		return false;

	// Setup output descriptor
	DataDescriptor_set_as_rgb_surface(
		&(self->out_descriptor), rgb_surface->w, rgb_surface->h
	);

	// Job done
	self->data = rgb_surface;
	return true;
}


static void
node_destroy(
	Node* self
) {
	SDL_Surface* rgb_surface = (SDL_Surface*)self->data;
	if (rgb_surface)
		SDL_FreeSurface(rgb_surface);
}


static NodeOutput
node_output(
	const Node* self
) {
	NodeOutput ret = { .rgb_surface = (SDL_Surface*)self->data };
	return ret;
}
