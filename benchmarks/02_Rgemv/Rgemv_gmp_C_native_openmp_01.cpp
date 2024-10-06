#include <iostream>
#include <chrono>
#include <gmp.h>

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
using namespace gmpxx;
#endif
#endif

#include "Rgemv.hpp"

#define MFLOPS 1e+6

gmp_randstate_t state;

#include <omp.h>

void _Rgemv(int64_t m, int64_t n, const mpf_t alpha, const mpf_t *A, int64_t lda, const mpf_t *x, int64_t incx, const mpf_t beta, mpf_t *y, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

#pragma omp parallel for
    for (int64_t i = 0; i < m; ++i) {
        mpf_mul(y[i], y[i], beta);
    }

#pragma omp parallel for
    for (int64_t i = 0; i < m; ++i) {
        mpf_t temp;
        mpf_init(temp);
        mpf_set_ui(temp, 0);
        for (int64_t j = 0; j < n; ++j) {
            mpf_t prod;
            mpf_init(prod);
            mpf_mul(prod, A[i + j * lda], x[j]);
            mpf_add(temp, temp, prod);
            mpf_clear(prod);
        }
        mpf_t scaled_temp;
        mpf_init(scaled_temp);
        mpf_mul(scaled_temp, alpha, temp);
        mpf_add(y[i], y[i], scaled_temp);
        mpf_clear(temp);
        mpf_clear(scaled_temp);
    }
}

// Initialize a matrix with random values
void init_mpf_mat(mpf_t *mat, int64_t m, int64_t n, int64_t lda, int prec) {
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            mpf_init2(mat[i + j * lda], prec);
            mpf_urandomb(mat[i + j * lda], state, prec); // 0 <= mat[i + j*lda] < 1
        }
    }
}

// Clear a matrix
void clear_mpf_mat(mpf_t *mat, int64_t m, int64_t n, int64_t lda) {
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            mpf_clear(mat[i + j * lda]);
        }
    }
}

int main(int argc, char **argv) {
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <rows m> <cols n> <precision>" << std::endl;
        return EXIT_FAILURE;
    }

    int64_t m = std::atoll(argv[1]);
    int64_t n = std::atoll(argv[2]);
    int prec = std::atoi(argv[3]);
    mpf_set_default_prec(prec);

    int64_t lda = m; // Leading dimension

    // Allocate matrices and vectors
    mpf_t *A = new mpf_t[m * n];
    mpf_t *x = new mpf_t[n];
    mpf_t *y = new mpf_t[m];

    // Initialize alpha and beta
    mpf_t alpha, beta;
    mpf_init2(alpha, prec);
    mpf_urandomb(alpha, state, prec);
    mpf_init2(beta, prec);
    mpf_urandomb(beta, state, prec);

    // Initialize A, x, y with random values
    init_mpf_mat(A, m, n, lda, prec);
    for (int64_t i = 0; i < n; ++i) {
        mpf_init2(x[i], prec);
        mpf_urandomb(x[i], state, prec);
    }
    for (int64_t i = 0; i < m; ++i) {
        mpf_init2(y[i], prec);
        mpf_urandomb(y[i], state, prec);
    }

    // Initialize mpf_class versions for reference
    mpf_class *A_mpf_class = new mpf_class[m * n];
    mpf_class *x_mpf_class = new mpf_class[n];
    mpf_class *y_mpf_class = new mpf_class[m];
    mpf_class alpha_class = mpf_class(alpha);
    mpf_class beta_class = mpf_class(beta);

    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            A_mpf_class[i + j * lda] = mpf_class(A[i + j * lda]);
        }
    }
    for (int64_t i = 0; i < n; ++i) {
        x_mpf_class[i] = mpf_class(x[i]);
    }
    for (int64_t i = 0; i < m; ++i) {
        y_mpf_class[i] = mpf_class(y[i]);
    }

    // Perform _Rgemv
    auto start = std::chrono::high_resolution_clock::now();
    _Rgemv(m, n, alpha, A, lda, x, 1, beta, y, 1);
    auto end = std::chrono::high_resolution_clock::now();

    // Perform reference Rgemv
    Rgemv("n", m, n, alpha_class, A_mpf_class, lda, x_mpf_class, 1, beta_class, y_mpf_class, 1);

    // Calculate elapsed time for _Rgemv
    std::chrono::duration<double> elapsed_seconds = end - start;
    double mflops = (2.0 * double(m) * double(n)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    // Compute L1 norm of the difference
    mpf_class l1_norm = 0;
    for (int64_t i = 0; i < m; ++i) {
        mpf_class diff = abs(mpf_class(y[i]) - y_mpf_class[i]);
        l1_norm += diff;
    }

    std::cout << "L1 Norm of difference: ";
    gmp_printf("%.4Fe\n", l1_norm.get_mpf_t());

    mpf_class threshold = 1e-5;
    if (l1_norm < threshold) {
        std::cout << "Result OK" << std::endl;
    } else {
        std::cout << "Result NG" << std::endl;
    }

    // Clear memory
    clear_mpf_mat(A, m, n, lda);
    for (int64_t i = 0; i < n; ++i) {
        mpf_clear(x[i]);
    }
    for (int64_t i = 0; i < m; ++i) {
        mpf_clear(y[i]);
    }
    mpf_clear(alpha);
    delete[] A;
    delete[] x;
    delete[] y;
    delete[] A_mpf_class;
    delete[] x_mpf_class;
    delete[] y_mpf_class;

    return EXIT_SUCCESS;
}
