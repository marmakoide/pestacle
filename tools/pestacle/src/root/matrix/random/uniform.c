#include <pestacle/memory.h>

#include "root/matrix/random/uniform.h"


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
#define SEED_PARAMETER 2

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
		ParameterType__integer,
		"seed",
		{ .int64_value = 0 }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_random_uniform_node_delegate = {
	"uniform",
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
	Matrix data;
	Randomizer rng;
} Uniform;


static void
Uniform_init(
	Uniform* self,
	size_t width,
	size_t height,
	uint32_t seed) {
	Randomizer_init(&(self->rng), RandomizerSize_4096);
	Randomizer_seed(&(self->rng), seed);

	Matrix_init(&(self->data), height, width);
	Matrix_fill(&(self->data), (real_t)0);
}



static void
Uniform_destroy(
	Uniform* self
) {
	Matrix_destroy(&(self->data));
}


static void
Uniform_update(
	Uniform* self
) {
	Matrix_random_uniform(&(self->data), &(self->rng));
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
	Uniform* data =
		(Uniform*)checked_malloc(sizeof(Uniform));

	if (!data)
		return false;

	// Setup data
	Uniform_init(data, width, height, seed);

	// Setup node type metadata
	self->type = NodeType__matrix;
	self->type_metadata.matrix.width = width;
	self->type_metadata.matrix.height = height;

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	Uniform* data = (Uniform*)self->data;

	if (data) {
		Uniform_destroy(data);
		free(data);
	}
}


static void
node_update(
	Node* self
) {
	Uniform* data = (Uniform*)self->data;
	Uniform_update(data);
}


static NodeOutput
node_output(
	const Node* self
) {
	const Uniform* data = (const Uniform*)self->data;

	NodeOutput ret = { .matrix = &(data->data) };
	return ret;
}
