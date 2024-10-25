#include <pestacle/memory.h>

#include "ieee764.h"
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

static const char
npy_header_signature[] = {
	'\x93', 'N', 'U', 'M', 'P', 'Y', '\x01', '\x00'
};

static const char
npy_byte_order = '<';

static const char*
npy_dtype = "f4";

static const bool
npy_fortran_order = false;


uint32_t
uint32t_reverse_bytes(
	uint32_t x
) { 
  return 
      ((x >> 24) & 0x000000fful) | 
      ((x >>  8) & 0x0000ff00ul) | 
      ((x <<  8) & 0x00ff0000ul) | 
      ((x << 24) & 0xff000000ul); 
}


static bool
write_npy(
	const char* path,
	const Matrix* matrix
) {
	/*
	 * Generate file header
	 */

	char header_data[1024];
	
	// Fill the header with ' ' char
	memset(header_data, ' ', sizeof(header_data));

	// Write the signature
	memcpy(header_data, npy_header_signature, sizeof(npy_header_signature));
	size_t header_size = sizeof(npy_header_signature) + 2;

	// Write metadata
	int metadata_size = snprintf(
		header_data + sizeof(npy_header_signature) + 2,
		sizeof(header_data) - sizeof(npy_header_signature) + 2,
		"{'descr': '%c%s', 'fortran_order': %s, 'shape': (%zu, %zu), }",
		npy_byte_order,
		npy_dtype,
		npy_fortran_order ? "True" : "False",
		matrix->row_count,
		matrix->col_count
	);

	header_size += metadata_size;

	// Replace the trailing '0' left by snprintf
	header_data[header_size] = ' ';

	// Pad header size to a multiple of 64
	size_t padded_header_size = header_size;
	if (header_size % 64 != 0)
		padded_header_size += 64 - (header_size % 64);

	// Final character of the header is a line return
	header_data[padded_header_size - 1] = '\n';

	// Write the header size
	header_data[sizeof(npy_header_signature) + 0] = (padded_header_size - (sizeof(npy_header_signature) + 2)) % 256;
	header_data[sizeof(npy_header_signature) + 1] = (padded_header_size - (sizeof(npy_header_signature) + 2)) / 256;
	
	// Open the file
	FILE* fp = fopen(path, "wb");

	// Write the file header
	fwrite(header_data, padded_header_size, 1, fp);

	// Write the file content
	real_t* ptr = matrix->data;
	for(size_t i = 0; i < matrix->row_count; ++i)
		for(size_t j = 0; j < matrix->col_count; ++j, ++ptr) {
			union ieee764_float32 value;
			ieee764_float32_encode(&value, *ptr);

			#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			uint32_t out = value.uint32;
			#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
			uint32_t out = uint32t_reverse_bytes(value.uint32);
			#else
				#error Unsupported byte order
			#endif
			
			fwrite(&out, 4, 1, fp);
		}

	// Close the file
	fclose(fp);

	// Job done
	return true;
}


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

	// Writeh file file
	write_npy(self->path, matrix);

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
