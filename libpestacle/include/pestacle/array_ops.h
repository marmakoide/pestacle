#ifndef PESTACLE_ARRAY_OPS_H
#define PESTACLE_ARRAY_OPS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdio.h>
#include <pestacle/real.h>


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
array_ops_sum(
	const real_t* src,
	size_t len
);


extern real_t
array_ops_square_sum(
	const real_t* src,
	size_t len
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
array_ops_convolution(
	real_t* dst,
	const real_t* src,
	const real_t* kernel,
	size_t len,
	size_t kernel_len
);


extern void
array_ops_strided_convolution(
	real_t* dst,
	const real_t* src,
	const real_t* kernel,
	size_t len,
	size_t kernel_len,
	size_t dst_stride,
	size_t src_stride
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_ARRAY_OPS_H */
