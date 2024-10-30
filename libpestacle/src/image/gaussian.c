#include <math.h>
#include <assert.h>
#include <pestacle/image/gaussian.h>



void
GaussianFilter_init(
	GaussianFilter* self,
	size_t row_count,
	size_t col_count,
	real_t sigma
) {
	assert(self);
	assert(sigma > (real_t)0);
	
	Matrix_init(&(self->U), row_count, col_count);
	Matrix_fill(&(self->U), (real_t)0);

	// Compute the Gaussian kernel
	size_t kernel_size = 6 * ((size_t)floorf(sigma)) + 1;
	Vector_init(&(self->kernel), kernel_size);
	Vector_set_gaussian_kernel(&(self->kernel), sigma);
}


void
GaussianFilter_destroy(
	GaussianFilter* self
) {
	assert(self);

	Matrix_destroy(&(self->U));
	Vector_destroy(&(self->kernel));
}


void
GaussianFilter_transform(
	GaussianFilter* self,
	Matrix* matrix
) {
	assert(self);
	assert(matrix->row_count == self->U.row_count);
	assert(matrix->col_count == self->U.col_count);

	Matrix_rowwise_convolution(
		&(self->U),
		matrix,
		&(self->kernel)
	);

	Matrix_colwise_convolution(
		matrix,
		&(self->U),
		&(self->kernel)
	);
}
