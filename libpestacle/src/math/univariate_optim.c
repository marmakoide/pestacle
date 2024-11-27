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

	const real_t inv_phi = (sqrtf(5.f) - 1.f) / 2;

	UnivariateOptimResult_init(out);

	// Order a and b
	if (a > b) {
		real_t tmp = a;
		a = b;
		b = tmp;
	}

	// Initialisation
	real_t c = b - (b - a) * inv_phi;
	real_t fc = func(c, func_data);

	real_t d = a + (b - a) * inv_phi;
	real_t fd = func(d, func_data);

	// Iterations
	out->iteration_count = 0;
	while(true) {
		out->iteration_count += 1;

		// Termination conditions
		if (((b - a) < tolerance) || (out->iteration_count == max_iteration_count)) {
			out->x = (b + a) / 2;
			out->fx = func(out->x, func_data);
			break;
		}

		// Interval selection
		if (fc < fd) {
			b = d;
			d = c;
			fd = fc;
			c = b - (b - a) * inv_phi;
			fc = func(c, func_data);
		}
		else {
			a = c;
			c = d;
			fc = fd;
			d = a + (b - a) * inv_phi;
			fd = func(d, func_data);
		}
	}
}
