#include <pestacle/memory.h>

#include "root/matrix/random/normal.h"


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


static const NodeInputDefinition
node_inputs[] = {
	NODE_INPUT_DEFINITION_END
};


#define WIDTH_PARAMETER  0
#define HEIGHT_PARAMETER 1
#define MEAN_PARAMETER   2
#define SIGMA_PARAMETER  3
#define SEED_PARAMETER   4

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
		"mean",
		{ .real_value = (real_t)0 }
	},
	{
		ParameterType__real,
		"sigma",
		{ .real_value = (real_t)1 }
	},
	{
		ParameterType__integer,
		"seed",
		{ .int64_value = 0 }
	},	
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_random_normal_node_delegate = {
	"normal",
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
	Matrix data;
	Randomizer rng;
} Normal;


static void
Normal_init(
	Normal* self,
	size_t width,
	size_t height,
	uint32_t seed) {
	Randomizer_init(&(self->rng), RandomizerSize_4096);
	Randomizer_seed(&(self->rng), seed);

	Matrix_init(&(self->data), height, width);
	Matrix_fill(&(self->data), (real_t)0);
}



static void
Normal_destroy(
	Normal* self
) {
	Matrix_destroy(&(self->data));
}


static void
Normal_update(
	Normal* self
) {
	Matrix_random_normal(&(self->data), &(self->rng));
}


static bool
node_setup(
	Node* self
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;
	int64_t seed = (int64_t)self->parameters[SEED_PARAMETER].int64_value;

	// Allocate data
	Normal* data =
		(Normal*)checked_malloc(sizeof(Normal));

	if (!data)
		return false;

	// Setup data
	Normal_init(data, width, height, seed);

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	Normal* data = (Normal*)self->data;

	if (data) {
		Normal_destroy(data);
		free(data);
	}
}


static void
node_update(
	Node* self
) {
	real_t mean = (real_t)self->parameters[MEAN_PARAMETER].real_value;
	real_t sigma = (real_t)self->parameters[SIGMA_PARAMETER].real_value;

	Normal* data = (Normal*)self->data;
	Normal_update(data);
	Matrix_scale(&(data->data), sigma);
	Matrix_inc(&(data->data), mean);
}


static NodeOutput
node_output(
	const Node* self
) {
	const Normal* data = (const Normal*)self->data;

	NodeOutput ret = { .matrix = &(data->data) };
	return ret;
}
