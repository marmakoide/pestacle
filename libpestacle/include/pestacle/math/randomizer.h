#ifndef PESTACLE_MATH_RANDOMIZER_H
#define PESTACLE_MATH_RANDOMIZER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <pestacle/math/real.h>


enum RandomizerSize {
	RandomizerSize_8 = 0, // period approx 2^285
	RandomizerSize_16,    // period approx 2^540
	RandomizerSize_32,    // period approx 2^1053
	RandomizerSize_64,    // period approx 2^2077
	RandomizerSize_128,   // period approx 2^4118
	RandomizerSize_256,   // period approx 2^8182
	RandomizerSize_512,   // period approx 2^16410
	RandomizerSize_1024,  // period approx 2^32794
	RandomizerSize_4096   // period approx 2^131104
}; // enum RandomizerSize



typedef struct {
	uint32_t* array;
	uint64_t multiplier;
	uint32_t size;
	uint32_t carry;
	uint32_t index;
} Randomizer;



extern void
Randomizer_init(
	Randomizer* self,
	enum RandomizerSize size
);



extern void
Randomizer_destroy(
	Randomizer* self
);



extern void
Randomizer_copy(
	Randomizer* self,
	const Randomizer* rnd
);



extern void
Randomizer_seed(
	Randomizer* self,
	uint32_t seed
);



extern uint32_t
Randomizer_next(
	Randomizer* self
);



extern real_t
Randomizer_next_uniform(
	Randomizer* self
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_MATH_RANDOMIZER_H */
