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


void
univariate_optim_brent(
	UnivariateFunc func,
	void* func_data,
	real_t a,
	real_t b,
	real_t epsilon,
	real_t tolerance,
	size_t max_iteration_count,
	UnivariateOptimResult* out
) {
	/*
	 * Brent's method for univariate minimization, implementation taken from John Burkardt
	 */

	assert(func);
	assert(out);

	UnivariateOptimResult_init(out);

	real_t d;
	real_t u;

	/*
	C is the square of the inverse of the golden ratio.
	*/
	const real_t c = (((real_t)3) - sqrt((real_t)5)) / 2;

	real_t sa = a;
	real_t sb = b;
	real_t x = sa + c * (b - a);
	real_t w = x;
	real_t v = w;
	real_t e = (real_t)0;
	real_t fx = func(x, func_data);
	real_t fw = fx;
	real_t fv = fw;

	while(true) { 
		out->iteration_count += 1;

		real_t m = (sa + sb) / 2;
		real_t tol = epsilon * fabs(x) + tolerance;
		real_t t2 = 2.0 * tol;

		// Termination conditions
		if ((fabs(x - m) <= t2 - (sb - sa) / 2) || (out->iteration_count == max_iteration_count))
			break;

		// Fit a parabola
		real_t r = (real_t)0;
		real_t q = r;
		real_t p = q;

		if (tol < fabs (e)) {
			r = (x - w ) * (fx - fv);
			q = (x - v ) * (fx - fw);
			p = (x - v ) * q - (x - w) * r;
			q = 2.0 * (q - r);
			if (0.0 < q)
				p = - p;

			q = fabs(q);
			r = e;
			e = d;
		}

		if (fabs(p) < fabs((q * r) / 2) &&  q * (sa - x) < p && p < q * (sb - x)) {
			// Take the parabolic interpolation step.
			d = p / q;
			u = x + d;

			// F must not be evaluated too close to A or B.
			if ((u - sa) < t2 || (sb - u) < t2) {
				if (x < m)
					d = tol;
				else
					d = -tol;
			}
		}
		// A golden-section step.
    	else {
			if (x < m)
				e = sb - x;
			else
				e = sa - x;
	
			d = c * e;
		}

		// F must not be evaluated too close to X.
		if (tol <= fabs(d))
			u = x + d;
		else {
			if (0.0 < d)
				u = x + tol;
			else
				u = x - tol;
		}

		real_t fu = func(u, func_data);

		// Update A, B, V, W, and X.
		if (fu <= fx) {
			if (u < x)
				sb = x;
			else
				sa = x;
			
			v = w;
			fv = fw;
			w = x;
			fw = fx;
			x = u;
			fx = fu;
		}
		else {
			if (u < x)
				sa = u;
			else
				sb = u;

			if ((fu <= fw) || (w == x)) {
				v = w;
				fv = fw;
				w = u;
				fw = fu;
			}
			else if ((fu <= fv) || (v == x) || (v == w)) {
				v = u;
				fv = fu;
			}
		}
	}

	// Job done
	out->x = x;
	out->fx = fx;
}