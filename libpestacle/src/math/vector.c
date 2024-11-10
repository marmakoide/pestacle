#include <stdlib.h>
#include <assert.h>
#include <pestacle/math/vector.h>


void
Vector_init(
	Vector* self,
	size_t len
) {
	assert(self);
	assert(len > 0);

	self->len = len;
	self->data = array_ops_allocate(self->len);
}


void
Vector_destroy(
	Vector* self
) {
	assert(self);
	assert(self->data);

	free(self->data);

	#ifdef DEBUG
	self->len = 0;
	self->data = 0;
	#endif
}


void
Vector_print(
	const Vector* self,
	FILE* f,
	const char* format
) {
	assert(self);
	assert(self->data);
	assert(f);
	assert(format);

	fputs("[", f);
	array_ops_print(self->data, self->len, f, format);
	fputs("]", f);	
}


void
Vector_set_gaussian_kernel(
	Vector* self,
	real_t sigma
) {
	assert(self);

	array_ops_set_gaussian_kernel(self->data, self->len, sigma);
}


void
Vector_set_coeff(
	Vector* self,
	size_t pos,
	real_t value
) {
	assert(self);
	assert(self->data);
	assert(pos < self->len);

	self->data[pos] = value;
}


real_t
Vector_get_coeff(
	const Vector* self,
	size_t pos
) {
	assert(self);
	assert(self->data);
	assert(pos < self->len);

	return self->data[pos];
}


void
Vector_fill(
	Vector* self,
	real_t value
) {
	assert(self);
	assert(self->data);

	array_ops_fill(
		self->data,
		self->len,
		value
	);
}


void
Vector_copy(
	Vector* self,
	const Vector* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_copy(
		self->data,
		other->data,
		self->len
	);
}


void
Vector_arange(
	Vector* self,
	real_t start,
	real_t step
) {
	assert(self);
	assert(self->data);

	array_ops_arange(
		self->data,
		self->len,
		start,
		step
	);
}


void
Vector_square(
	Vector* self
) {
	assert(self);

	array_ops_square(
		self->data,
		self->len
	);
}


void
Vector_sqrt(
	Vector* self
) {
	assert(self);

	array_ops_sqrt(
		self->data,
		self->len
	);
}


void
Vector_exp(
	Vector* self
) {
	assert(self);

	array_ops_exp(
		self->data,
		self->len
	);
}


void
Vector_log(
	Vector* self
) {
	assert(self);

	array_ops_log(
		self->data,
		self->len
	);
}


void
Vector_inc(
	Vector* self,
	real_t value
) {
	assert(self);
	assert(self->data);

	array_ops_inc(
		self->data,
		self->len,
		value
	);
}


void
Vector_scale(
	Vector* self,
	real_t value
) {
	assert(self);
	assert(self->data);

	array_ops_scale(
		self->data,
		self->len,
		value
	);
}


real_t
Vector_reduction_min(
	const Vector* self
) {
	assert(self);

	return array_ops_reduction_min(
		self->data,
		self->len
	);
}


real_t
Vector_reduction_max(
	const Vector* self
) {
	assert(self);

	return array_ops_reduction_max(
		self->data,
		self->len
	);
}


real_t
Vector_reduction_sum(
	const Vector* self
) {
	assert(self);
	assert(self->data);

	return array_ops_reduction_sum(
		self->data,
		self->len
	);
}


real_t
Vector_reduction_square_sum(
	const Vector* self
) {
	assert(self);
	assert(self->data);

	return array_ops_reduction_square_sum(
		self->data,
		self->len
	);
}


real_t
Vector_reduction_logsumexp(
	const Vector* self
) {
	assert(self);
	assert(self->data);

	return array_ops_reduction_logsumexp(
		self->data,
		self->len
	);
}


real_t
Vector_dot(
	const Vector* self,
	const Vector* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	return array_ops_dot(
		self->data,
		other->data,
		self->len
	);
}


void
Vector_add(
	Vector* self,
	const Vector* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_add(
		self->data,
		other->data,
		self->len
	);
}


void
Vector_sub(
	Vector* self,
	const Vector* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_sub(
		self->data,
		other->data,
		self->len
	);
}


void
Vector_scaled_add(
	Vector* self,
	const Vector* other,
	real_t value
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_scaled_add(
		self->data,
		other->data,
		self->len,
		value
	);
}


void
Vector_mul(
	Vector* self,
	const Vector* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_mul(
		self->data,
		other->data,
		self->len
	);
}


void
Vector_div(
	Vector* self,
	const Vector* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_div(
		self->data,
		other->data,
		self->len
	);
}


void
Vector_min(
	Vector* self,
	const Vector* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_min(
		self->data,
		other->data,
		self->len
	);
}


void
Vector_scaled_min(
	Vector* self,
	const Vector* other,
	real_t value
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_scaled_min(
		self->data,
		other->data,
		self->len,
		value
	);
}


void
Vector_max(
	Vector* self,
	const Vector* other
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_max(
		self->data,
		other->data,
		self->len
	);
}


void
Vector_scaled_max(
	Vector* self,
	const Vector* other,
	real_t value
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_scaled_max(
		self->data,
		other->data,
		self->len,
		value
	);
}


void
Vector_convolution__zero(
	Vector* self,
	const Vector* other,
	const Vector* kernel
) {
	assert(self);
	assert(self->data);
	assert(kernel);
	assert(kernel->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_convolution__zero(
		self->data,
		other->data,
		kernel->data,
		self->len,
		kernel->len
	);
}


void
Vector_convolution__mirror(
	Vector* self,
	const Vector* other,
	const Vector* kernel
) {
	assert(self);
	assert(self->data);
	assert(kernel);
	assert(kernel->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);

	array_ops_convolution__mirror(
		self->data,
		other->data,
		kernel->data,
		self->len,
		kernel->len
	);
}


void
Vector_box_filter(
	Vector* self,
	const Vector* other,
	size_t filter_size
) {
	assert(self);
	assert(self->data);
	assert(other);
	assert(other->data);
	assert(self->len == other->len);
	assert(filter_size % 2 == 1);

	array_ops_box_filter(
		self->data,
		other->data,
		self->len,
		filter_size
	);
}
