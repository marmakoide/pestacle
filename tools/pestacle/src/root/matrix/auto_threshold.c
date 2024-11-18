#define _USE_MATH_DEFINES
#include <math.h>

#ifndef  M_PI
#define  M_PI  3.1415926535897932384626433
#endif

#include <pestacle/memory.h>
#include <pestacle/math/univariate_optim.h>

#include "root/matrix/auto_threshold.h"


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
	PARAMETER_DEFINITION_END
};


const NodeDelegate
root_matrix_auto_threshold_node_delegate = {
	"auto-threshold",
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
	Matrix P[2];
	Matrix P_sum;
	real_t theta[2];
	real_t mu[2];
	real_t sigma[2];
	bool initialized;
	size_t iterations_per_update;
} AutoThreshold;


static void
AutoThreshold_init(
	AutoThreshold* self,
	size_t width,
	size_t height
) {
	Matrix_init(&(self->out), height, width);
	Matrix_fill(&(self->out), (real_t)0);

	for(int i = 0; i < 2; ++i) {
		Matrix_init(&(self->P[i]), height, width);
		Matrix_fill(&(self->P[i]), (real_t)0);
	}

	Matrix_init(&(self->P_sum), height, width);
	Matrix_fill(&(self->P_sum), (real_t)0);

	self->theta[0] = (real_t).5;
	self->theta[1] = (real_t).5 ;

	self->mu[0] = (real_t)0;
	self->mu[1] = (real_t)0;

	self->sigma[0] = (real_t)1;
	self->sigma[1] = (real_t)1;

	self->initialized = false;
	self->iterations_per_update = 1;
}


static void
AutoThreshold_destroy(
AutoThreshold* self
) {
	Matrix_destroy(&(self->out));

	for(int i = 0; i < 2; ++i)
		Matrix_destroy(&(self->P[i]));

	Matrix_destroy(&(self->P_sum));
}


static void
AutoThreshold_em_initialization(
	AutoThreshold* self,
	const Matrix* input
) {
	// Compute extremum values
	real_t coeff_min = Matrix_reduction_min(input);
	real_t coeff_max = Matrix_reduction_max(input);

	// Compute mean, sigma, theta with Welford's algorithm
	size_t count[2] = { 0, 0 };
	for(int i = 0; i < 2; ++i) {
		self->mu[i] = (real_t)0;
		self->sigma[i] = (real_t)0;
	}

	const real_t* coeff = input->data;
	for(size_t i = 0; i < input->row_count; ++i) {
		for(size_t j = 0; j < input->col_count; ++j, ++coeff) {
			int k = fabsf((*coeff) - coeff_min) > fabsf((*coeff) - coeff_max);

			real_t delta = (*coeff) - self->mu[k];
			count[k] += 1;
			self->mu[k] += delta / count[k];
			self->sigma[k] += delta * ((*coeff)- self->mu[k]);
		}
	}

	for(int i = 0; i < 2; ++i) {
		self->sigma[i] = sqrtf(self->sigma[i] / count[i]);
		self->theta[i] = ((real_t)count[i]) / ((real_t)input->data_len);
	}
}


static bool
AutoThreshold_em_iteration(
	AutoThreshold* self,
	const Matrix* input
) {
	// Compute P, ie. membership for each input coeff
	for(int i = 0; i < 2; ++i) {
		Matrix_copy(&(self->P[i]), input);
		Matrix_inc(&(self->P[i]), -self->mu[i]);
		Matrix_scale(&(self->P[i]), ((real_t)1) / self->sigma[i]);
		Matrix_square(&(self->P[i]));
		Matrix_scale(&(self->P[i]), (real_t)-.5);
		Matrix_inc(&(self->P[i]), -logf((sqrtf(2 * M_PI) * self->sigma[i])));
		Matrix_exp(&(self->P[i]));
		Matrix_scale(&(self->P[i]), self->theta[i]);
	}

	// Normalize P
	Matrix_copy(&(self->P_sum), &(self->P[0]));
	Matrix_add(&(self->P_sum), &(self->P[1]));
	for(int i = 0; i < 2; ++i)
		Matrix_div(&(self->P[i]), &(self->P_sum));
	
	// Update mu and sigma
	for(int i = 0; i < 2; ++i) {
		self->theta[i] = Matrix_reduction_mean(&(self->P[i]), 0);
		self->mu[i] = 
			Matrix_reduction_average(
				input,
				&(self->P[i]),
				&(self->sigma[i]));
	}

	// If theta, mu, or sigma is nan, something went wrong
	for(int i = 0; i < 2; ++i)
		if (isnan(self->theta[i]) || isnan(self->mu[i]) || isnan(self->sigma[i]))
			return false;

	// Job done
	return true;
}


static real_t
square(
	real_t x
) {
	return x * x;
}


static real_t
threshold_fitness_func(
	real_t x,
	void* data
) {
	AutoThreshold* self = (AutoThreshold*)data;

	real_t p0 = expf(-.5 * square((x - self->mu[0]) / self->sigma[0]));
    p0 *= self->theta[0] / (sqrtf(2 * M_PI) * self->sigma[0]);

	real_t p1 = expf(-.5 * square((x - self->mu[1]) / self->sigma[1]));
    p1 *= self->theta[1] / (sqrtf(2 * M_PI) * self->sigma[1]);

    real_t p = p1 + p0;
    return p;
}


static real_t
AutoThreshold_get_threshold(
	AutoThreshold* self
) {
	UnivariateOptimResult optim_out;
	univariate_optim_golden_section(
		threshold_fitness_func,
		self,
		self->mu[0],
		self->mu[1],
		1e-4f,
		300,
		&optim_out
	);

	printf("threshold = %f iterations = %zu\n", optim_out.x, optim_out.iteration_count);
	return optim_out.x;
}


static void
AutoThreshold_update(
	AutoThreshold* self,
	const Matrix* input
) {
	// EM iterations
	for(size_t iteration_count = self->iterations_per_update; iteration_count != 0; --iteration_count) {
		if (!(self->initialized)) {
			AutoThreshold_em_initialization(self, input);
			self->initialized = true;
		}
		else
			if (!AutoThreshold_em_iteration(self, input))
				self->initialized = false;

		if (!self->initialized)
			break;
		
		printf(
			"theta = [%f, %f] mu = [%f, %f] sigma = [%f, %f]\n",
			self->theta[0],
			self->theta[1],
			self->mu[0],
			self->mu[1],
			self->sigma[0],
			self->sigma[1]
		);
	}

	// Thresholding
	if (self->initialized) {
		real_t threshold = AutoThreshold_get_threshold(self);
		Matrix_copy(&(self->out), input);
		Matrix_scale(&(self->out), -1);
		Matrix_heaviside(&(self->out), -threshold);
	}
}


static bool
node_setup(
	Node* self
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;

	// Allocate data
	AutoThreshold* data =
		(AutoThreshold*)checked_malloc(sizeof(AutoThreshold));

	if (!data)
		return false;

	// Setup data
	AutoThreshold_init(data, width, height);

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	AutoThreshold* data = (AutoThreshold*)self->data;

	if (data != 0) {
		AutoThreshold_destroy(data);
		free(data);
	}
}


static void
node_update(
	Node* self
) {
	AutoThreshold* data = (AutoThreshold*)self->data;

	AutoThreshold_update(
		data,
		Node_output(self->inputs[SOURCE_INPUT]).matrix
	);
}


static NodeOutput
node_output(
	const Node* self
) {
	const AutoThreshold* data = (const AutoThreshold*)self->data;

	NodeOutput ret = { .matrix = &(data->out) };
	return ret;
}
