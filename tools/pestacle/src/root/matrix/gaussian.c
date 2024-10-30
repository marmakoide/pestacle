#include <pestacle/memory.h>
#include <pestacle/image/gaussian.h>

#include "root/matrix/gaussian.h"


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
		NodeType__matrix,
		"source"
	},
	NODE_INPUT_DEFINITION_END
};


#define WIDTH_PARAMETER  0
#define HEIGHT_PARAMETER 1
#define SIGMA_PARAMETER  2

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
	{
		ParameterType__real,
		"sigma",
		{ .real_value = (real_t)1 }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_gaussian_node_delegate = {
	"gaussian",
	NodeType__matrix,
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
	GaussianFilter filter;
} GaussianData;


static void
GaussianData_destroy(
	GaussianData* self
) {
	Matrix_destroy(&(self->out));
	GaussianFilter_destroy(&(self->filter));
}


static bool
node_setup(
	Node* self
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;
	real_t sigma = (real_t)self->parameters[SIGMA_PARAMETER].real_value;

	// Allocate data
	GaussianData* data =
		(GaussianData*)checked_malloc(sizeof(GaussianData));

	if (!data)
		return false;

	// Setup data
	Matrix_init(&(data->out), height, width);
	Matrix_fill(&(data->out), (real_t)0);

	GaussianFilter_init(
		&(data->filter),
		height,
		width,
		sigma
	);

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	GaussianData* data = (GaussianData*)self->data;

	if (data != 0) {
		GaussianData_destroy(data);
		free(data);
	}
}


static void
node_update(
	Node* self
) {
	GaussianData* data = (GaussianData*)self->data;

	Matrix_copy(
		&(data->out),
		Node_output(self->inputs[SOURCE_INPUT]).matrix
	);

	GaussianFilter_transform(
		&(data->filter),
		&(data->out)
	);
}


static NodeOutput
node_output(
	const Node* self
) {
	const GaussianData* data = (const GaussianData*)self->data;

	NodeOutput ret = { .matrix = &(data->out) };
	return ret;
}
