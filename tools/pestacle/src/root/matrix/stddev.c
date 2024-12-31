#include <pestacle/memory.h>
#include <pestacle/image/gaussian.h>

#include "root/matrix/stddev.h"


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


#define SIGMA_PARAMETER  0

static const ParameterDefinition
node_parameters[] = {
	{
		ParameterType__real,
		"sigma",
		{ .real_value = (real_t)1 }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_stddev_node_delegate = {
	"stddev",
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

typedef struct {
	Matrix U;
	Matrix out;
	GaussianFilter filter;
} StdDevData;


static void
StdDevData_init(
	StdDevData* self,
	size_t width,
	size_t height,
	real_t sigma
) {
	Matrix_init(&(self->out), height, width);
	Matrix_fill(&(self->out), (real_t)0);

	Matrix_init(&(self->U), height, width);
	Matrix_fill(&(self->U), (real_t)0);

	GaussianFilter_init(
		&(self->filter),
		height,
		width,
		sigma,
		GaussianFilterMode__MIRROR
	);
}


static void
StdDevData_destroy(
	StdDevData* self
) {
	Matrix_destroy(&(self->U));
	Matrix_destroy(&(self->out));
	GaussianFilter_destroy(&(self->filter));
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

	// Retrieve the parameters
	real_t sigma = (real_t)self->parameters[SIGMA_PARAMETER].real_value;

	// Allocate data
	StdDevData* data =
		(StdDevData*)checked_malloc(sizeof(StdDevData));

	if (!data)
		return false;

	// Setup data
	StdDevData_init(data, width, height, sigma);

	// Setup output descriptor
	self->out_descriptor.type = DataType__matrix;
	self->out_descriptor.matrix.width = width;
	self->out_descriptor.matrix.height = height;

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	StdDevData* data = (StdDevData*)self->data;

	if (data != 0) {
		StdDevData_destroy(data);
		free(data);
	}
}


static void
node_update(
	Node* self
) {
	StdDevData* data = (StdDevData*)self->data;

	// Compute out = gaussian(E^2)
	Matrix_copy(
		&(data->out),
		Node_output(self->inputs[SOURCE_INPUT]).matrix
	);
	Matrix_square(&(data->out));
	GaussianFilter_transform(
		&(data->filter),
		&(data->out)
	);

	// Compute U = gaussian(E)^2
	Matrix_copy(
		&(data->U),
		Node_output(self->inputs[SOURCE_INPUT]).matrix
	);
	GaussianFilter_transform(
		&(data->filter),
		&(data->U)
	);
	Matrix_square(&(data->U));

	// Compute out = sqrt(out - U)
	Matrix_sub(&(data->out), &(data->U));
	Matrix_sqrt(&(data->out));
}


static NodeOutput
node_output(
	const Node* self
) {
	const StdDevData* data = (const StdDevData*)self->data;

	NodeOutput ret = { .matrix = &(data->out) };
	return ret;
}
