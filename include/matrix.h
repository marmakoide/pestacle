#ifndef PESTACLE_MATRIX_H
#define PESTACLE_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif


#include "vector.h"

// col major matrix

typedef struct {
	size_t row_count;
	size_t col_count;
	size_t data_len;
	real_t* data;
} Matrix;


extern void
Matrix_init(
	Matrix* self,
	size_t row_count,
	size_t col_count
);


extern void
Matrix_destroy(
	Matrix* self
);


extern void
Matrix_print(
	const Matrix* self,
	FILE* f,
	const char* format
);


extern void
Matrix_set_coeff(
	Matrix* self,
	size_t row,
	size_t col,
	real_t value
);


extern real_t
Matrix_get_coeff(
	const Matrix* self,
	size_t row,
	size_t col
);


extern void
Matrix_fill(
	Matrix* self,
	real_t value
);


extern void
Matrix_scale(
	Matrix* self,
	real_t value
);


extern void
Matrix_add(
	Matrix* self,
	const Matrix* in
);


extern void
Matrix_scaled_add(
	Matrix* self,
	const Matrix* in,
	real_t value
);


extern void
Matrix_min(
	Matrix* self,
	const Matrix* in
);


extern void
Matrix_scaled_min(
	Matrix* self,
	const Matrix* in,
	real_t value
);


extern void
Matrix_resample_nearest(
	Matrix* self,
	const Matrix* in
);


extern void
Matrix_max(
	Matrix* self,
	const Matrix* in
);


extern void
Matrix_scaled_max(
	Matrix* self,
	const Matrix* in,
	real_t value
);


extern void
Matrix_rowwise_convolution(
	const Matrix* self,
	const Vector* kernel,
	Matrix* out
);


void
Matrix_colwise_convolution(
	const Matrix* self,
	const Vector* kernel,
	Matrix* out
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_MATRIX_H */