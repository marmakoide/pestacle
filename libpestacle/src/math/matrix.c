#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <pestacle/math/matrix.h>


void
Matrix_init(
	Matrix* self,
	size_t row_count,
	size_t col_count
) {
	assert(self);

	self->row_count = row_count;
	self->col_count = col_count;
	self->data_len = row_count * col_count;
	self->data = array_ops_allocate(self->data_len);
}


void
Matrix_destroy(
	Matrix* self
) {
	assert(self);
	assert(self->data);
	
	free(self->data);

	#ifdef DEBUG
	self->row_count = 0;
	self->col_count = 0;
	self->data_len = 0;
	self->data = 0;
	#endif
}


void
Matrix_print(
	const Matrix* self,
	FILE* fp,
	const char* format
) {
	assert(self);
	assert(self->data);
	assert(fp);
	assert(format);

	const real_t* src = self->data;
	for(size_t i = self->row_count; i != 0; --i, src += self->col_count) {
		if (i == self->row_count)
			fputs("[", fp);
		else
			fputs(" ", fp);
	
		fputs("[", fp);
		array_ops_print(src, self->col_count, fp, format);
		fputs("]", fp);

		if (i > 0)
			fputs("]\n", fp);
		else
			fputs("]]\n", fp);
	}
}


void
Matrix_set_coeff(
	Matrix* self,
	size_t row,
	size_t col,
	real_t value
) {
	assert(self);
	assert(self->data);
	assert(row < self->row_count);
	assert(col < self->col_count);

	self->data[row * self->col_count + col] = value;
}


real_t
Matrix_get_coeff(
	const Matrix* self,
	size_t row,
	size_t col
) {
	assert(self);
	assert(self->data);
	assert(row < self->row_count);
	assert(col < self->col_count);

	return self->data[row * self->col_count + col];
}


void
Matrix_random_uniform(
	Matrix* self,
	Randomizer* rng
) {
	assert(self);
	assert(rng);

	array_ops_random_uniform(
		self->data,
		self->data_len,
		rng
	);
}


void
Matrix_random_normal(
	Matrix* self,
	Randomizer* rng
) {
	assert(self);
	assert(rng);

	array_ops_random_normal(
		self->data,
		self->data_len,
		rng
	);
}


void
Matrix_transpose(
	Matrix* self,
	const Matrix* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->row_count == other->col_count);
	assert(self->col_count == other->row_count);

	real_t* u_row_ptr = self->data;
	const real_t* v_col_ptr = other->data;

	for(size_t i = 0; i < self->row_count; ++i, u_row_ptr += self->col_count, v_col_ptr += 1) {
		real_t* u_ptr = u_row_ptr;
		const real_t* v_ptr = v_col_ptr;

		for(size_t j = 0; j < self->col_count; ++j, ++u_ptr, v_ptr += self->row_count)
			*u_ptr = *v_ptr;
	}
}


void
Matrix_fill(
	Matrix* self,
	real_t value
) {
	assert(self);
	assert(self->data);

	array_ops_fill(
		self->data,
		self->data_len,
		value
	);
}


void
Matrix_copy(
	Matrix* self,
	const Matrix* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	array_ops_copy(
		self->data,
		other->data,
		self->data_len
	);
}


void
Matrix_abs(
	Matrix* self
) {
	assert(self);

	array_ops_abs(
		self->data,
		self->data_len
	);
}


void
Matrix_square(
	Matrix* self
) {
	assert(self);

	array_ops_square(
		self->data,
		self->data_len
	);
}


void
Matrix_sqrt(
	Matrix* self
) {
	assert(self);

	array_ops_sqrt(
		self->data,
		self->data_len
	);
}


void
Matrix_exp(
	Matrix* self
) {
	assert(self);

	array_ops_exp(
		self->data,
		self->data_len
	);
}


void
Matrix_log(
	Matrix* self
) {
	assert(self);

	array_ops_log(
		self->data,
		self->data_len
	);
}


void
Matrix_heaviside(
	Matrix* self,
	real_t threshold
) {
	assert(self);

	array_ops_heaviside(
		self->data,
		self->data_len,
		threshold
	);
}


real_t
Matrix_reduction_min(
	const Matrix* self
) {
	assert(self);

	return
		array_ops_reduction_min(
			self->data,
			self->data_len
		);
}


real_t
Matrix_reduction_max(
	const Matrix* self
) {
	assert(self);

	return
		array_ops_reduction_max(
			self->data,
			self->data_len
		);
}


real_t
Matrix_reduction_sum(
	const Matrix* self
) {
	assert(self);

	return
		array_ops_reduction_sum(
			self->data,
			self->data_len
		);
}


real_t
Matrix_reduction_mean(
	const Matrix* self,
	real_t* out_std
) {
	assert(self);

	return
		array_ops_reduction_mean(
			self->data,
			self->data_len,
			out_std
		);
}


real_t
Matrix_reduction_average(
	const Matrix* self,
	const Matrix* weight,
	real_t* out_std
) {
	assert(self);
	assert(weight);
	assert(self->col_count == weight->col_count);
	assert(self->row_count == weight->row_count);

	return
		array_ops_reduction_average(
			self->data,
			weight->data,
			self->data_len,
			out_std
		);
}


real_t
Matrix_reduction_square_sum(
	const Matrix* self
) {
	assert(self);

	return
		array_ops_reduction_square_sum(
			self->data,
			self->data_len
		);
}


