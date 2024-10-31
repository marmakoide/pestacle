#include "minunit.h"

#include <pestacle/macros.h>
#include <pestacle/math/vector.h>
#include <pestacle/math/matrix.h>


// --- Vector tests -----------------------------------------------------------

MU_TEST(test_Vector_fill) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		real_t k = i;

		Vector_init(&U, i);
		Vector_fill(&U, k);

		for(size_t j = 0; j < i; ++j)
			mu_assert_int_eq(
				k,
				Vector_get_coeff(&U, j)
			);

		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_copy) {
	Vector U, V;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);
		Vector_arange(&U, (real_t)0, (real_t)1);
		
		Vector_init(&V, i);
		Vector_copy(&V, &U);

		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(
				Vector_get_coeff(&U, j),
				Vector_get_coeff(&V, j)
			);

		Vector_destroy(&U);
		Vector_destroy(&V);
	}
}


MU_TEST(test_Vector_add) {
	Vector U, V;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);
		Vector_arange(&U, (real_t)0, (real_t)1);
		
		Vector_init(&V, i);
		Vector_copy(&V, &U);

		Vector_add(&U, &V);

		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(
				Vector_get_coeff(&U, j),
				2 * Vector_get_coeff(&V, j)
			);

		Vector_destroy(&U);
		Vector_destroy(&V);
	}
}


MU_TEST(test_Vector_sub) {
	Vector U, V;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);
		Vector_arange(&U, (real_t)0, (real_t)1);
		
		Vector_init(&V, i);
		Vector_copy(&V, &U);

		Vector_sub(&U, &V);

		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(
				Vector_get_coeff(&U, j),
				(real_t)0
			);

		Vector_destroy(&U);
		Vector_destroy(&V);
	}
}


MU_TEST(test_Vector_scaled_add) {
	Vector U, V;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);
		Vector_arange(&U, (real_t)0, (real_t)1);
		
		Vector_init(&V, i);
		Vector_copy(&V, &U);

		Vector_scaled_add(&U, &V, (real_t)4);

		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(
				Vector_get_coeff(&U, j),
				5 * Vector_get_coeff(&V, j)
			);

		Vector_destroy(&U);
		Vector_destroy(&V);
	}
}


MU_TEST(test_Vector_square) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)0, (real_t)1);
		Vector_square(&U);
		
		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(
				j * j,
				Vector_get_coeff(&U, j)
			);

		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_sqrt) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)0, (real_t)1);
		Vector_square(&U);
		Vector_sqrt(&U);
		
		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(
				j,
				Vector_get_coeff(&U, j)
			);

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
			mu_assert_double_eq(
				k * j,
				Vector_get_coeff(&U, j)
			);

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


MU_TEST(test_Vector_convolution__zero) {
	Vector U, V, W;

	for(size_t vector_len = 16; vector_len < 256; ++vector_len) {
		Vector_init(&U, vector_len);
		Vector_arange(&U, (real_t)0, (real_t)1);

		Vector_init(&V, vector_len);
		Vector_fill(&V, (real_t)0);

		for(size_t i = 1; i < 8; ++i) {
			size_t kernel_len = 2 * i + 1;

			Vector_init(&W, kernel_len);
			Vector_set_gaussian_kernel(&W, (real_t)1);

			Vector_convolution__zero(&V, &U, &W);

			for(size_t k = 0; k < vector_len; ++k) {
				real_t sum = (real_t)0;
				for(size_t n = 0; n < kernel_len; ++n)
					if ((k + n >= i) && (k + n < vector_len + i))
						sum = fmaf(
							Vector_get_coeff(&U, k + n - i),
							Vector_get_coeff(&W, n),
							sum
						);
				
				mu_assert_double_eq(Vector_get_coeff(&V, k), sum);
			}
			
			Vector_destroy(&W);
		}

		Vector_destroy(&U);
		Vector_destroy(&V);
	}
}


