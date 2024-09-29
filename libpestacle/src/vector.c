#include <stdlib.h>
#include <assert.h>
#include <pestacle/vector.h>


void
Vector_init(
	Vector* self,
	size_t len
) {
	assert(self != 0);
	assert(len > 0);

	self->len = len;
	self->data = array_ops_allocate(self->len);
}


void
Vector_destroy(
	Vector* self
) {
	assert(self != 0);
	assert(self->data != 0);

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
	assert(self != 0);
	assert(self->data != 0);
	assert(f != 0);
	assert(format != 0);

	fputs("[", f);
	array_ops_print(self->data, self->len, f, format);
	fputs("]", f);	
}


void
Vector_set_gaussian_kernel(
	Vector* self,
	real_t sigma
) {
	assert(self != 0);

	array_ops_set_gaussian_kernel(self->data, self->len, sigma);
}


void
Vector_set_coeff(
	Vector* self,
	size_t pos,
	real_t value
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(pos < self->len);

	self->data[pos] = value;
}


real_t
Vector_get_coeff(
	const Vector* self,
	size_t pos
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(pos < self->len);

	return self->data[pos];
}


void
Vector_fill(
	Vector* self,
	real_t value
) {
	assert(self != 0);
	assert(self->data != 0);

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
	assert(self != 0);
	assert(self->data != 0);
	assert(other != 0);
	assert(other->data != 0);
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
	assert(self != 0);
	assert(self->data != 0);

	array_ops_arange(
		self->data,
		self->len,
		start,
		step
	);
}


void
Vector_scale(
	Vector* self,
	real_t value
) {
	assert(self != 0);
	assert(self->data != 0);

	array_ops_scale(
		self->data,
		self->len,
		value
	);
}


real_t
Vector_sum(
	const Vector* self
) {
	assert(self != 0);
	assert(self->data != 0);

	return array_ops_sum(
		self->data,
		self->len
	);
}


real_t
Vector_square_sum(
	const Vector* self
) {
	assert(self != 0);
	assert(self->data != 0);

	return array_ops_square_sum(
		self->data,
		self->len
	);
}


real_t
Vector_dot(
	const Vector* self,
	const Vector* other
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(other != 0);
	assert(other->data != 0);
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
	const Vector* in
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(in != 0);
	assert(in->data != 0);
	assert(self->len == in->len);

	array_ops_add(
		self->data,
		in->data,
		self->len
	);
}


void
Vector_sub(
	Vector* self,
	const Vector* in
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(in != 0);
	assert(in->data != 0);
	assert(self->len == in->len);

	array_ops_sub(
		self->data,
		in->data,
		self->len
	);
}


void
Vector_scaled_add(
	Vector* self,
	const Vector* in,
	real_t value
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(in != 0);
	assert(in->data != 0);
	assert(self->len == in->len);

	array_ops_scaled_add(
		self->data,
		in->data,
		self->len,
		value
	);
}


void
Vector_min(
	Vector* self,
	const Vector* in
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(in != 0);
	assert(in->data != 0);
	assert(self->len == in->len);

	array_ops_min(
		self->data,
		in->data,
		self->len
	);
}


void
Vector_scaled_min(
	Vector* self,
	const Vector* in,
	real_t value
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(in != 0);
	assert(in->data != 0);
	assert(self->len == in->len);

	array_ops_scaled_min(
		self->data,
		in->data,
		self->len,
		value
	);
}


void
Vector_max(
	Vector* self,
	const Vector* in
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(in != 0);
	assert(in->data != 0);
	assert(self->len == in->len);

	array_ops_max(
		self->data,
		in->data,
		self->len
	);
}


void
Vector_scaled_max(
	Vector* self,
	const Vector* in,
	real_t value
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(in != 0);
	assert(in->data != 0);
	assert(self->len == in->len);

	array_ops_scaled_max(
		self->data,
		in->data,
		self->len,
		value
	);
}


void
Vector_convolution(
	const Vector* self,
	const Vector* kernel,
	Vector* out
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(kernel != 0);
	assert(kernel->data != 0);
	assert(out != 0);
	assert(out->data != 0);
	assert(self->len == out->len);

	array_ops_convolution(
		out->data,
		self->data,
		kernel->data,
		self->len,
		kernel->len
	);
}


void
Vector_box_filter(
	const Vector* self,
	size_t filter_size,
	Vector* out
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(out != 0);
	assert(out->data != 0);
	assert(self->len == out->len);
	assert(filter_size % 2 == 1);

	array_ops_box_filter(
		out->data,
		self->data,
		self->len,
		filter_size
	);
}