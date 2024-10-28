#include <pestacle/memory.h>

#include "root/rgb_surface/luminance.h"


// --- Interface --------------------------------------------------------------

static bool
luminance_node_setup(
	Node* self
);


static void
luminance_node_destroy(
	Node* self
);


static void
luminance_node_update(
	Node* self
);


static NodeOutput
luminance_node_output(
	const Node* self
);


#define SOURCE_INPUT 0


static const NodeInputDefinition
luminance_inputs[] = {
	{
		NodeType__rgb_surface,
		"source",
	},
	NODE_INPUT_DEFINITION_END
};


#define WIDTH_PARAMETER  0
#define HEIGHT_PARAMETER 1

static const ParameterDefinition
luminance_parameters[] = {
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
root_rgb_surface_luminance_node_delegate = {
	"luminance",
	NodeType__matrix,
	luminance_inputs,
	luminance_parameters,
	{
		luminance_node_setup,
		luminance_node_destroy,
		luminance_node_update,
		luminance_node_output
	},
};


// --- Implementation ---------------------------------------------------------

static bool
luminance_node_setup(
	Node* self
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;

	// Allocate
	Matrix* matrix =
		(Matrix*)checked_malloc(sizeof(Matrix));

	if (!matrix)
		return false;

	// Setup node type metadata
	self->metadata.matrix.width = width;
	self->metadata.matrix.height = height;

	// Setup the accumulator matrix
	Matrix_init(matrix, height, width);
	Matrix_fill(matrix, (real_t)0);

	// Job done
	self->data = matrix;
	return true;
}


static void
luminance_node_destroy(
	Node* self
) {
	Matrix* matrix = (Matrix*)self->data;
	if (matrix) {
		Matrix_destroy(matrix);
		free(matrix);
	}
}


static real_t
sRGB_to_linear(real_t x) {
	if (x <= ((real_t).04045))
		return x / ((real_t)12.92);

	return powf(((x + ((real_t).055)) / ((real_t)1.055)), ((real_t)2.4));
}


static real_t
Y_to_Lstar(real_t Y) {
	// 1976 CIELAB perceived luminance formula
	if (Y <= ((real_t)0.008856))
		return Y * ((real_t)(903.3));

	return pow(Y, ((real_t)1) / ((real_t)3)) * 116 - 16;
}


static void
luminance_node_update(
	Node* self
) {
	// Retrieve inputs and outputs
	const SDL_Surface* src =
		Node_output(self->inputs[SOURCE_INPUT]).rgb_surface;

	Matrix* dst = (Matrix*)self->data;

	// Compute the output
	real_t* coeff = dst->data;
	const uint8_t* pixel_row = (const uint8_t*)src->pixels;
	for(int i = src->h; i != 0; --i, pixel_row += src->pitch) {
		const uint8_t* pixel = pixel_row;
		for(int j = src->w; j != 0; --j, pixel += 4, ++coeff) {
			real_t R = pixel[0] / ((real_t)255);
			real_t G = pixel[1] / ((real_t)255);
			real_t B = pixel[2] / ((real_t)255);

			R = sRGB_to_linear(R);
			G = sRGB_to_linear(G);
			B = sRGB_to_linear(B);

			real_t Y =
				((real_t)0.2126) * R +
				((real_t)0.7152) * G +
				((real_t)0.0722) * B;
			
			real_t L_star = Y_to_Lstar(Y);

			*coeff = L_star / ((real_t)100);
		}
	}
}


static NodeOutput
luminance_node_output(
	const Node* self
) {
	NodeOutput ret = { .matrix = (Matrix*)self->data };
	return ret;
}
