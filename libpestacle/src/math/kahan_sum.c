#include <assert.h>
#include <tgmath.h>
#include <pestacle/math/kahan_sum.h>


void
KahanSum_init(
	KahanSum* self
) {
	assert(self);

	self->sum = 0;
	self->c = 0;
}


void
KahanSum_accumulate(
	KahanSum* self,
	real_t x
) {
	assert(self);
	assert(!isnan(x));

	volatile real_t t = self->sum + x;

	if (fabs(self->sum) >= fabs(x))
		self->c += (self->sum - t) + x;
	else
		self->c += (x - t) + self->sum;

	self->sum = t;
}


real_t
KahanSum_sum(
	KahanSum* self
) {
	assert(self);

	return self->sum + self->c;
}