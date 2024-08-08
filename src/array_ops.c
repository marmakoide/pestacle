#include <math.h>
#include <stdlib.h>
#include "array_ops.h"
#include "memory.h"


real_t*
array_ops_allocate(
	size_t len
) {
	return (real_t*)checked_malloc(sizeof(real_t) * len);
}


void
array_ops_print(
	const real_t* src,
	size_t len,
	FILE* f,
	const char* format
) {
	for( ; len != 0; --len, ++src) {
		fprintf(f, format, *src);
		if (len > 1)
			fputs(", ", f);
	}
}


void
array_ops_fill(
	real_t* dst,
	size_t len,
	real_t value
) {
	for( ; len != 0; --len, ++dst)
		*dst = value;
}


void
array_ops_arange(
	real_t* dst,
	size_t len,
	real_t start,
	real_t step
) {
	*dst = start;
	dst += 1;
	for(size_t i = 1; i < len; ++i, ++dst)
		*dst = start + i * step;
}


void
array_ops_copy(
	real_t* dst,
	const real_t* src,
	size_t len
) {
	for( ; len != 0; --len, ++dst, ++src)
		*dst = *src;
}


void
array_ops_scale(
	real_t* dst,
	size_t len,
	real_t factor
) {
	for( ; len != 0; --len, ++dst)
		*dst *= factor;
}


void
array_ops_scaled_copy(
	real_t* dst,
	const real_t* src,
	size_t len,
	real_t factor
) {
	for( ; len != 0; --len, ++dst, ++src)
		*dst = factor * (*src);
}


real_t
array_ops_sum(
	const real_t* src,
	size_t len
) {
	real_t sum = (*src);
	for(--len, ++src; len != 0; --len, ++src)
		sum += (*src);
	return sum;
}


real_t
array_ops_square_sum(
	const real_t* src,
	size_t len
) {
	real_t sum = (*src) * (*src);
	for(--len, ++src; len != 0; --len, ++src)
		sum += (*src) * (*src);
	return sum;
}


real_t
array_ops_dot(
	const real_t* src,
	const real_t* other,
	size_t len
) {
	real_t sum = (*src) * (*other);
	for(--len, ++src, ++other; len != 0; --len, ++src, ++other)
		sum += (*src) * (*other);
	return sum;
}


void
array_ops_add(
	real_t* dst,
	const real_t* src,
	size_t len
) {
	for( ; len != 0; --len, ++dst, ++src)
		*dst += (*src);
}


void
array_ops_scaled_add(
	real_t* dst,
	const real_t* src,
	size_t len,
	real_t factor
) {
	for( ; len != 0; --len, ++dst, ++src)
		*dst += factor * (*src);
}


void
array_ops_min(
	real_t* dst,
	const real_t* src,
	size_t len
) {
	for( ; len != 0; --len, ++dst, ++src)
		*dst = fminf(*dst, *src);
}


void
array_ops_scaled_min(
	real_t* dst,
	const real_t* src,
	size_t len,
	real_t factor
) {
	for( ; len != 0; --len, ++dst, ++src)
		*dst = factor * fminf(*dst, *src);
}


void
array_ops_max(
	real_t* dst,
	const real_t* src,
	size_t len
) {
	for( ; len != 0; --len, ++dst, ++src)
		*dst = fmaxf(*dst, *src);
}


void
array_ops_scaled_max(
	real_t* dst,
	const real_t* src,
	size_t len,
	real_t factor
) {
	for( ; len != 0; --len, ++dst, ++src)
		*dst = factor * fmaxf(*dst, *src);
}


void
array_ops_convolution(
	real_t* dst,
	const real_t* src,
	const real_t* kernel,
	size_t len,
	size_t kernel_len
) {
	// Left part
	for(size_t i = kernel_len / 2; i != 0; --i, ++dst) {
		const real_t* _src = src;
		const real_t* _kernel = kernel + i;
		
		real_t sum = (*_kernel) * (*_src);
		_src += 1;
		_kernel += 1;
		for(size_t j = kernel_len - 1 - i; j != 0; --j, ++_src, ++_kernel)
			sum += (*_kernel) * (*_src);

		*dst = sum;
	}

	// Center part
	for(size_t i = len - (kernel_len - 1); i != 0; --i, ++dst, ++src) {
		const real_t* _src = src;
		const real_t* _kernel = kernel;
		
		real_t sum = (*_kernel) * (*_src);
		_src += 1;
		_kernel += 1;
		for(size_t j = kernel_len - 1; j != 0; --j, ++_src, ++_kernel)
			sum += (*_kernel) * (*_src);

		*dst = sum;
	}

	// Right part
	for(size_t i = 0; i < kernel_len / 2; ++i, ++dst, ++src) {
		const real_t* _src = src;
		const real_t* _kernel = kernel;
		
		real_t sum = (*_kernel) * (*_src);
		_src += 1;
		_kernel += 1;
		for(size_t j = kernel_len - 2 - i; j != 0; --j, ++_src, ++_kernel)
			sum += (*_kernel) * (*_src);

		*dst = sum;
	}
}


void
array_ops_strided_convolution(
	real_t* dst,
	const real_t* src,
	const real_t* kernel,
	size_t len,
	size_t kernel_len,
	size_t dst_stride,
	size_t src_stride
) {
	// Left part
	for(size_t i = kernel_len / 2; i != 0; --i, dst += dst_stride) {
		const real_t* _src = src;
		const real_t* _kernel = kernel + i;
		
		real_t sum = (*_kernel) * (*_src);
		_src += src_stride;
		_kernel += 1;
		for(size_t j = kernel_len - 1 - i; j != 0; --j, _src += src_stride, ++_kernel)
			sum += (*_kernel) * (*_src);

		*dst = sum;
	}

	// Center part
	for(size_t i = len - (kernel_len - 1); i != 0; --i, dst += dst_stride, src += src_stride) {
		const real_t* _src = src;
		const real_t* _kernel = kernel;
		
		real_t sum = (*_kernel) * (*_src);
		_src += src_stride;
		_kernel += 1;
		for(size_t j = kernel_len - 1; j != 0; --j, _src += src_stride, ++_kernel)
			sum += (*_kernel) * (*_src);

		*dst = sum;
	}

	// Right part
	for(size_t i = 0; i < kernel_len / 2; ++i, dst += dst_stride, src += src_stride) {
		const real_t* _src = src;
		const real_t* _kernel = kernel;
		
		real_t sum = (*_kernel) * (*_src);
		_src += src_stride;
		_kernel += 1;
		for(size_t j = kernel_len - 2 - i; j != 0; --j, _src += src_stride, ++_kernel)
			sum += (*_kernel) * (*_src);

		*dst = sum;
	}
}