real_t
Matrix_reduction_logsumexp(
	const Matrix* self
) {
	assert(self);

	return
		array_ops_reduction_logsumexp(
			self->data,
			self->data_len
		);
}


void
Matrix_inc(
	Matrix* self,
	real_t value
) {
	assert(self);
	assert(self->data);

	array_ops_inc(
		self->data,
		self->data_len,
		value
	);
}


void
Matrix_scale(
	Matrix* self,
	real_t value
) {
	assert(self);
	assert(self->data);

	array_ops_scale(
		self->data,
		self->data_len,
		value
	);
}


void
Matrix_add(
	Matrix* self,
	const Matrix* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	array_ops_add(
		self->data,
		other->data,
		self->data_len
	);
}


void
Matrix_sub(
	Matrix* self,
	const Matrix* other
) {
	assert(self );
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	array_ops_sub(
		self->data,
		other->data,
		self->data_len
	);
}


void
Matrix_scaled_add(
	Matrix* self,
	const Matrix* other,
	real_t value
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	array_ops_scaled_add(
		self->data,
		other->data,
		self->data_len,
		value
	);
}


void
Matrix_mul(
	Matrix* self,
	const Matrix* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	array_ops_mul(
		self->data,
		other->data,
		self->data_len
	);
}


void
Matrix_div(
	Matrix* self,
	const Matrix* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	array_ops_div(
		self->data,
		other->data,
		self->data_len
	);
}


void
Matrix_min(
	Matrix* self,
	const Matrix* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	array_ops_min(
		self->data,
		other->data,
		self->data_len
	);
}


void
Matrix_scaled_min(
	Matrix* self,
	const Matrix* other,
	real_t value
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	array_ops_scaled_min(
		self->data,
		other->data,
		self->data_len,
		value
	);
}


void
Matrix_max(
	Matrix* self,
	const Matrix* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	array_ops_max(
		self->data,
		other->data,
		self->data_len
	);
}


void
Matrix_scaled_max(
	Matrix* self,
	const Matrix* other,
	real_t value
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	array_ops_scaled_max(
		self->data,
		other->data,
		self->data_len,
		value
	);
}


void
Matrix_resample_nearest(
	Matrix* self,
	const Matrix* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);

	real_t row_scaling_factor = ((real_t)other->row_count) / ((real_t)self->row_count);
	real_t col_scaling_factor = ((real_t)other->col_count) / ((real_t)self->col_count);

	real_t* dst_row = self->data;
	
	for(size_t i = 0; i < self->row_count; ++i, dst_row += self->col_count) {
		size_t u = (size_t)floorf(row_scaling_factor * (i + ((real_t).5)));
		const real_t* src_row = other->data + u * other->col_count;
		
		for(size_t j = 0; j < self->col_count; ++j) {
			size_t v = (size_t)floorf(col_scaling_factor * (j + ((real_t).5)));

			dst_row[j] = src_row[v];
		}
	}
}


void
Matrix_rowwise_convolution__zero(
	Matrix* self,
	const Matrix* other,
	const Vector* kernel
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(kernel);
	assert(kernel->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	const real_t* src = other->data;
	real_t* dst = self->data;
	for(size_t i = other->row_count; i != 0; --i, src += other->col_count, dst += other->col_count)
		array_ops_convolution__zero(
			dst,
			src,
			kernel->data,
			other->col_count,
			kernel->len
		);
}


void
Matrix_colwise_convolution__zero(
	Matrix* self,
	const Matrix* other,
	const Vector* kernel
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(kernel);
	assert(kernel->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	const real_t* src = other->data;
	real_t* dst = self->data;
	for(size_t i = other->col_count; i != 0; --i, ++src, ++dst)
		array_ops_strided_convolution__zero(
			dst,
			src,
			kernel->data,
			other->row_count,
			kernel->len,
			other->col_count,
			other->col_count
		);
}


void
Matrix_rowwise_convolution__mirror(
	Matrix* self,
	const Matrix* other,
	const Vector* kernel
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(kernel);
	assert(kernel->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	const real_t* src = other->data;
	real_t* dst = self->data;
	for(size_t i = other->row_count; i != 0; --i, src += other->col_count, dst += other->col_count)
		array_ops_convolution__mirror(
			dst,
			src,
			kernel->data,
			other->col_count,
			kernel->len
		);
}


void
Matrix_colwise_convolution__mirror(
	Matrix* self,
	const Matrix* other,
	const Vector* kernel
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(kernel);
	assert(kernel->data);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	const real_t* src = other->data;
	real_t* dst = self->data;
	for(size_t i = other->col_count; i != 0; --i, ++src, ++dst)
		array_ops_strided_convolution__mirror(
			dst,
			src,
			kernel->data,
			other->row_count,
			kernel->len,
			other->col_count,
			other->col_count
		);
}


void
Matrix_rowwise_box_filter(
	Matrix* self,
	const Matrix* other,
	size_t filter_size
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(filter_size % 2 == 1);
	assert(self->row_count == other->row_count);
	assert(self->col_count == other->col_count);

	const real_t* src = other->data;
	real_t* dst = self->data;
	for(size_t i = other->row_count; i != 0; --i, src += other->col_count, dst += other->col_count)
		array_ops_box_filter(
			dst,
			src,
			other->col_count,
			filter_size
		);
}
