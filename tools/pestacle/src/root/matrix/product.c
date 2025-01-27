#include <SDL_log.h>
#include <pestacle/memory.h>

#include "root/matrix/product.h"


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


#define SOURCE_A_INPUT 0
#define SOURCE_B_INPUT 1

static const NodeInputDefinition
node_inputs[] = {
	{
		"source-a",
		true
	},
	{
		"source-b",
		true
	},	
	NODE_INPUT_DEFINITION_END
};


static const ParameterDefinition
node_parameters[] = {
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_product_node_delegate = {
	"product",
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
} Product;


static void
Product_init(
	Product* self,
	size_t width,
	size_t height
) {
	Matrix_init(&(self->out), height, width);
	Matrix_fill(&(self->out), (real_t)0);
}


static void
Product_destroy(
	Product* self
) {
	Matrix_destroy(&(self->out));
}


static bool
node_setup(
	Node* self
) {
	// Retrieve input data descriptors
	const DataDescriptor* in_descriptor =
		&(self->inputs[SOURCE_A_INPUT]->out_descriptor);

	size_t width  = in_descriptor->matrix.width;
	size_t height = in_descriptor->matrix.height;

	// Setup input data descriptor
	DataDescriptor_set_as_matrix(
		&(self->in_descriptors[SOURCE_A_INPUT]), width, height
	);

	DataDescriptor_set_as_matrix(
		&(self->in_descriptors[SOURCE_B_INPUT]), width, height
	);

	// Allocate data
	Product* data =
		(Product*)checked_malloc(sizeof(Product));

	if (!data)
		return false;

	// Setup data
	Product_init(data, width, height);

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
	Product* data = (Product*)self->data;

	if (data) {
		Product_destroy(data);
		free(data);
	}
}


static void
node_update(
	Node* self
) {
	Product* data = (Product*)self->data;

	Matrix_copy(
		&(data->out),
		Node_output(self->inputs[SOURCE_A_INPUT]).matrix
	);
	Matrix_mul(
		&(data->out),
		Node_output(self->inputs[SOURCE_B_INPUT]).matrix
	);
}


static NodeOutput
node_output(
	const Node* self
) {
	const Product* data = (const Product*)self->data;

	NodeOutput ret = { .matrix = &(data->out) };
	return ret;
}
