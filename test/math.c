#include "minunit.h"

#include <pestacle/macros.h>
#include <pestacle/math/kahan_sum.h>
#include <pestacle/math/vector.h>
#include <pestacle/math/matrix.h>
#include <pestacle/math/special.h>


// --- Special functions tests ------------------------------------------------

MU_TEST(test_special_erfinv) {
	real_t start = (real_t)-1.6;
	real_t end   = (real_t) 1.6;
	size_t step_count = 256;

	for(size_t i = 0; i < step_count; ++i) {
		real_t x = start + ((end - start) / (step_count - 1)) * i;
		real_t y = erfinv(erf(x));
		mu_assert_double_eq(x, y);
	}
}


// --- Kahan sum test ---------------------------------------------------------

MU_TEST(test_kahan_sum) {
	KahanSum sum;

	for(size_t i = 1; i < 1024; ++i) {
		KahanSum_init(&sum);
		for(size_t j = 1; j <= i; ++j)
			KahanSum_accumulate(&sum, j);

		size_t expected_value = (i * (i + 1)) / 2;
		mu_assert_double_eq(expected_value, KahanSum_sum(&sum));
	}
}


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


MU_TEST(test_Vector_mul) {
	Vector U, V;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);
		Vector_arange(&U, (real_t)0, (real_t)1);
		
		Vector_init(&V, i);
		Vector_copy(&V, &U);

		Vector_mul(&U, &V);

		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(
				Vector_get_coeff(&U, j),
				Vector_get_coeff(&V, j) * Vector_get_coeff(&V, j)
			);

		Vector_destroy(&U);
		Vector_destroy(&V);
	}
}


MU_TEST(test_Vector_div) {
	Vector U, V;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);
		Vector_arange(&U, (real_t)1, (real_t)1);
		
		Vector_init(&V, i);
		Vector_copy(&V, &U);

		Vector_div(&U, &V);

		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(
				Vector_get_coeff(&U, j),
				1
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


MU_TEST(test_Vector_log) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)0, (real_t)1);
		Vector_log(&U);
		
		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(
				log(j),
				Vector_get_coeff(&U, j)
			);

		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_exp) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)0, (real_t)1);
		Vector_exp(&U);

		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(
				exp((real_t)j),
				Vector_get_coeff(&U, j)
			);

		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_inc) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		real_t k = 42;

		Vector_init(&U, i);

		Vector_arange(&U, (real_t)0, (real_t)1);
		Vector_inc(&U, k);
		
		for(size_t j = 0; j < i; ++j)
			mu_assert_double_eq(
				k + j,
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


MU_TEST(test_Vector_reduction_min) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)1, (real_t)1);
		real_t ret = Vector_reduction_min(&U);
		real_t ret_expected = Vector_get_coeff(&U, 0);
		mu_assert_double_eq(ret_expected, ret);
		
		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_reduction_max) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)1, (real_t)1);
		real_t ret = Vector_reduction_max(&U);
		real_t ret_expected = Vector_get_coeff(&U, i - 1);
		mu_assert_double_eq(ret_expected, ret);
		
		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_reduction_sum) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)1, (real_t)1);
		real_t ret = Vector_reduction_sum(&U);
		real_t ret_expected = (i * (i + 1)) / 2;
		mu_assert_double_eq(ret_expected, ret);
		
		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_reduction_square_sum) {
	Vector U;

	for(size_t i = 1; i < 256; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)1, (real_t)1);
		real_t ret = Vector_reduction_square_sum(&U);
		real_t ret_expected = (i * (i + 1) * (2 * i + 1)) / 6;
		mu_assert_double_eq(ret_expected, ret);
		
		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_reduction_logsumexp) {
	Vector U;

	for(size_t i = 1; i < 64; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)0, (real_t)1);
		real_t ret = Vector_reduction_logsumexp(&U);
		real_t ret_expected = log((1 - exp(i)) / (1 - exp(1)));
		mu_assert_double_eq(ret_expected, ret);
		
		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_reduction_mean) {
	Vector U;
	real_t ret, ret_expected, std;

	for(size_t i = 1; i < 64; ++i) {
		Vector_init(&U, i);

		Vector_arange(&U, (real_t)1, (real_t)1);

		ret = Vector_reduction_mean(&U, 0);
		ret_expected = (i * (i + 1)) / 2;
		ret_expected /= i;
		mu_assert_double_eq(ret_expected, ret);
		
		ret = Vector_reduction_mean(&U, &std);
		mu_assert_double_eq(ret_expected, ret);

		Vector_destroy(&U);
	}
}


