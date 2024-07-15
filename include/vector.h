#ifndef PESTACLE_VECTOR_H
#define PESTACLE_VECTOR_H

#include "array_ops.h"

struct Vector {
	size_t len;
	real_t* data;
}; // struct Vector


extern void
Vector_init(
	struct Vector* self,
	size_t len
);


extern void
Vector_destroy(
	struct Vector* self
);


extern void
Vector_print(
	const struct Vector* self,
	FILE* f,
	const char* format
);


extern void
Vector_set_coeff(
	struct Vector* self,
	size_t pos,
	real_t value
);


extern real_t
Vector_get_coeff(
	const struct Vector* self,
	size_t pos
);


extern void
Vector_fill(
	struct Vector* self,
	real_t value
);


extern void
Vector_arange(
	struct Vector* self,
	real_t start,
	real_t step
);


extern void
Vector_scale(
	struct Vector* self,
	real_t value
);


extern real_t
Vector_sum(
	const struct Vector* self
);


extern real_t
Vector_dot(
	const struct Vector* self,
	const struct Vector* other
);


extern void
Vector_add(
	struct Vector* self,
	const struct Vector* in
);


extern void
Vector_scaled_add(
	struct Vector* self,
	const struct Vector* in,
	real_t value
);


extern void
Vector_correlation(
	const struct Vector* self,
	const struct Vector* kernel,
	struct Vector* out
);


#endif /* PESTACLE_VECTOR_H */