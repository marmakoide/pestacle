#ifndef PESTACLE_MATH_ARRAY_OPS_H
#define PESTACLE_MATH_ARRAY_OPS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdio.h>
#include <pestacle/math/randomizer.h>


extern real_t*
array_ops_allocate(
	size_t len
);


extern void
array_ops_print(
	const real_t* src,
	size_t len,
	FILE* f,
	const char* format
);


extern void
array_ops_fill(
	real_t* dst,
	size_t len,
	real_t value
);


extern void
array_ops_arange(
	real_t* dst,
	size_t len,
	real_t start,
	real_t step
);


extern void
array_ops_random_uniform(
	real_t* dst,
	size_t len,
	Randomizer* rng
);


extern void
array_ops_random_normal(
	real_t* dst,
	size_t len,
	Randomizer* rng
);


extern void
array_ops_copy(
	real_t* dst,
	const real_t* src,
	size_t len
);


extern void
array_ops_set_gaussian_kernel(
	real_t* dst,
	size_t len,
	real_t sigma
);


extern void
array_ops_abs(
	real_t* dst,
	size_t len
);


extern void
array_ops_square(
	real_t* dst,
	size_t len
);


extern void
array_ops_sqrt(
	real_t* dst,
	size_t len
);


extern void
array_ops_exp(
	real_t* dst,
	size_t len
);


extern void
array_ops_log(
	real_t* dst,
	size_t len
);


extern void
array_ops_heaviside(
	real_t* dst,
	size_t len,
	real_t threshold
);


extern void
array_ops_inc(
	real_t* dst,
	size_t len,
	real_t shift
);


extern void
array_ops_scale(
	real_t* dst,
	size_t len,
	real_t factor
);


extern void
array_ops_scaled_copy(
	real_t* dst,
	const real_t* src,
	size_t len,
	real_t factor
);


extern real_t
array_ops_reduction_min(
	const real_t* src,
	size_t len
);


extern real_t
array_ops_reduction_max(
	const real_t* src,
	size_t len
);


extern real_t
array_ops_reduction_sum(
	const real_t* src,
	size_t len
);


extern real_t
array_ops_reduction_square_sum(
	const real_t* src,
	size_t len
);


extern real_t
array_ops_reduction_logsumexp(
	const real_t* src,
	size_t len
);


extern real_t
array_ops_reduction_mean(
	const real_t* src,
	size_t len,
	real_t* out_std
);


extern real_t
array_ops_reduction_average(
	const real_t* src,
	const real_t* weight,
	size_t len,
	real_t* out_std
);

 
extern real_t
array_ops_dot(
	const real_t* src,
	const real_t* other,
	size_t len
);


extern void
array_ops_add(
	real_t* dst,
	const real_t* src,
	size_t len
);


extern void
array_ops_sub(
	real_t* dst,
	const real_t* src,
	size_t len
);


extern void
array_ops_scaled_add(
	real_t* dst,
	const real_t* src,
	size_t len,
	real_t factor
);


extern void
array_ops_mul(
	real_t* dst,
	const real_t* src,
	size_t len
);


extern void
array_ops_div(
	real_t* dst,
	const real_t* src,
	size_t len
);


extern void
array_ops_min(
	real_t* dst,
	const real_t* src,
	size_t len
);


extern void
array_ops_scaled_min(
	real_t* dst,
	const real_t* src,
	size_t len,
	real_t factor
);


extern void
array_ops_max(
	real_t* dst,
	const real_t* src,
	size_t len
);


extern void
array_ops_scaled_max(
	real_t* dst,
	const real_t* src,
	size_t len,
	real_t factor
);


extern void
array_ops_convolution__zero(
	real_t* dst,
	const real_t* src,
	const real_t* kernel,
	size_t len,
	size_t kernel_len
);


extern void
array_ops_convolution__mirror(
	real_t* dst,
	const real_t* src,
	const real_t* kernel,
	size_t len,
	size_t kernel_len
);


extern void
array_ops_strided_convolution__zero(
	real_t* dst,
	const real_t* src,
	const real_t* kernel,
	size_t len,
	size_t kernel_len,
	size_t dst_stride,
	size_t src_stride
);


extern void
array_ops_strided_convolution__mirror(
	real_t* dst,
	const real_t* src,
	const real_t* kernel,
	size_t len,
	size_t kernel_len,
	size_t dst_stride,
	size_t src_stride
);


extern void
array_ops_box_filter(
	real_t* dst,
	const real_t* src,
	size_t len,
	size_t filter_size
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_MATH_ARRAY_OPS_H */