MU_TEST(test_Vector_reduction_average) {
	Vector U, V;
	real_t ret, ret_expected, std;

	for(size_t i = 1; i < 64; ++i) {
		Vector_init(&U, i);
		Vector_init(&V, i);

		Vector_arange(&U, (real_t)1, (real_t)1);
		Vector_fill(&V, (real_t)1);

		ret = Vector_reduction_average(&U, &V, 0);
		ret_expected = (i * (i + 1)) / 2;
		ret_expected /= i;
		mu_assert_double_eq(ret_expected, ret);
		
		ret = Vector_reduction_average(&U, &V, &std);
		mu_assert_double_eq(ret_expected, ret);

		Vector_destroy(&U);
		Vector_destroy(&V);
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
						sum = fma(
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


MU_TEST(test_Vector_convolution__mirror) {
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

			Vector_convolution__mirror(&V, &U, &W);

			for(size_t k = 0; k < vector_len; ++k) {
				real_t sum = (real_t)0;
				for(size_t n = 0; n < kernel_len; ++n) {
					ssize_t index = k + n;
					index -= kernel_len / 2;

					if (index < 0)
						index = -index;

					if (index >= (ssize_t)vector_len) {
						index = -index;
						index += 2 * (vector_len - 1);
					}

					sum = fma(
						Vector_get_coeff(&U, index),
						Vector_get_coeff(&W, n),
						sum
					);
				}

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


MU_TEST(test_Matrix_log) {
	Matrix U;

	for(size_t i = 1; i < 32; i += 9) {
		for(size_t j = 1; j < 32; j += 9) {
			Matrix_init(&U, i, j);
			Matrix_filler(&U);
			Matrix_log(&U);

			size_t uk = 0;
			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj, ++uk)
					mu_assert_double_eq(
						log(uk),
						Matrix_get_coeff(&U, ui, uj)
					);

			Matrix_destroy(&U);
		}
	}
}


MU_TEST(test_Matrix_exp) {
	Matrix U;

	for(size_t i = 1; i < 32; i += 9) {
		for(size_t j = 1; j < 32; j += 9) {
			Matrix_init(&U, i, j);
			Matrix_filler(&U);
			Matrix_exp(&U);

			size_t uk = 0;
			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj, ++uk)
					mu_assert_double_eq(
						exp(((real_t)uk)),
						Matrix_get_coeff(&U, ui, uj)
					);

			Matrix_destroy(&U);
		}
	}
}


MU_TEST(test_Matrix_reduction_min) {
	Matrix U;

	for(size_t i = 1; i < 64; ++i) {
		for(size_t j = 1; j < 64; ++j) {
			Matrix_init(&U, i, j);
			Matrix_filler(&U);

			real_t ret = Matrix_reduction_min(&U);
			real_t ret_expected = Matrix_get_coeff(&U, 0, 0);
			mu_assert_double_eq(ret_expected, ret);
			
			Matrix_destroy(&U);
		}
	}
}


MU_TEST(test_Matrix_reduction_max) {
	Matrix U;

	for(size_t i = 1; i < 64; ++i) {
		for(size_t j = 1; j < 64; ++j) {
			Matrix_init(&U, i, j);
			Matrix_filler(&U);

			real_t ret = Matrix_reduction_max(&U);
			real_t ret_expected = Matrix_get_coeff(&U, i - 1, j - 1);
			mu_assert_double_eq(ret_expected, ret);
			
			Matrix_destroy(&U);
		}
	}
}


MU_TEST(test_Matrix_reduction_sum) {
	Matrix U;

	for(size_t i = 1; i < 64; ++i) {
		for(size_t j = 1; j < 64; ++j) {
			size_t k = i * j - 1;

			Matrix_init(&U, i, j);
			Matrix_filler(&U);

			real_t ret = Matrix_reduction_sum(&U);
			real_t ret_expected = (k * (k + 1)) / 2;
			mu_assert_double_eq(ret_expected, ret);

			Matrix_destroy(&U);
		}
	}
}


MU_TEST(test_Matrix_reduction_square_sum) {
	Matrix U;

	for(size_t i = 1; i < 16; ++i) {
		for(size_t j = 1; j < 16; ++j) {
			size_t k = i * j - 1;

			Matrix_init(&U, i, j);
			Matrix_filler(&U);

			real_t ret = Matrix_reduction_square_sum(&U);
			real_t ret_expected = (k * (k + 1) * (2 * k + 1)) / 6;
			mu_assert_double_eq(ret_expected, ret);

			Matrix_destroy(&U);
		}
	}
}


MU_TEST(test_Matrix_reduction_logsumexp) {
	Matrix U;

	for(size_t i = 1; i < 5; ++i) {
		for(size_t j = 1; j < 16; ++j) {
			size_t k = i * j;

			Matrix_init(&U, i, j);
			Matrix_filler(&U);

			real_t ret = Matrix_reduction_logsumexp(&U);
			real_t ret_expected = log((1 - exp(k)) / (1 - exp(1)));
			mu_assert_double_eq(ret_expected, ret);

			Matrix_destroy(&U);
		}
	}
}


MU_TEST(test_Matrix_reduction_mean) {
	Matrix U;

	for(size_t i = 1; i < 64; ++i) {
		for(size_t j = 1; j < 64; ++j) {
			size_t k = i * j - 1;

			Matrix_init(&U, i, j);
			Matrix_filler(&U);

			real_t ret = Matrix_reduction_mean(&U, 0);
			real_t ret_expected = (k * (k + 1)) / 2;
			ret_expected /= k + 1;
			mu_assert_double_eq(ret_expected, ret);

			Matrix_destroy(&U);
		}
	}
}


MU_TEST(test_Matrix_reduction_average) {
	Matrix U, V;

	for(size_t i = 1; i < 64; ++i) {
		for(size_t j = 1; j < 64; ++j) {
			size_t k = i * j - 1;

			Matrix_init(&U, i, j);
			Matrix_filler(&U);

			Matrix_init(&V, i, j);
			Matrix_fill(&V, (real_t)1);

			real_t ret = Matrix_reduction_average(&U, &V, 0);
			real_t ret_expected = (k * (k + 1)) / 2;
			ret_expected /= k + 1;
			mu_assert_double_eq(ret_expected, ret);

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


MU_TEST(test_Matrix_inc) {
	Matrix U;

	for(size_t i = 1; i < 256; i += 9) {
		for(size_t j = 1; j < 256; j += 9) {
			real_t k = 42;

			Matrix_init(&U, i, j);
			Matrix_filler(&U);
			Matrix_inc(&U, k);

			size_t uk = 0;
			for(size_t ui = 0; ui < i; ++ui)
				for(size_t uj = 0; uj < j; ++uj, ++uk)
					mu_assert_double_eq(
						k + uk,
						Matrix_get_coeff(&U, ui, uj)
					);

			Matrix_destroy(&U);
		}
	}
}


// --- Main entry point -------------------------------------------------------

MU_TEST_SUITE(test_special_suite) {
	MU_RUN_TEST(test_special_erfinv);
}


MU_TEST_SUITE(test_kahan_sum_suite) {
	MU_RUN_TEST(test_kahan_sum);
}


MU_TEST_SUITE(test_Vector_suite) {
	MU_RUN_TEST(test_Vector_fill);
	MU_RUN_TEST(test_Vector_copy);
	MU_RUN_TEST(test_Vector_add);
	MU_RUN_TEST(test_Vector_sub);
	MU_RUN_TEST(test_Vector_scaled_add);
	MU_RUN_TEST(test_Vector_mul);
	MU_RUN_TEST(test_Vector_div);	
	MU_RUN_TEST(test_Vector_square);
	MU_RUN_TEST(test_Vector_sqrt);
	MU_RUN_TEST(test_Vector_log);
	MU_RUN_TEST(test_Vector_exp);	
	MU_RUN_TEST(test_Vector_scale);
	MU_RUN_TEST(test_Vector_inc);
	MU_RUN_TEST(test_Vector_reduction_min);
	MU_RUN_TEST(test_Vector_reduction_max);
	MU_RUN_TEST(test_Vector_reduction_sum);
	MU_RUN_TEST(test_Vector_reduction_square_sum);
	MU_RUN_TEST(test_Vector_reduction_logsumexp);
	MU_RUN_TEST(test_Vector_reduction_mean);
	MU_RUN_TEST(test_Vector_reduction_average);
	MU_RUN_TEST(test_Vector_dot);
	MU_RUN_TEST(test_Vector_convolution__zero);
	MU_RUN_TEST(test_Vector_convolution__mirror);	
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
	MU_RUN_TEST(test_Matrix_inc);
	MU_RUN_TEST(test_Matrix_reduction_min);
	MU_RUN_TEST(test_Matrix_reduction_max);
	MU_RUN_TEST(test_Matrix_reduction_sum);
	MU_RUN_TEST(test_Matrix_reduction_square_sum);
	MU_RUN_TEST(test_Matrix_reduction_logsumexp);
	MU_RUN_TEST(test_Matrix_reduction_mean);
	MU_RUN_TEST(test_Matrix_reduction_average);
	MU_RUN_TEST(test_Matrix_log);
	MU_RUN_TEST(test_Matrix_exp);
}


int
main(ATTRIBUTE_UNUSED int argc, ATTRIBUTE_UNUSED char *argv[]) {
	MU_RUN_SUITE(test_special_suite);
	MU_RUN_SUITE(test_kahan_sum_suite);	
	MU_RUN_SUITE(test_Vector_suite);
	MU_RUN_SUITE(test_Matrix_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
