#ifndef PESTACLE_MATH_MATRIX_H
#define PESTACLE_MATH_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif


#include <pestacle/math/vector.h>

// row major matrix

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
Matrix_random_uniform(
	Matrix* self,
	Randomizer* rng
);


extern void
Matrix_random_normal(
	Matrix* self,
	Randomizer* rng
);


extern void
Matrix_transpose(
	Matrix* self,
	const Matrix* other
);


extern void
Matrix_fill(
	Matrix* self,
	real_t value
);


extern void
Matrix_copy(
	Matrix* self,
	const Matrix* other
);


extern void
Matrix_abs(
	Matrix* self
);


extern void
Matrix_square(
	Matrix* self
);


extern void
Matrix_sqrt(
	Matrix* self
);


extern void
Matrix_exp(
	Matrix* self
);


extern void
Matrix_log(
	Matrix* self
);


extern void
Matrix_heaviside(
	Matrix* self,
	real_t threshold
);


extern real_t
Matrix_reduction_min(
	const Matrix* self
);


extern real_t
Matrix_reduction_max(
	const Matrix* self
);


extern real_t
Matrix_reduction_sum(
	const Matrix* self
);


extern real_t
Matrix_reduction_mean(
	const Matrix* self,
	real_t* out_std
);


extern real_t
Matrix_reduction_average(
	const Matrix* self,
	const Matrix* weight,
	real_t* out_std
);


extern real_t
Matrix_reduction_square_sum(
	const Matrix* self
);


extern real_t
Matrix_reduction_logsumexp(
	const Matrix* self
);


extern void
Matrix_inc(
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
	const Matrix* other
);


extern void
Matrix_sub(
	Matrix* self,
	const Matrix* other
);


extern void
Matrix_scaled_add(
	Matrix* self,
	const Matrix* other,
	real_t value
);


extern void
Matrix_mul(
	Matrix* self,
	const Matrix* other
);


extern void
Matrix_div(
	Matrix* self,
	const Matrix* other
);


extern void
Matrix_min(
	Matrix* self,
	const Matrix* other
);


extern void
Matrix_scaled_min(
	Matrix* self,
	const Matrix* other,
	real_t value
);


extern void
Matrix_resample_nearest(
	Matrix* self,
	const Matrix* other
);


extern void
Matrix_max(
	Matrix* self,
	const Matrix* other
);


extern void
Matrix_scaled_max(
	Matrix* self,
	const Matrix* other,
	real_t value
);


extern void
Matrix_rowwise_convolution__zero(
	Matrix* self,
	const Matrix* other,
	const Vector* kernel
);


extern void
Matrix_colwise_convolution__zero(
	Matrix* self,
	const Matrix* other,
	const Vector* kernel
);


extern void
Matrix_rowwise_convolution__mirror(
	Matrix* self,
	const Matrix* other,
	const Vector* kernel
);


extern void
Matrix_colwise_convolution__mirror(
	Matrix* self,
	const Matrix* other,
	const Vector* kernel
);


extern void
Matrix_rowwise_box_filter(
	Matrix* self,
	const Matrix* other,
	size_t filter_size
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_MATH_MATRIX_H */
