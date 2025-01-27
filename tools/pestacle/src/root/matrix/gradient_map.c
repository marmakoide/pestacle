#include <SDL_log.h>
#include <tgmath.h>
#include <pestacle/memory.h>

#include "root/matrix/gradient_map.h"


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


static const ParameterDefinition
node_parameters[] = {
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_gradient_map_node_delegate = {
	"gradient-map",
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
	const DataDescriptor* in_descriptor =
		&(self->inputs[SOURCE_INPUT]->out_descriptor);

	size_t width  = in_descriptor->matrix.width;
	size_t height = in_descriptor->matrix.height;

	// Setup input data descriptor
	DataDescriptor_set_as_matrix(
		&(self->in_descriptors[SOURCE_INPUT]), width, height
	);

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
	DataDescriptor_set_as_rgb_surface(&(self->out_descriptor), width, height);

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
	const Matrix* src =
		Node_output(self->inputs[SOURCE_INPUT]).matrix;

	SDL_Surface* dst =
		(SDL_Surface*)self->data;

	// Compute the output
	const real_t* coeff = src->data;
	uint8_t* pixel_row = (uint8_t*)dst->pixels;
	for(int i = dst->h; i != 0; --i, pixel_row += dst->pitch) {
		uint8_t* pixel = pixel_row;
		for(int j = dst->w; j != 0; --j, pixel += 4, ++coeff) {
			uint8_t level = (uint8_t)fmax(fmin(255.f * (*coeff), 255.f), 0.f);

			for(int k = 0; k < 3; ++k)
				pixel[k] = level;
			pixel[3] = 0xff;
		}
	}
}


static NodeOutput
node_output(
	const Node* self
) {
	NodeOutput ret = { .rgb_surface = (SDL_Surface*)self->data };
	return ret;
}
