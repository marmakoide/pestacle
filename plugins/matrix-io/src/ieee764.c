#include <assert.h>
#include <math.h>

#include "ieee764.h"


void
ieee764_float32_encode(
	union ieee764_float32* self,
	float value
) {
	assert(self);

	int exponent;
	float mantissa = frexpf(value, &exponent);

	if (value > 0) {
		self->ieee764.sign = 0;
		self->ieee764.mantissa = ((uint32_t)(mantissa * 0x1000000)) & 0x7fffff;
	}
	else {
		self->ieee764.sign = 1;
		self->ieee764.mantissa = ((uint32_t)(-mantissa * 0x1000000)) & 0x7fffff;
	}

	self->ieee764.exponent = exponent + 126;
}
