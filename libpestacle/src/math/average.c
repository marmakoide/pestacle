#include <tgmath.h>
#include <assert.h>
#include <pestacle/math/average.h>


// --- AverageResult ---------------------------------------------------------

void
AverageResult_init(
	AverageResult* self
) {
	assert(self);

	self->count = 0;
	self->mean = 0;
	self->mean2 = 0;
}


void
AverageResult_accumulate(
	AverageResult* self,
	real_t x
) {
	assert(self);
	assert(!isnan(x));

	self->count += 1;
	real_t delta = x - self->mean;
	self->mean += delta / self->count;
	self->mean2 = fma(delta, x - self->mean, self->mean2);

}


size_t
AverageResult_count(
	AverageResult* self
) {
	assert(self);

	return self->count;
}


real_t
AverageResult_mean(
	AverageResult* self
) {
	assert(self);

	return self->mean;
}


real_t
AverageResult_stddev(
	AverageResult* self
) {
	assert(self);

	return sqrt(self->mean2 / self->count);
}


// --- WeightedAverageResult -------------------------------------------------

void
WeightedAverageResult_init(
	WeightedAverageResult* self
) {
	assert(self);

	self->weight_sum = 0;
	self->mean = 0;
	self->mean2 = 0;
}


void
WeightedAverageResult_accumulate(
	WeightedAverageResult* self,
	real_t w,
	real_t x
) {
	assert(self);
	assert(w >= 0);
	assert(!isnan(w));
	assert(!isnan(x));

	if (w != 0) {
		self->weight_sum += w;
		real_t prev_mean = self->mean;
		real_t delta = x - prev_mean;
		self->mean = fma(w / self->weight_sum, delta, prev_mean);
		self->mean2 = fma(w, delta * (x - self->mean), self->mean2);
	}
}


real_t
WeightedAverageResult_weight_sum(
	WeightedAverageResult* self
) {
	assert(self);

	return self->weight_sum;
}


real_t
WeightedAverageResult_mean(
	WeightedAverageResult* self
) {
	assert(self);

	return self->mean;
}


real_t
WeightedAverageResult_stddev(
	WeightedAverageResult* self
) {
	assert(self);

	return sqrt(self->mean2 / self->weight_sum);
}
