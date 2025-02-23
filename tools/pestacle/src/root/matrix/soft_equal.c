#include <tgmath.h>
#include <SDL_log.h>
#include <pestacle/memory.h>
#include <pestacle/math/special.h>

#include "root/matrix/soft_equal.h"


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


#define VALUE_PARAMETER  0
#define RADIUS_PARAMETER 1
#define RATIO_PARAMETER  2

static const ParameterDefinition
node_parameters[] = {
	{
		ParameterType__real,
		"value",
		{ .real_value = 0. }
	},
	{
		ParameterType__real,
		"radius",
		{ .real_value = 1. }
	},
	{
		ParameterType__real,
		"ratio",
		{ .real_value = (real_t).9 }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_soft_equal_node_delegate = {
	"soft-equal",
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

typedef struct {
	Matrix out;
	real_t value;
	real_t factor;
} SoftEqual;


static void
SoftEqual_init(
	SoftEqual* self,
	size_t width,
	size_t height,
	real_t value,
	real_t radius,
	real_t ratio
) {
	Matrix_init(&(self->out), height, width);
	Matrix_fill(&(self->out), (real_t)0);

	self->value = value;
	self->factor = -pow(erfinv(ratio) / radius, 2);
}


static void
SoftEqual_destroy(
	SoftEqual* self
) {
	Matrix_destroy(&(self->out));
}


static void
SoftEqual_transform(
	SoftEqual* self,
	Matrix* out
) {
	Matrix_inc(out, -self->value);
	Matrix_square(out);
	Matrix_scale(out, self->factor);
	Matrix_exp(out);
}


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

	// Retrieve the parameters
	real_t value  = (real_t)self->parameters[VALUE_PARAMETER].real_value;
	real_t radius = (real_t)self->parameters[RADIUS_PARAMETER].real_value;
	real_t ratio  = (real_t)self->parameters[RATIO_PARAMETER].real_value;

	// Check parameters validity
	if (radius <= 0) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"invalid radius parameter"
		);
		return false;
	}

	if ((ratio <= 0) || (ratio >= 1)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"invalid ratio parameter"
		);
		return false;
	}

	// Allocate data
	SoftEqual* data =
		(SoftEqual*)checked_malloc(sizeof(SoftEqual));

	if (!data)
		return false;

	// Setup data
	SoftEqual_init(data, width, height, value, radius, ratio);

	// Setup output descriptor
	DataDescriptor_set_as_matrix(&(self->out_descriptor), width, height);

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	SoftEqual* data = (SoftEqual*)self->data;

	if (data) {
		SoftEqual_destroy(data);
		free(data);
	}
}


static void
node_update(
	Node* self
) {
	SoftEqual* data = (SoftEqual*)self->data;

	Matrix_copy(
		&(data->out),
		Node_output(self->inputs[SOURCE_INPUT]).matrix
	);

	SoftEqual_transform(
		data,
		&(data->out)
	);
}


static NodeOutput
node_output(
	const Node* self
) {
	const SoftEqual* data = (const SoftEqual*)self->data;

	NodeOutput ret = { .matrix = &(data->out) };
	return ret;
}
