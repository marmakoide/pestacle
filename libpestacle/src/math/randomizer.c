#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <pestacle/memory.h>
#include <pestacle/math/randomizer.h>

/*
  Complementary-multiply-with-carry random number generator.

  Devised by George Marsaglia, that generator have, according to my opinion,
  several advantages the popular Mersenne Twister :
     - Various settings for the inner state size (huge period VS memory usage)
     - Simple and elegant
*/



typedef struct {
	uint64_t multiplier;
	uint32_t size;
} RandomizerSetting;



const RandomizerSetting
RandomizerSettingsList[] = 
{
	{ 716514398u,    8u },
	{ 487198574u,   16u },
	{ 547416522u,   32u },
	{ 647535442u,   64u },
	{ 8007626u,    128u },
	{ 8001634u,    256u },
	{ 123554632u,  512u },
	{ 123471786u, 1024u },
	{ 18782u,     4096u }
};

#define RandomizerSettingsListSize (sizeof(RandomizerSettingsList) / sizeof(RandomizerSetting))



static void
Randomizer_reset(
	Randomizer* self
) {
	assert(self);

	self->carry = 362436u;
	self->index = self->size - 1;
}



void
Randomizer_init(
	Randomizer* self,
	enum RandomizerSize size
) {
	assert(self);

	const RandomizerSetting* setting;

	setting =  RandomizerSettingsList + size;
	self->multiplier = setting->multiplier;
	self->size = setting->size;

	self->array = (uint32_t*)checked_malloc(sizeof(uint32_t) * self->size);
}



void
Randomizer_destroy(
	Randomizer* self
) {
	assert(self);

	free(self->array);
}



void
Randomizer_copy(
	Randomizer* self,
	const Randomizer* rnd
) {
	assert(self);
	assert(rnd);

	if (self->size != rnd->size) {
		free(self->array);
		self->size = rnd->size;
		self->multiplier = rnd->multiplier;
		self->array = (uint32_t*)checked_malloc(sizeof(uint32_t) * self->size);
	}

	self->carry = rnd->carry;
	self->index = rnd->index;
	memcpy(self->array, rnd->array, sizeof(uint32_t) * self->size);	
}



void
Randomizer_seed(
	Randomizer* self,
	uint32_t seed
) {
	assert(self);

	uint32_t i, j;
	uint32_t* offset;

	offset = self->array;
	for(i = self->size, j = seed; i != 0; --i, ++offset) {
		j = j ^ (j << 13u);
		j = j ^ (j >> 17u);
		j = j ^ (j << 5u);
		(*offset) = j;
	}

	Randomizer_reset(self);
}



uint32_t
Randomizer_next(
	Randomizer* self
) {
	assert(self);

	uint64_t t;
	uint32_t x, result;
	const uint32_t r = 0xfffffffe;

	self->index += 1;
	self->index &= self->size - 1;

	t = self->multiplier * self->array[self->index] + self->carry;
	self->carry = (uint32_t)(t >> 32u);

	x = (uint32_t)(t + self->carry);
	if (x < self->carry) {
		x++;
		self->carry++;
	}

	result = r - x;
	self->array[self->index] = result;

	return result;
}



real_t
Randomizer_next_uniform(
	Randomizer* self
) {
	assert(self);

	real_t y = Randomizer_next(self);
	return y / ((real_t)4294967295);
}
