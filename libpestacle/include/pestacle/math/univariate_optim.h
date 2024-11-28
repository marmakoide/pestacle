#ifndef PESTACLE_MATH_UNIVARIATE_OPTIM_H
#define PESTACLE_MATH_UNIVARIATE_OPTIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <pestacle/math/real.h>


typedef struct {
	real_t x;
	real_t fx;
	size_t iteration_count;
} UnivariateOptimResult;


typedef real_t (*UnivariateFunc)(real_t, void*);


extern void
univariate_optim_golden_section(
	UnivariateFunc func,
	void* func_data,
	real_t a,
	real_t b,
	real_t tolerance,
	size_t max_iteration_count,
	UnivariateOptimResult* out
);


extern void
univariate_optim_brent(
	UnivariateFunc func,
	void* func_data,
	real_t a,
	real_t b,
	real_t epsilon,
	real_t tolerance,
	size_t max_iteration_count,
	UnivariateOptimResult* out
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_MATH_UNIVARIATE_OPTIM_H */