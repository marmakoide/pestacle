#ifndef PESTACLE_MATRIX_H
#define PESTACLE_MATRIX_H

#include "vector.h"

// col major matrix

struct Matrix {
	size_t row_count;
	size_t col_count;
	size_t data_len;
	real_t* data;
}; // struct Matrix


extern void
Matrix_init(
	struct Matrix* self,
	size_t row_count,
	size_t col_count
);


extern void
Matrix_destroy(
	struct Matrix* self
);


extern void
Matrix_print(
	const struct Matrix* self,
	FILE* f,
	const char* format
);


extern void
Matrix_set_coeff(
	struct Matrix* self,
	size_t row,
	size_t col,
	real_t value
);


extern real_t
Matrix_get_coeff(
	const struct Matrix* self,
	size_t row,
	size_t col
);


extern void
Matrix_fill(
	struct Matrix* self,
	real_t value
);


extern void
Matrix_scale(
	struct Matrix* self,
	real_t value
);


extern void
Matrix_add(
	struct Matrix* self,
	const struct Matrix* in
);


extern void
Matrix_scaled_add(
	struct Matrix* self,
	const struct Matrix* in,
	real_t value
);


extern void
Matrix_rowwise_correlation(
	const struct Matrix* self,
	const struct Vector* kernel,
	struct Matrix* out
);


void
Matrix_colwise_correlation(
	const struct Matrix* self,
	const struct Vector* kernel,
	struct Matrix* out
);


#endif /* PESTACLE_MATRIX_H */