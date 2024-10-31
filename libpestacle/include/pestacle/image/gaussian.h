#ifndef PESTACLE_IMAGE_GAUSSIAN_H
#define PESTACLE_IMAGE_GAUSSIAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pestacle/math/vector.h>
#include <pestacle/math/matrix.h>


enum GaussianFilterMode {
	GaussianFilterMode__ZERO,
	GaussianFilterMode__MIRROR
}; // enum GaussianFilterMode


typedef struct {
	Matrix U;
	Vector kernel;
	enum GaussianFilterMode mode;
} GaussianFilter;



extern void
GaussianFilter_init(
	GaussianFilter* self,
	size_t row_count,
	size_t col_count,
	real_t sigma,
	enum GaussianFilterMode mode
);


extern void
GaussianFilter_destroy(
	GaussianFilter* self
);


extern void
GaussianFilter_transform(
	GaussianFilter* self,
	Matrix* matrix
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_IMAGE_GAUSSIAN_H */
