#ifndef PESTACLE_MATH_AVERAGE_H
#define PESTACLE_MATH_AVERAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <pestacle/math/real.h>
#include <pestacle/math/kahan_sum.h>


/*
 * Implementation of Welford's algorithm to compute average and standard 
 * deviations. It is both numerically stable and memory efficient
 */


// --- AverageResult ----------------------------------------------------------

typedef struct {
	size_t count;
	real_t mean;
	real_t mean2;
} AverageResult;


extern void
AverageResult_init(
	AverageResult* self
);


extern void
AverageResult_accumulate(
	AverageResult* self,
	real_t x
);


extern size_t
AverageResult_count(
	AverageResult* self
);


extern real_t
AverageResult_mean(
	AverageResult* self
);


extern real_t
AverageResult_stddev(
	AverageResult* self
);


// --- WeightedAverageResult --------------------------------------------------

typedef struct {
	real_t weight_sum;
	real_t mean;
	real_t mean2;
} WeightedAverageResult;


extern void
WeightedAverageResult_init(
	WeightedAverageResult* self
);


extern void
WeightedAverageResult_accumulate(
	WeightedAverageResult* self,
	real_t w,
	real_t x
);


extern real_t
WeightedAverageResult_weight_sum(
	WeightedAverageResult* self
);


extern real_t
WeightedAverageResult_mean(
	WeightedAverageResult* self
);


extern real_t
WeightedAverageResult_stddev(
	WeightedAverageResult* self
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_MATH_AVERAGE_H */