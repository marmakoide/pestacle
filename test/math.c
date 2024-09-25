#include "minunit.h"

#include <pestacle/macros.h>
#include <pestacle/vector.h>


// --- Vector tests -----------------------------------------------------------

MU_TEST(test_Vector_fill) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		real_t k = i;

		Vector_init(&U, i);

		Vector_fill(&U, k);
		for(size_t j = 1; j < i; ++j)
			mu_assert_int_eq(k, Vector_get_coeff(&U, j));

		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_scale) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		real_t k = 42;

		Vector_init(&U, i);

		Vector_arange(&U, (real_t)0, (real_t)1);
		Vector_scale(&U, k);
		
		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(k * j, Vector_get_coeff(&U, j));

		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_sum) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)1, (real_t)1);
		real_t u_sum = Vector_sum(&U);

		real_t u_sum_expected = (i * (i + 1)) / 2;
		mu_assert_double_eq(u_sum_expected, u_sum);
		
		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_square_sum) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)1, (real_t)1);
		real_t u_sqr_sum = Vector_square_sum(&U);

		real_t u_sqr_sum_expected = (i * (i + 1) * (2 * i + 1)) / 6;
		mu_assert_double_eq(u_sqr_sum_expected, u_sqr_sum);
		
		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_dot) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)1, (real_t)1);
		real_t u_sqr_sum = Vector_dot(&U, &U);

		real_t u_sqr_sum_expected = (i * (i + 1) * (2 * i + 1)) / 6;
		mu_assert_double_eq(u_sqr_sum_expected, u_sqr_sum);
		
		Vector_destroy(&U);
	}
}


// --- Main entry point -------------------------------------------------------

MU_TEST_SUITE(test_suite) {
	MU_RUN_TEST(test_Vector_fill);
	MU_RUN_TEST(test_Vector_scale);
	MU_RUN_TEST(test_Vector_sum);
	MU_RUN_TEST(test_Vector_square_sum);
	MU_RUN_TEST(test_Vector_dot);
}


int
main(ATTRIBUTE_UNUSED int argc, ATTRIBUTE_UNUSED char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
