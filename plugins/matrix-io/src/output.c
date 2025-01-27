#include <errno.h>
#include <pestacle/memory.h>
#include <SDL_log.h>

#include "ieee764.h"
#include "output.h"


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


#define SOURCE_INPUT 0

static const NodeInputDefinition
node_inputs[] = {
	{
		"source",
		true
	},
	NODE_INPUT_DEFINITION_END
};


#define PATH_PREFIX_PARAMETER 0
#define WIDTH_PARAMETER       1
#define HEIGHT_PARAMETER      2

static const ParameterDefinition
node_parameters[] = {
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
matrix_io_output_node_delegate = {
	"output",
	node_inputs,
	node_parameters,
	{
		node_setup,
		node_destroy,
		node_update,
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
	#define WRITE_BUFFER_SIZE 4096 // Must be a multiple of 4

	// Allocate write buffer
	char* write_buffer = checked_malloc(WRITE_BUFFER_SIZE);

	/*
	 * Generate file header
	 */
	
	// Fill the write_buffer with ' ' char
	memset(write_buffer, ' ', WRITE_BUFFER_SIZE);

	// Copy the signature to write_buffer
	memcpy(write_buffer, npy_header_signature, sizeof(npy_header_signature));
	size_t header_size = sizeof(npy_header_signature) + 2;

	// Write metadata
	int metadata_size = snprintf(
		write_buffer + header_size,
		WRITE_BUFFER_SIZE - header_size,
		"{'descr': '%c%s', 'fortran_order': %s, 'shape': (%zu, %zu), }",
		npy_byte_order,
		npy_dtype,
		npy_fortran_order ? "True" : "False",
		matrix->row_count,
		matrix->col_count
	);

	header_size += metadata_size;

	// Replace the trailing '0' left by snprintf
	write_buffer[header_size] = ' ';

	// Pad header size to a multiple of 64
	size_t padded_header_size = header_size;
	if (header_size % 64 != 0)
		padded_header_size += 64 - (header_size % 64);

	// Final character of the header is a line return
	write_buffer[padded_header_size - 1] = '\n';

	// Write the header size
	write_buffer[sizeof(npy_header_signature) + 0] = (padded_header_size - (sizeof(npy_header_signature) + 2)) % 256;
	write_buffer[sizeof(npy_header_signature) + 1] = (padded_header_size - (sizeof(npy_header_signature) + 2)) / 256;
	
	// Open the file
	FILE* fp = fopen(path, "wb");
	if (!fp) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to open file '%s': %s",
			path,
			strerror(errno)
		);
		goto termination;
	}

	// Write the file header
	if (!fwrite(write_buffer, padded_header_size, 1, fp)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to write to file '%s': %s",
			path,
			strerror(errno)
		);
		goto termination;
	}

	// Write the file content, through a buffer
	const real_t* src = matrix->data;
	
	uint32_t* dst = (uint32_t*)write_buffer;
	size_t buffer_pos = 0;

	for(size_t i = 0; i < matrix->row_count; ++i)
		for(size_t j = 0; j < matrix->col_count; ++j, ++src) {
			union ieee764_float32 value;
			ieee764_float32_encode(&value, *src);

			#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			*dst = value.uint32;
			#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
			*dst = uint32t_reverse_bytes(value.uint32);
			#else
				#error Unsupported byte order
			#endif

			buffer_pos += 4;
			dst += 1;

			if (buffer_pos == WRITE_BUFFER_SIZE) {
				if (!fwrite(write_buffer, WRITE_BUFFER_SIZE, 1, fp)) {
					SDL_LogError(
						SDL_LOG_CATEGORY_SYSTEM,
						"Unable to write to file '%s': %s",
						path,
						strerror(errno)
					);
					goto termination;
				}
				
				dst = (uint32_t*)write_buffer;
				buffer_pos = 0;
			}
		}

	if (buffer_pos > 0) {
		if (!fwrite(write_buffer, buffer_pos, 1, fp)) {
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"Unable to write to file '%s': %s",
				path,
				strerror(errno)
			);
			goto termination;
		}
	}

	// Close the file
	fclose(fp);

termination:
	// Deallocate write_buffer
	if (write_buffer)
		free(write_buffer);

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
node_setup(
	Node* self
) {
	// Retrieve path to video file
	const char* path_prefix = self->parameters[PATH_PREFIX_PARAMETER].string_value;
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;

	// Setup input data descriptor
	DataDescriptor_set_as_matrix(
		&(self->in_descriptors[SOURCE_INPUT]), width, height
	);

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
node_destroy(
	Node* self
) {
	OutputData* data = (OutputData*)self->data;
	if (data)
		free(data);
}


static void
node_update(
	Node* self
) {
	OutputData* data = (OutputData*)self->data;
	OutputData_write(
		data,
		Node_output(self->inputs[SOURCE_INPUT]).matrix
	);
}
