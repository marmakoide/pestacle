#include <pestacle/memory.h>

#include "root/surface_overlay.h"


// --- Interface --------------------------------------------------------------

static bool
surface_overlay_node_setup(
	Node* self
);


static void
surface_overlay_node_destroy(
	Node* self
);


static void
surface_overlay_node_update(
	Node* self
);


static NodeOutput
surface_overlay_node_output(
	const Node* self
);


#define SOURCE_A_INPUT 0
#define SOURCE_B_INPUT 1


static const NodeInputDefinition
surface_overlay_inputs[] = {
	{
		NodeType__rgb_surface,
		"source-a"
	},
	{
		NodeType__rgb_surface,
		"source-b"
	},
	NODE_INPUT_DEFINITION_END
};


#define WIDTH_PARAMETER  0
#define HEIGHT_PARAMETER 1

static const ParameterDefinition
surface_overlay_parameters[] = {
	{
		ParameterType__integer,
		"width",
		{ .int64_value = 32 }
	},
	{
		ParameterType__integer,
		"height",
		{ .int64_value = 32 }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
surface_overlay_node_delegate = {
	"surface-overlay",
	NodeType__rgb_surface,
	surface_overlay_inputs,
	surface_overlay_parameters,
	{
		surface_overlay_node_setup,
		surface_overlay_node_destroy,
		surface_overlay_node_update,
		surface_overlay_node_output
	},
};


// --- Implementation ---------------------------------------------------------

static bool
surface_overlay_node_setup(
	Node* self
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;

	// Allocate
	SDL_Surface* rgb_surface =
		SDL_CreateRGBSurfaceWithFormat(
			0,
			width,
			height,
			32,
			SDL_PIXELFORMAT_RGBA32
		);

	if (!rgb_surface) {
		SDL_LogError(
			SDL_LOG_CATEGORY_VIDEO,
			"Could not create SDL surface : %s\n",
			SDL_GetError()
		);
		return false;
	}

	// Setup node type metadata
	self->metadata.rgb_surface.width = width;
	self->metadata.rgb_surface.height = height;

	// Job done
	self->data = rgb_surface;
	return true;
}


static void
surface_overlay_node_destroy(
	Node* self
) {
	SDL_Surface* rgb_surface = (SDL_Surface*)self->data;
	if (rgb_surface)
		SDL_FreeSurface(rgb_surface);
}


static void
surface_overlay_node_update(
	Node* self
) {
	// Retrieve inputs and outputs
	SDL_Surface* src_a =
		Node_output(self->inputs[SOURCE_A_INPUT]).rgb_surface;

	SDL_Surface* src_b =
		Node_output(self->inputs[SOURCE_B_INPUT]).rgb_surface;

	SDL_Surface* dst =
		(SDL_Surface*)self->data;

	SDL_BlitSurface(src_a, 0, dst, 0);
	SDL_BlitSurface(src_b, 0, dst, 0);
}


static NodeOutput
surface_overlay_node_output(
	const Node* self
) {
	NodeOutput ret = { .rgb_surface = (SDL_Surface*)self->data };
	return ret;
}
