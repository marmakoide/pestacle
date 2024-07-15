#include <stdlib.h>
#include <assert.h>
#include "vector.h"


void
Vector_init(
	struct Vector* self,
	size_t len
) {
	assert(self != 0);

	self->len = len;
	self->data = array_ops_allocate(self->len);
}


void
Vector_destroy(
	struct Vector* self
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
	const struct Vector* self,
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
Vector_set_coeff(
	struct Vector* self,
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
	const struct Vector* self,
	size_t pos
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(pos < self->len);

	return self->data[pos];
}


void
Vector_fill(
	struct Vector* self,
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
Vector_arange(
	struct Vector* self,
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
	struct Vector* self,
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
	const struct Vector* self
) {
	assert(self != 0);
	assert(self->data != 0);

	return array_ops_sum(
		self->data,
		self->len
	);
}


real_t
Vector_dot(
	const struct Vector* self,
	const struct Vector* other
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
	struct Vector* self,
	const struct Vector* in
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
Vector_scaled_add(
	struct Vector* self,
	const struct Vector* in,
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
Vector_correlation(
	const struct Vector* self,
	const struct Vector* kernel,
	struct Vector* out
) {
	assert(self != 0);
	assert(self->data != 0);
	assert(kernel != 0);
	assert(kernel->data != 0);
	assert(out != 0);
	assert(out->data != 0);
	assert(self->len == out->len);

	array_ops_correlation(
		out->data,
		self->data,
		kernel->data,
		self->len,
		kernel->len
	);
}