#include <tgmath.h>
#include <pestacle/memory.h>

#include "root/rgb_surface/luminance.h"


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
root_rgb_surface_luminance_node_delegate = {
	"luminance",
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

	size_t width  = in_descriptor->rgb_surface.width;
	size_t height = in_descriptor->rgb_surface.height;

	DataDescriptor_set_as_rgb_surface(
		&(self->in_descriptors[SOURCE_INPUT]), width, height
	);

	// Allocate
	Matrix* matrix =
		(Matrix*)checked_malloc(sizeof(Matrix));

	if (!matrix)
		return false;

	// Setup output descriptor
	DataDescriptor_set_as_matrix(&(self->out_descriptor), width, height);

	// Setup the accumulator matrix
	Matrix_init(matrix, height, width);
	Matrix_fill(matrix, (real_t)0);

	// Job done
	self->data = matrix;
	return true;
}


static void
node_destroy(
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

	return pow(((x + ((real_t).055)) / ((real_t)1.055)), ((real_t)2.4));
}


static real_t
Y_to_Lstar(real_t Y) {
	// 1976 CIELAB perceived luminance formula
	if (Y <= ((real_t)0.008856))
		return Y * ((real_t)(903.3));

	return pow(Y, ((real_t)1) / ((real_t)3)) * 116 - 16;
}


static void
node_update(
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
node_output(
	const Node* self
) {
	NodeOutput ret = { .matrix = (Matrix*)self->data };
	return ret;
}
