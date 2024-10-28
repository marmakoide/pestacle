#include "root/rgb_surface/resize.h"


// --- Interface --------------------------------------------------------------

static bool
resize_node_setup(
	Node* self
);


static void
resize_node_destroy(
	Node* self
);


static void
resize_node_update(
	Node* self
);


static NodeOutput
resize_node_output(
	const Node* self
);


#define SOURCE_INPUT 0

static const NodeInputDefinition
resize_inputs[] = {
	{
		NodeType__rgb_surface,
		"source",
	},
	NODE_INPUT_DEFINITION_END
};


#define INPUT_WIDTH_PARAMETER   0
#define INPUT_HEIGHT_PARAMETER  1
#define OUTPUT_WIDTH_PARAMETER  2
#define OUTPUT_HEIGHT_PARAMETER 3

static const ParameterDefinition
resize_parameters[] = {
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
	NodeType__rgb_surface,
	resize_inputs,
	resize_parameters,
	{
		resize_node_setup,
		resize_node_destroy,
		resize_node_update,
		resize_node_output
	},
};


// --- Implementation ---------------------------------------------------------

static bool
resize_node_setup(
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
	self->metadata.rgb_surface.width = output_width;
	self->metadata.rgb_surface.height = output_height;

	// Job done
	self->data = rgb_surface;
	return true;
}


static void
resize_node_destroy(
	Node* self
) {
	SDL_Surface* rgb_surface = (SDL_Surface*)self->data;
	if (rgb_surface)
		SDL_FreeSurface(rgb_surface);
}


static void
resize_node_update(
	Node* self
) {
	SDL_Surface* src =
		Node_output(self->inputs[SOURCE_INPUT]).rgb_surface;

	SDL_Surface* dst = (SDL_Surface*)self->data;

	SDL_BlitScaled(src, 0, dst, 0);
}


static NodeOutput
resize_node_output(
	const Node* self
) {
	NodeOutput ret = { .rgb_surface = (SDL_Surface*)self->data };
	return ret;
}