MU_TEST(test_Vector_box_filter) {
	Vector U, V;

	for(size_t vector_len = 16; vector_len < 256; ++vector_len) {
		Vector_init(&U, vector_len);
		Vector_arange(&U, (real_t)0, (real_t)1);

		Vector_init(&V, vector_len);
		Vector_fill(&V, (real_t)0);

		for(size_t i = 1; i < 8; ++i) {
			size_t kernel_len = 2 * i + 1;

			Vector_box_filter(&V, &U, kernel_len);

			for(size_t k = 0; k < vector_len; ++k) {
				real_t sum = (real_t)0;
				for(size_t n = 0; n < kernel_len; ++n)
					if ((k + n >= i) && (k + n < vector_len + i))
						sum += Vector_get_coeff(&U, k + n - i);
				
				mu_assert_double_eq(Vector_get_coeff(&V, k), sum / kernel_len);
			}
		}

		Vector_destroy(&U);
		Vector_destroy(&V);
	}
}


// --- Matrix tests -----------------------------------------------------------

static void
Matrix_filler(
	Matrix* M
) {
	size_t k = 0;
	for(size_t i = 0; i < M->row_count; ++i)
		for(size_t j = 0; j < M->col_count; ++j, ++k)
			Matrix_set_coeff(M, i, j, k);
}


MU_TEST(test_Matrix_fill) {
	Matrix U;

	for(size_t i = 1; i < 256; i += 9) {
		for(size_t j = 1; j < 256; j += 9) {
			real_t k = i * j;

			Matrix_init(&U, i, j);
			Matrix_fill(&U, k);

			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj)
					mu_assert_int_eq(
						k,
						Matrix_get_coeff(&U, ui, uj)
					);

			Matrix_destroy(&U);
		}
	}
}


MU_TEST(test_Matrix_copy) {
	Matrix U, V;

	for(size_t i = 1; i < 256; i += 9) {
		for(size_t j = 1; j < 256; j += 9) {
			Matrix_init(&U, i, j);
			Matrix_filler(&U);
			
			Matrix_init(&V, i, j);
			Matrix_copy(&V, &U);

			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj)
					mu_assert_double_eq(
						Matrix_get_coeff(&U, ui, uj),
						Matrix_get_coeff(&V, ui, uj)
					);

			Matrix_destroy(&U);
			Matrix_destroy(&V);
		}
	}
}


MU_TEST(test_Matrix_transpose) {
	Matrix U, V;

	for(size_t i = 1; i < 256; i += 9) {
		for(size_t j = 1; j < 256; j += 9) {
			Matrix_init(&U, i, j);
			Matrix_filler(&U);
			
			Matrix_init(&V, j, i);
			Matrix_transpose(&V, &U);

			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj)
					mu_assert_double_eq(
						Matrix_get_coeff(&U, ui, uj),
						Matrix_get_coeff(&V, uj, ui)
					);

			Matrix_destroy(&U);
			Matrix_destroy(&V);
		}
	}
}


MU_TEST(test_Matrix_add) {
	Matrix U, V;

	for(size_t i = 1; i < 256; i += 9) {
		for(size_t j = 1; j < 256; j += 9) {
			Matrix_init(&U, i, j);
			Matrix_filler(&U);
			
			Matrix_init(&V, i, j);
			Matrix_copy(&V, &U);

			Matrix_add(&U, &V);

			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj)
					mu_assert_double_eq(
						Matrix_get_coeff(&U, ui, uj),
						2 * Matrix_get_coeff(&V, ui, uj)
					);

			Matrix_destroy(&U);
			Matrix_destroy(&V);
		}
	}
}


MU_TEST(test_Matrix_sub) {
	Matrix U, V;

	for(size_t i = 1; i < 256; i += 9) {
		for(size_t j = 1; j < 256; j += 9) {
			Matrix_init(&U, i, j);
			Matrix_filler(&U);
			
			Matrix_init(&V, i, j);
			Matrix_copy(&V, &U);

			Matrix_sub(&U, &V);

			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj)
					mu_assert_double_eq(
						Matrix_get_coeff(&U, ui, uj),
						(real_t)0
					);

			Matrix_destroy(&U);
			Matrix_destroy(&V);
		}
	}
}


