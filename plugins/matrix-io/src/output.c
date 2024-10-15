#include <pestacle/memory.h>

#include <cnpy.h>
#include "output.h"


// --- Interface --------------------------------------------------------------

static bool
output_node_setup(
	Node* self
);


static void
output_node_destroy(
	Node* self
);


static void
output_node_update(
	Node* self
);


#define SOURCE_INPUT 0

static const NodeInputDefinition
output_inputs[] = {
	{
		NodeType__matrix,
		"source"
	},
	NODE_INPUT_DEFINITION_END
};


#define PATH_PREFIX_PARAMETER 0
#define WIDTH_PARAMETER       1
#define HEIGHT_PARAMETER      2

static const ParameterDefinition
output_parameters[] = {
	{
		ParameterType__string,
		"path-prefix",
		{ .string_value = "out-" }
	},
	{
		ParameterType__integer,
		"width",
		{ .int64_value = 320 }
	},
	{
		ParameterType__integer,
		"height",
		{ .int64_value = 240 }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
output_node_delegate = {
	"output",
	NodeType__void,
	output_inputs,
	output_parameters,
	{
		output_node_setup,
		output_node_destroy,
		output_node_update,
		0
	},
};


// --- Implementation ---------------------------------------------------------

#define MAX_PATH_LEN 1024

typedef struct {
	size_t dims[2];
	const char* path_prefix;
	char path[MAX_PATH_LEN];
	size_t counter;
} OutputData;


static bool
OutputData_init(
	OutputData* self,
	size_t col_count,
	size_t row_count,
	const char* path_prefix
) {
	self->dims[0] = row_count;
	self->dims[1] = col_count;
	self->path_prefix = path_prefix;
	self->counter = 0;

	return true;
}


static bool
OutputData_write(
	OutputData* self,
	const Matrix* matrix
) {
	bool exit_code = true;

	// Creates the path
	int ret = snprintf(
		self->path,
		MAX_PATH_LEN,
		"%s%06zu.npy",
		self->path_prefix,
		self->counter
	);

	if (ret < 0) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"unable to create file path\n"
		);
		exit_code = false;
		goto termination;
	}

	if (ret >= MAX_PATH_LEN) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"file path is too long\n"
		);
		exit_code = false;
		goto termination;
	}

	// Creates the file
	cnpy_byte_order byte_order = CNPY_BE;
	cnpy_dtype dtype           = CNPY_F4;
	cnpy_flat_order order      = CNPY_FORTRAN_ORDER; 
	
	cnpy_array array;
	if (cnpy_create(
		self->path,
		byte_order,
		dtype, order,
		2,
		self->dims,
		&array) != CNPY_SUCCESS) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"unable to create file '%s'\n",
			self->path
		);
		exit_code = false;
		goto termination;
	}

	// Write the file
	size_t index[2] = { 0, 0 };
	const real_t* src = matrix->data;
	for(size_t i = 0; i < self->dims[0]; ++i) {
		index[0] = i;
		for(size_t j = 0; j < self->dims[1]; ++j, ++src) {
			index[1] = j;
			cnpy_set_f4(array, index, *src);
		}
	}

	// Close the file
	if (cnpy_close(&array) != CNPY_SUCCESS) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"unable to close file '%s'\n",
			self->path
		);
		exit_code = false;
		goto termination;
	}

termination:
	// Job done
	self->counter += 1;
	return exit_code;
}


static bool
output_node_setup(
	Node* self
) {
	// Retrieve path to video file
	const char* path_prefix = self->parameters[PATH_PREFIX_PARAMETER].string_value;
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;

	// Allocate data
	OutputData* data = (OutputData*)checked_malloc(sizeof(OutputData));
	if (!data)
		return false;

	// Initialise data
	if (!OutputData_init(data, width, height, path_prefix)) {
		free(data);
		return false;
	}

	// Job done
	self->data = data;
	return true;
}


static void
output_node_destroy(
	Node* self
) {
	OutputData* data = (OutputData*)self->data;
	if (data)
		free(data);
}


static void
output_node_update(
	Node* self
) {
	OutputData* data = (OutputData*)self->data;
	OutputData_write(
		data,
		Node_output(self->inputs[SOURCE_INPUT]).matrix
	);
}
