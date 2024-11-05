#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pestacle/math/array_ops.h>
#include <pestacle/memory.h>


static real_t
square(real_t x) {
	return x * x;
}


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
array_ops_set_gaussian_kernel(
	real_t* dst,
	size_t len,
	real_t sigma) {
	ssize_t x = len / 2;
	x = -x;

	real_t k = -((real_t)1) / (2 * square(sigma));
	real_t sum = (real_t)0;
	
	real_t* ptr = dst;
	for(size_t i = len; i != 0; --i, ++ptr, ++x) {
		*ptr = expf(k * square(x));
		sum += *ptr;
	}

	array_ops_scale(dst, len, ((real_t)1) / sum);
}


void
array_ops_square(
	real_t* dst,
	size_t len
) {
	for( ; len != 0; --len, ++dst)
		*dst *= *dst;
}


void
array_ops_sqrt(
	real_t* dst,
	size_t len
) {
	for( ; len != 0; --len, ++dst)
		*dst = sqrtf(*dst);
}


void
array_ops_exp(
	real_t* dst,
	size_t len
) {
	for( ; len != 0; --len, ++dst)
		*dst = expf(*dst);
}


void
array_ops_log(
	real_t* dst,
	size_t len
) {
	for( ; len != 0; --len, ++dst)
		*dst = logf(*dst);
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
array_ops_reduction_min(
	const real_t* src,
	size_t len
) {
	real_t ret = *src;
	for(--len, ++src; len != 0; --len, ++src)
		ret = fminf(ret, *src);

	return ret;
}


real_t
array_ops_reduction_max(
	const real_t* src,
	size_t len
) {
	real_t ret = *src;
	for(--len, ++src; len != 0; --len, ++src)
		ret = fmaxf(ret, *src);

	return ret;
}


real_t
array_ops_reduction_sum(
	const real_t* src,
	size_t len
) {
	real_t ret = (*src);
	for(--len, ++src; len != 0; --len, ++src)
		ret += (*src);

	return ret;
}


real_t
array_ops_reduction_square_sum(
	const real_t* src,
	size_t len
) {
	real_t ret = (*src) * (*src);
	for(--len, ++src; len != 0; --len, ++src)
		ret = fmaf(*src, *src, ret);

	return ret;
}


real_t
array_ops_dot(
	const real_t* src,
	const real_t* other,
	size_t len
) {
	real_t sum = (*src) * (*other);
	for(--len, ++src, ++other; len != 0; --len, ++src, ++other)
		sum = fmaf(*src, *other, sum);
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
array_ops_sub(
	real_t* dst,
	const real_t* src,
	size_t len
) {
	for( ; len != 0; --len, ++dst, ++src)
		*dst -= (*src);
}


void
array_ops_scaled_add(
	real_t* dst,
	const real_t* src,
	size_t len,
	real_t factor
) {
	for( ; len != 0; --len, ++dst, ++src)
		*dst = fmaf(factor, *src, *dst);
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
array_ops_convolution__zero(
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
			sum = fmaf(*_kernel, *_src, sum);

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
			sum = fmaf(*_kernel, *_src, sum);

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
			sum = fmaf(*_kernel, *_src, sum);

		*dst = sum;
	}
}


void
array_ops_convolution__mirror(
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
			sum = fmaf(*_kernel, *_src, sum);

		_src = src + 1;
		_kernel = kernel + i - 1;

		for(size_t j = i; j != 0; --j, ++_src, --_kernel)
			sum = fmaf(*_kernel, *_src, sum);

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
			sum = fmaf(*_kernel, *_src, sum);

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
			sum = fmaf(*_kernel, *_src, sum);

		_src -= 2;
		for(size_t j = i + 1; j != 0; --j, --_src, ++_kernel)
			sum = fmaf(*_kernel, *_src, sum);

		*dst = sum;
	}
}


void
array_ops_strided_convolution__zero(
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
			sum = fmaf(*_kernel, *_src, sum);

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
			sum = fmaf(*_kernel, *_src, sum);

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
			sum = fmaf(*_kernel, *_src, sum);

		*dst = sum;
	}
}


void
array_ops_strided_convolution__mirror(
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
			sum = fmaf(*_kernel, *_src, sum);

		_src = src + src_stride;
		_kernel = kernel + i - 1;

		for(size_t j = i; j != 0; --j, _src += src_stride, --_kernel)
			sum = fmaf(*_kernel, *_src, sum);
		
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
			sum = fmaf(*_kernel, *_src, sum);

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
			sum = fmaf(*_kernel, *_src, sum);

		_src -= 2 * src_stride;
		for(size_t j = i + 1; j != 0; --j, _src -= src_stride, ++_kernel)
			sum = fmaf(*_kernel, *_src, sum);
		
		*dst = sum;
	}
}


void
array_ops_box_filter(
	real_t* dst,
	const real_t* src,
	size_t len,
	size_t filter_size
) {
	size_t n = filter_size / 2;

	// Left part 
	real_t acc = (real_t)0;
	for(size_t i = n; i != 0; --i, ++src)
		acc += *src;
	
	for(size_t i = n + 1; i != 0; --i, ++src, ++dst) {
		acc += *src;
		*dst = acc / filter_size;
	}

	// Center part
	for(size_t i = len - filter_size; i != 0; --i, ++src, ++dst) {
		acc += *src;
		acc -= *(src - filter_size);
		*dst = acc / filter_size;
	}

	// Right part
	src -= filter_size;
	for(size_t i = n; i != 0; --i, ++src, ++dst) {
		acc -= *src;
		*dst = acc / filter_size;
	}
}
