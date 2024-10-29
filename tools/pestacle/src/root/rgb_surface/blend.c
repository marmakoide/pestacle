#include <pestacle/memory.h>

#include "root/rgb_surface/blend.h"


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
#define MASK_INPUT     2


static const NodeInputDefinition
node_inputs[] = {
	{
		NodeType__rgb_surface,
		"source-a"
	},
	{
		NodeType__rgb_surface,
		"source-b"
	},
	{
		NodeType__matrix,
		"mask"
	},
	NODE_INPUT_DEFINITION_END
};


#define WIDTH_PARAMETER  0
#define HEIGHT_PARAMETER 1

static const ParameterDefinition
node_parameters[] = {
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
root_rgb_surface_blend_node_delegate = {
	"blend",
	NodeType__rgb_surface,
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
	const SDL_Surface* src_a =
		Node_output(self->inputs[SOURCE_A_INPUT]).rgb_surface;

	const SDL_Surface* src_b =
		Node_output(self->inputs[SOURCE_B_INPUT]).rgb_surface;

	const Matrix* mask =
		Node_output(self->inputs[MASK_INPUT]).matrix;

	SDL_Surface* dst =
		(SDL_Surface*)self->data;

	// Compute the blend
	const real_t* coeff = mask->data;
	uint8_t* dst_pixel_row = (uint8_t*)dst->pixels;
	const uint8_t* src_a_pixel_row = (const uint8_t*)src_a->pixels;
	const uint8_t* src_b_pixel_row = (const uint8_t*)src_b->pixels;
	
	for(int i = dst->h; i != 0; --i, dst_pixel_row += dst->pitch, src_a_pixel_row += src_a->pitch, src_b_pixel_row += src_b->pitch) {
		uint8_t* dst_pixel = dst_pixel_row;
		const uint8_t* src_a_pixel = src_a_pixel_row;
		const uint8_t* src_b_pixel = src_b_pixel_row;

		for(int j = dst->w; j != 0; --j, dst_pixel += 4, src_a_pixel += 4, src_b_pixel += 4, ++coeff) {
			float level = fmaxf(fminf(*coeff, 1.f), 0.f);
			
			for(int k = 0; k < 3; ++k)
				dst_pixel[k] = (uint8_t)(src_b_pixel[k] * level + src_a_pixel[k] * (1.f - level));
			dst_pixel[3] = 0xff;
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
