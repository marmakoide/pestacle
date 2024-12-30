#include <SDL_log.h>
#include "root/rgb_surface/resize.h"


// --- Interface --------------------------------------------------------------

static bool
node_setup(
	Node* self
);


static void
node_destroy(
	Node* self
);


static void
node_update(
	Node* self
);


static NodeOutput
node_output(
	const Node* self
);


#define SOURCE_INPUT 0

static const NodeInputDefinition
node_inputs[] = {
	{
		NodeType__rgb_surface,
		"source",
		true
	},
	NODE_INPUT_DEFINITION_END
};


#define INPUT_WIDTH_PARAMETER   0
#define INPUT_HEIGHT_PARAMETER  1
#define OUTPUT_WIDTH_PARAMETER  2
#define OUTPUT_HEIGHT_PARAMETER 3

static const ParameterDefinition
node_parameters[] = {
	{
		ParameterType__integer,
		"input-width",
		{ .int64_value = 32 }
	},
	{
		ParameterType__integer,
		"input-height",
		{ .int64_value = 32 }
	},
	{
		ParameterType__integer,
		"output-width",
		{ .int64_value = 32 }
	},
	{
		ParameterType__integer,
		"output-height",
		{ .int64_value = 32 }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_rgb_surface_resize_node_delegate = {
	"resize",
	true,
	node_inputs,
	node_parameters,
	{
		node_setup,
		node_destroy,
		node_update,
		node_output
	},
};


// --- Implementation ---------------------------------------------------------

static bool
node_setup(
	Node* self
) {
	// Retrieve the parameters
	//size_t input_width   = (size_t)self->parameters[INPUT_WIDTH_PARAMETER].int64_value;
	//size_t input_height  = (size_t)self->parameters[INPUT_HEIGHT_PARAMETER].int64_value;
	size_t output_width  = (size_t)self->parameters[OUTPUT_WIDTH_PARAMETER].int64_value;
	size_t output_height = (size_t)self->parameters[OUTPUT_HEIGHT_PARAMETER].int64_value;

	// Allocate
	SDL_Surface* rgb_surface =
		SDL_CreateRGBSurfaceWithFormat(
			0,
			output_width,
			output_height,
			32,
			SDL_PIXELFORMAT_RGBA32
		);

	if (!rgb_surface) {
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Could not create SDL surface : %s\n", SDL_GetError());
		return false;
	}

	// Setup node type metadata
	self->type = NodeType__rgb_surface;
	self->type_metadata.rgb_surface.width = output_width;
	self->type_metadata.rgb_surface.height = output_height;

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


static void
node_update(
	Node* self
) {
	SDL_Surface* src =
		Node_output(self->inputs[SOURCE_INPUT]).rgb_surface;

	SDL_Surface* dst = (SDL_Surface*)self->data;

	SDL_BlitScaled(src, 0, dst, 0);
}


static NodeOutput
node_output(
	const Node* self
) {
	NodeOutput ret = { .rgb_surface = (SDL_Surface*)self->data };
	return ret;
}
