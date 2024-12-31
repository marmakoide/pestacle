#include <SDL_log.h>
#include <pestacle/memory.h>

#include "root/rgb_surface/overlay.h"


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


#define SOURCE_A_INPUT 0
#define SOURCE_B_INPUT 1


static const NodeInputDefinition
node_inputs[] = {
	{
		"source-a",
		true
	},
	{
		"source-b",
		true
	},
	NODE_INPUT_DEFINITION_END
};


static const ParameterDefinition
node_parameters[] = {
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_rgb_surface_overlay_node_delegate = {
	"overlay",
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
	// Retrieve input data descriptor
	const DataDescriptor* in_a_descriptor =
		&(self->inputs[SOURCE_A_INPUT]->out_descriptor);

	const DataDescriptor* in_b_descriptor =
		&(self->inputs[SOURCE_B_INPUT]->out_descriptor);

	size_t width  = in_a_descriptor->matrix.width;
	size_t height = in_a_descriptor->matrix.height;

	// Check input descriptors validity
	if
		((in_b_descriptor->matrix.width != width) ||
		 (in_b_descriptor->matrix.height != height)) {
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"source-a and source-b should have the same dimensions"
			);
		return false;
	}

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

	// Setup output descriptor
	self->out_descriptor.type = DataType__rgb_surface;
	self->out_descriptor.rgb_surface.width = width;
	self->out_descriptor.rgb_surface.height = height;

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
node_output(
	const Node* self
) {
	NodeOutput ret = { .rgb_surface = (SDL_Surface*)self->data };
	return ret;
}
