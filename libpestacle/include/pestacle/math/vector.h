#ifndef PESTACLE_MATH_VECTOR_H
#define PESTACLE_MATH_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif


#include <pestacle/math/array_ops.h>


typedef struct {
	size_t len;
	real_t* data;
} Vector;


extern void
Vector_init(
	Vector* self,
	size_t len
);


extern void
Vector_destroy(
	Vector* self
);


extern void
Vector_print(
	const Vector* self,
	FILE* f,
	const char* format
);


extern void
Vector_set_gaussian_kernel(
	Vector* self,
	real_t sigma
);


extern void
Vector_set_coeff(
	Vector* self,
	size_t pos,
	real_t value
);


extern real_t
Vector_get_coeff(
	const Vector* self,
	size_t pos
);


extern void
Vector_fill(
	Vector* self,
	real_t value
);


extern void
Vector_copy(
	Vector* self,
	const Vector* other
);


extern void
Vector_arange(
	Vector* self,
	real_t start,
	real_t step
);


extern void
Vector_scale(
	Vector* self,
	real_t value
);


extern void
Vector_square(
	Vector* self
);


extern void
Vector_sqrt(
	Vector* self
);


extern real_t
Vector_sum(
	const Vector* self
);


extern real_t
Vector_square_sum(
	const Vector* self
);


extern real_t
Vector_dot(
	const Vector* self,
	const Vector* other
);


extern void
Vector_add(
	Vector* self,
	const Vector* other
);


extern void
Vector_sub(
	Vector* self,
	const Vector* other
);


extern void
Vector_scaled_add(
	Vector* self,
	const Vector* other,
	real_t value
);


extern void
Vector_min(
	Vector* self,
	const Vector* other
);


extern void
Vector_scaled_min(
	Vector* self,
	const Vector* other,
	real_t value
);


extern void
Vector_max(
	Vector* self,
	const Vector* other
);


extern void
Vector_scaled_max(
	Vector* self,
	const Vector* other,
	real_t value
);


extern void
Vector_convolution(
	Vector* self,
	const Vector* other,
	const Vector* kernel
);


extern void
Vector_box_filter(
	Vector* self,
	const Vector* other,
	size_t filter_size
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_MATH_VECTOR_H */
