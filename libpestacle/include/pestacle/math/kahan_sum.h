#ifndef PESTACLE_MATH_KAHAN_SUM_H
#define PESTACLE_MATH_KAHAN_SUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <pestacle/math/real.h>


/*
 * Implementation of Kahan-Neumaier summation algorithm
 */


typedef struct {
	real_t sum;
	real_t c;
} KahanSum;


extern void
KahanSum_init(
	KahanSum* self
);


extern void
KahanSum_accumulate(
	KahanSum* self,
	real_t x
);


extern real_t
KahanSum_sum(
	KahanSum* self
);




#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_MATH_KAHAN_SUM_H */