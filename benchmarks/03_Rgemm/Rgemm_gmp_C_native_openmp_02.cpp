#include <iostream>
#include <chrono>
#include <gmp.h>

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
using namespace gmp;
#endif
#endif

#include "Rgemm.hpp" // Assuming you have an Rgemm.hpp similar to Rgemv.hpp

#define MFLOPS 1e+6

gmp_randstate_t state;

// cf. https://netlib.org/lapack/lawnspdf/lawn41.pdf p.120
double flops_gemm(int k_i, int m_i, int n_i) {
    double adds, muls, flops;
    double k, m, n;
    m = (double)m_i;
    n = (double)n_i;
    k = (double)k_i;
    muls = m * (k + 2) * n;
    adds = m * k * n;
    flops = muls + adds;
    return flops;
}

// Reference implementation using mpf_t for C = alpha * A * B + beta * C
void _Rgemm(int64_t m, int64_t k, int64_t n, const mpf_t alpha, const mpf_t *A, int64_t lda, const mpf_t *B, int64_t ldb, const mpf_t beta, mpf_t *C, int64_t ldc) {

    if (lda < m || ldb < k || ldc < m) {
        std::cerr << "Leading dimensions are too small." << std::endl;
        exit(EXIT_FAILURE);
    }
#pragma omp parallel
    {
        mpf_t sum, temp;
        mpf_init(sum);
        mpf_init(temp);

        // Scale C by beta: C = beta * C
#pragma omp for collapse(2)
        for (long i = 0; i < m; ++i) {
            for (long j = 0; j < n; ++j) {
                mpf_mul(C[i + j * ldc], beta, C[i + j * ldc]); // C[i + j*ldc] = beta * C[i + j*ldc]
            }
        }
        // Compute alpha * A * B and add to C
#pragma omp for collapse(2) nowait
        for (long j = 0; j < n; ++j) {
            for (long l = 0; l < k; ++l) {
                mpf_mul(temp, alpha, B[l + j * ldb]);
                for (long i = 0; i < m; ++i) {
                    mpf_mul(sum, temp, A[i + l * lda]);
                    mpf_add(C[i + j * ldc], C[i + j * ldc], sum);
                }
            }
        }
        mpf_clear(sum);
        mpf_clear(temp);
    }
}

// Initialize a matrix with random values
void init_mpf_mat(mpf_t *mat, int64_t rows, int64_t cols, int64_t ld, int prec) {
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            mpf_init2(mat[i + j * ld], prec);
            mpf_urandomb(mat[i + j * ld], state, prec); // 0 <= mat[i + j*ld] < 1
        }
    }
}

// Clear a matrix
void clear_mpf_mat(mpf_t *mat, int64_t rows, int64_t cols, int64_t ld) {
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            mpf_clear(mat[i + j * ld]);
        }
    }
}

int main(int argc, char **argv) {
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <rows m> <cols k> <cols n> <precision>" << std::endl;
        return EXIT_FAILURE;
    }

    int64_t m = std::atoll(argv[1]);
    int64_t k = std::atoll(argv[2]);
    int64_t n = std::atoll(argv[3]);
    int prec = std::atoi(argv[4]);
    mpf_set_default_prec(prec);

    int64_t lda = m; // Leading dimension for A
    int64_t ldb = k; // Leading dimension for B
    int64_t ldc = m; // Leading dimension for C

    // Allocate matrices and vectors
    mpf_t *A = new mpf_t[m * k];
    mpf_t *B = new mpf_t[k * n];
    mpf_t *C = new mpf_t[m * n];

    // Initialize alpha and beta
    mpf_t alpha, beta;
    mpf_init2(alpha, prec);
    mpf_urandomb(alpha, state, prec);
    mpf_init2(beta, prec);
    mpf_urandomb(beta, state, prec);

    // Initialize A, B, C with random values
    init_mpf_mat(A, m, k, lda, prec);
    init_mpf_mat(B, k, n, ldb, prec);
    init_mpf_mat(C, m, n, ldc, prec);

    // Initialize mpf_class versions for reference
    mpf_class *A_mpf_class = new mpf_class[m * k];
    mpf_class *B_mpf_class = new mpf_class[k * n];
    mpf_class *C_mpf_class = new mpf_class[m * n];
    mpf_class alpha_class = mpf_class(alpha);
    mpf_class beta_class = mpf_class(beta);

    for (int64_t j = 0; j < k; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            A_mpf_class[i + j * lda] = mpf_class(A[i + j * lda]);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < k; ++i) {
            B_mpf_class[i + j * ldb] = mpf_class(B[i + j * ldb]);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            C_mpf_class[i + j * ldc] = mpf_class(C[i + j * ldc]);
        }
    }

    // Perform _Rgemm
    auto start = std::chrono::high_resolution_clock::now();
    _Rgemm(m, k, n, alpha, A, lda, B, ldb, beta, C, ldc);
    auto end = std::chrono::high_resolution_clock::now();

    // Perform reference Rgemm
    Rgemm("n", "n", m, n, k, alpha_class, A_mpf_class, lda, B_mpf_class, ldb, beta_class, C_mpf_class, ldc);

    // Calculate elapsed time for _Rgemm
    std::chrono::duration<double> elapsed_seconds = end - start;
    // For matrix-matrix multiply, number of floating-point operations is 2 * m * n * k
    double mflops = flops_gemm(m, n, k) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    // Compute L1 norm of the difference
    mpf_class l1_norm = 0;
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            mpf_class diff = abs(mpf_class(C[i + j * ldc]) - C_mpf_class[i + j * ldc]);
            l1_norm += diff;
        }
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
    clear_mpf_mat(A, m, k, lda);
    clear_mpf_mat(B, k, n, ldb);
    clear_mpf_mat(C, m, n, ldc);
    mpf_clear(alpha);
    mpf_clear(beta);
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] A_mpf_class;
    delete[] B_mpf_class;
    delete[] C_mpf_class;

    gmp_randclear(state);

    return EXIT_SUCCESS;
}
