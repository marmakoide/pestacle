#include "minunit.h"

#include <pestacle/macros.h>
#include <pestacle/vector.h>


MU_TEST(test_Vector_fill) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		real_t k = i;

		Vector_init(&U, i);

		Vector_fill(&U, k);
		for(size_t j = 1; j < i; ++j)
			mu_check(k == Vector_get_coeff(&U, j));

		Vector_destroy(&U);
	}
}


// --- Main entry point -------------------------------------------------------

MU_TEST_SUITE(test_suite) {
	MU_RUN_TEST(test_Vector_fill);
}


int
main(ATTRIBUTE_UNUSED int argc, ATTRIBUTE_UNUSED char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
