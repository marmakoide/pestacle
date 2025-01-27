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
		"source",
		true
	},
	NODE_INPUT_DEFINITION_END
};


#define OUTPUT_WIDTH_PARAMETER  0
#define OUTPUT_HEIGHT_PARAMETER 1

static const ParameterDefinition
node_parameters[] = {
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
	// Retrieve input data descriptors
	const DataDescriptor* in_descriptor =
		&(self->inputs[SOURCE_INPUT]->out_descriptor);

	size_t input_width  = in_descriptor->matrix.width;
	size_t input_height = in_descriptor->matrix.height;

	// Setup input data descriptor
	DataDescriptor_set_as_matrix(
		&(self->in_descriptors[SOURCE_INPUT]), input_width, input_height
	);

	// Retrieve the parameters
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

	// Setup output descriptor
	DataDescriptor_set_as_rgb_surface(
		&(self->out_descriptor),
		output_width,
		output_height
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
