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
		NodeType__matrix,
		"source"
	},
	NODE_INPUT_DEFINITION_END
};


#define WIDTH_PARAMETER  0
#define HEIGHT_PARAMETER 1
#define VALUE_PARAMETER  2
#define RADIUS_PARAMETER 3
#define RATIO_PARAMETER  4

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
	real_t value;
	real_t factor;
} SoftEqualData;


static void
SoftEqualData_init(
	SoftEqualData* self,
	size_t width,
	size_t height,
	real_t value,
	real_t radius,
	real_t ratio
) {
	Matrix_init(&(self->out), height, width);
	Matrix_fill(&(self->out), (real_t)0);

	self->value = value;
	self->factor = -powf(erfinv(ratio) / radius, 2);
}


static void
SoftEqualData_destroy(
	SoftEqualData* self
) {
	Matrix_destroy(&(self->out));
}


static void
SoftEqualData_transform(
	SoftEqualData* self,
	Matrix* out
) {
	Matrix_inc(out, -self->value);
	Matrix_scale(out, self->factor);
	Matrix_exp(out);
	Matrix_sqrt(out);
}


static bool
node_setup(
	Node* self
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;
	real_t value = (real_t)self->parameters[VALUE_PARAMETER].real_value;
	real_t radius = (real_t)self->parameters[RADIUS_PARAMETER].real_value;
	real_t ratio = (real_t)self->parameters[RATIO_PARAMETER].real_value;

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
	SoftEqualData* data =
		(SoftEqualData*)checked_malloc(sizeof(SoftEqualData));

	if (!data)
		return false;

	// Setup data
	SoftEqualData_init(data, width, height, value, radius, ratio);

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	SoftEqualData* data = (SoftEqualData*)self->data;

	if (data != 0) {
		SoftEqualData_destroy(data);
		free(data);
	}
}


static void
node_update(
	Node* self
) {
	SoftEqualData* data = (SoftEqualData*)self->data;

	Matrix_copy(
		&(data->out),
		Node_output(self->inputs[SOURCE_INPUT]).matrix
	);

	SoftEqualData_transform(
		data,
		&(data->out)
	);
}


static NodeOutput
node_output(
	const Node* self
) {
	const SoftEqualData* data = (const SoftEqualData*)self->data;

	NodeOutput ret = { .matrix = &(data->out) };
	return ret;
}
