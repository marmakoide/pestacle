#include "nodes/picture.h"
#include "picture.h"


// --- Interface --------------------------------------------------------------

static bool
picture_node_setup(
	Node* self
);


static void
picture_node_destroy(
	Node* self
);


static NodeOutput
picture_node_output(
	const Node* self
);


static const NodeInputDefinition
picture_inputs[] = {
	NODE_INPUT_DEFINITION_END
};


#define PATH_PARAMETER 0

static const ParameterDefinition
picture_parameters[] = {
	{
		ParameterType__string,
		{ "path", 5 },
		{ .string_value = { "", 1 } }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
picture_node_delegate = {
	{ "picture", 8 },
	NodeType__rgb_surface,
	picture_inputs,
	picture_parameters,
	{
		picture_node_setup,
		picture_node_destroy,
		0,
		picture_node_output
	},
};


// --- Implementation ---------------------------------------------------------

static bool
picture_node_setup(
	Node* self
) {
	const String* path = &(self->parameters[PATH_PARAMETER].string_value);

	// Load the picture
	SDL_Surface* rgb_surface = load_png(path->data);
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
picture_node_destroy(
	Node* self
) {
	SDL_Surface* rgb_surface = (SDL_Surface*)self->data;
	if (rgb_surface)
		SDL_FreeSurface(rgb_surface);
}


static NodeOutput
picture_node_output(
	const Node* self
) {
	NodeOutput ret = { .rgb_surface = (SDL_Surface*)self->data };
	return ret;
}