MU_TEST(test_Matrix_scaled_add) {
	Matrix U, V;

	for(size_t i = 1; i < 256; i += 9) {
		for(size_t j = 1; j < 256; j += 9) {
			Matrix_init(&U, i, j);
			Matrix_filler(&U);
			
			Matrix_init(&V, i, j);
			Matrix_copy(&V, &U);

			Matrix_scaled_add(&U, &V, (real_t)4);

			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj)
					mu_assert_double_eq(
						Matrix_get_coeff(&U, ui, uj),
						5 * Matrix_get_coeff(&V, ui, uj)
					);

			Matrix_destroy(&U);
			Matrix_destroy(&V);
		}
	}
}


MU_TEST(test_Matrix_square) {
	Matrix U;

	for(size_t i = 1; i < 32; i += 9) {
		for(size_t j = 1; j < 32; j += 9) {
			Matrix_init(&U, i, j);
			Matrix_filler(&U);
			Matrix_square(&U);

			size_t uk = 0;
			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj, ++uk)
					mu_assert_double_eq(
						uk * uk,
						Matrix_get_coeff(&U, ui, uj)
					);

			Matrix_destroy(&U);
		}
	}
}


MU_TEST(test_Matrix_sqrt) {
	Matrix U;

	for(size_t i = 1; i < 256; i += 9) {
		for(size_t j = 1; j < 256; j += 9) {
			Matrix_init(&U, i, j);
			Matrix_filler(&U);
			Matrix_square(&U);
			Matrix_sqrt(&U);

			size_t uk = 0;
			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj, ++uk)
					mu_assert_double_eq(
						uk,
						Matrix_get_coeff(&U, ui, uj)
					);

			Matrix_destroy(&U);
		}
	}
}


MU_TEST(test_Matrix_scale) {
	Matrix U;

	for(size_t i = 1; i < 256; i += 9) {
		for(size_t j = 1; j < 256; j += 9) {
			real_t k = 42;

			Matrix_init(&U, i, j);
			Matrix_filler(&U);
			Matrix_scale(&U, k);

			size_t uk = 0;
			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj, ++uk)
					mu_assert_double_eq(
						k * uk,
						Matrix_get_coeff(&U, ui, uj)
					);

			Matrix_destroy(&U);
		}
	}
}


// --- Main entry point -------------------------------------------------------

MU_TEST_SUITE(test_Vector_suite) {
	MU_RUN_TEST(test_Vector_fill);
	MU_RUN_TEST(test_Vector_copy);
	MU_RUN_TEST(test_Vector_add);
	MU_RUN_TEST(test_Vector_sub);
	MU_RUN_TEST(test_Vector_scaled_add);
	MU_RUN_TEST(test_Vector_square);
	MU_RUN_TEST(test_Vector_sqrt);
	MU_RUN_TEST(test_Vector_scale);
	MU_RUN_TEST(test_Vector_sum);
	MU_RUN_TEST(test_Vector_square_sum);
	MU_RUN_TEST(test_Vector_dot);
	MU_RUN_TEST(test_Vector_convolution__zero);
	MU_RUN_TEST(test_Vector_box_filter);
}


MU_TEST_SUITE(test_Matrix_suite) {
	MU_RUN_TEST(test_Matrix_fill);
	MU_RUN_TEST(test_Matrix_copy);
	MU_RUN_TEST(test_Matrix_transpose);
	MU_RUN_TEST(test_Matrix_add);
	MU_RUN_TEST(test_Matrix_sub);
	MU_RUN_TEST(test_Matrix_scaled_add);
	MU_RUN_TEST(test_Matrix_square);
	MU_RUN_TEST(test_Matrix_sqrt);
	MU_RUN_TEST(test_Matrix_scale);
}


int
main(ATTRIBUTE_UNUSED int argc, ATTRIBUTE_UNUSED char *argv[]) {
	MU_RUN_SUITE(test_Vector_suite);
	MU_RUN_SUITE(test_Matrix_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
