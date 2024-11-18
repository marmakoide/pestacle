#include <math.h>
#include <stdbool.h>
#include <assert.h>
#include <pestacle/math/univariate_optim.h>


static void
UnivariateOptimResult_init(
	UnivariateOptimResult* self
) {
	self->x = 0;
	self->fx = 0;
	self->iteration_count = 0;
}


void
univariate_optim_golden_section(
	UnivariateFunc func,
	void* func_data,
	real_t a,
	real_t b,
	real_t tolerance,
	size_t max_iteration_count,
	UnivariateOptimResult* out
) {
	assert(func);
	assert(out);

	const real_t phi = (sqrtf(5.f) - 1.f) / 2;

	UnivariateOptimResult_init(out);

	// Order a and b
	if (a > b) {
		real_t tmp = a;
		a = b;
		b = tmp;
	}

	// Initialization
	real_t x1 = a + (((real_t)1) - phi) * (b - a);
	real_t f1 = func(x1, func_data);

	real_t x2 = a + phi * (b - a);
	real_t f2 = func(x2, func_data);

	out->iteration_count = 0;

	// Iterations
	while(true) {
		out->iteration_count += 1;

		// Termination conditions
		if ((b - a) < tolerance) {
			out->x = x1;
			out->fx = f1;
			break;
		}

		if (out->iteration_count == max_iteration_count) {
			out->x = x1;
			out->fx = f1;			
			break;
		}

		// Interval selection
		if (f1 > f2) {
			a = x1;
			x1 = x2;
			f1 = f2;
			x2 = a + phi * (b - a);
			f2 = func(x2, func_data);
		}
		else {
			b = x2;
			x2 = x1;
			f2 = x1;
			x1 = a + (((real_t)1) - phi) * (b - a);
			f1 = func(x1, func_data);
		}
	}
}