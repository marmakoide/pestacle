#include <SDL_log.h>
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
		"source",
		true
	},
	NODE_INPUT_DEFINITION_END
};


#define SIGMA_PARAMETER  0
#define MODE_PARAMETER   1

static const ParameterDefinition
node_parameters[] = {
	{
		ParameterType__real,
		"sigma",
		{ .real_value = (real_t)1 }
	},
	{
		ParameterType__string,
		"mode",
		{ .string_value = "zero" }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_gaussian_node_delegate = {
	"gaussian",
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
GaussianData_init(
	GaussianData* self,
	size_t width,
	size_t height,
	real_t sigma,
	enum GaussianFilterMode mode
) {
	Matrix_init(&(self->out), height, width);
	Matrix_fill(&(self->out), (real_t)0);

	GaussianFilter_init(
		&(self->filter),
		height,
		width,
		sigma,
		mode
	);
}


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
	// Retrieve input data descriptor
	const DataDescriptor* in_descriptor =
		&(self->inputs[SOURCE_INPUT]->out_descriptor);

	// Setup input data descriptor
	size_t width  = in_descriptor->matrix.width;
	size_t height = in_descriptor->matrix.height;

	DataDescriptor_set_as_matrix(
		&(self->in_descriptors[SOURCE_INPUT]), width, height
	);

	// Retrieve the parameters
	real_t sigma = (real_t)self->parameters[SIGMA_PARAMETER].real_value;
	char* mode_str = (char*)self->parameters[MODE_PARAMETER].string_value;

	// Check parameters validity
	if (sigma <= __FLT_EPSILON__) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"invalid sigma parameter"
		);
		return false;
	}

	enum GaussianFilterMode mode = GaussianFilterMode__ZERO;
	if (strcmp(mode_str, "zero") == 0) {
		mode = GaussianFilterMode__ZERO;
	}
	else if (strcmp(mode_str, "mirror") == 0) {
		mode = GaussianFilterMode__MIRROR;
	}
	else {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"invalid mode parameter"
		);
		return false;
	}

	// Allocate data
	GaussianData* data =
		(GaussianData*)checked_malloc(sizeof(GaussianData));

	if (!data)
		return false;

	// Setup data
	GaussianData_init(data, width, height, sigma, mode);

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
	GaussianData* data = (GaussianData*)self->data;

	if (data) {
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
