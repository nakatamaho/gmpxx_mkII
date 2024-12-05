#include <iostream>
#include <chrono>
#include <cstdlib>

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
using namespace gmpxx;
#endif
#endif

#include "Rgemm.hpp" // Ensure you have this header implemented

#define MFLOPS 1e+6

#include <omp.h>

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

// Reference implementation using mpf_class for C = alpha * A * B + beta * C
void _Rgemm(int64_t m, int64_t k, int64_t n, const mpf_class &alpha, const mpf_class *A, int64_t lda, const mpf_class *B, int64_t ldb, const mpf_class &beta, mpf_class *C, int64_t ldc) {

    // Scale C by beta: C = beta * C
#pragma omp parallel for collapse(2) schedule(static)
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            C[i + j * ldc] *= beta;
        }
    }

    // Compute alpha * A * B and add to C: C += alpha * A * B
    mpf_class temp, templ;
#pragma omp parallel for private(temp, templ) schedule(static)
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t l = 0; l < k; ++l) {
            temp = alpha;
            temp *= B[l + j * ldb];
            for (int64_t i = 0; i < m; ++i) {
                templ = temp;
                templ *= A[i + l * lda];
                C[i + j * ldc] += templ;
            }
        }
    }
}

void _Rgemm_wo_openmp(int64_t m, int64_t k, int64_t n, const mpf_class &alpha, const mpf_class *A, int64_t lda, const mpf_class *B, int64_t ldb, const mpf_class &beta, mpf_class *C, int64_t ldc) {

    // Scale C by beta: C = beta * C
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            C[i + j * ldc] *= beta;
        }
    }

    // Compute alpha * A * B and add to C: C += alpha * A * B
    mpf_class temp, templ;
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t l = 0; l < k; ++l) {
            temp = alpha;
            temp *= B[l + j * ldb];
            for (int64_t i = 0; i < m; ++i) {
                templ = temp;
                templ *= A[i + l * lda];
                C[i + j * ldc] += templ;
            }
        }
    }
}

int main(int argc, char **argv) {
    // Initialize random state
    gmp_randclass r(gmp_randinit_default);
    r.seed(42);

    // Check command-line arguments
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <rows m> <cols k> <cols n> <precision>" << std::endl;
        return EXIT_FAILURE;
    }

    int64_t M = std::atoll(argv[1]); // Number of rows in A and C
    int64_t K = std::atoll(argv[2]); // Number of columns in A and rows in B
    int64_t N = std::atoll(argv[3]); // Number of columns in B and C
    int prec = std::atoi(argv[4]);   // Precision in bits
    mpf_set_default_prec(prec);

    // Allocate memory for A (M x K), B (K x N), C (M x N), and reference C (C_ref)
    mpf_class *A = new mpf_class[M * K];
    mpf_class *B = new mpf_class[K * N];
    mpf_class *C = new mpf_class[M * N];
    mpf_class *C_ref = new mpf_class[M * N];
    mpf_class *C_wo_openmp = new mpf_class[M * N];

    // Initialize scalars alpha and beta with random values
    mpf_class alpha = r.get_f(prec);
    mpf_class beta = r.get_f(prec);

    // Initialize matrix A with random values
    for (int64_t i = 0; i < M; ++i) {
        for (int64_t j = 0; j < K; ++j) {
            A[i + j * M] = r.get_f(prec); // Column-major order
        }
    }

    // Initialize matrix B with random values
    for (int64_t i = 0; i < K; ++i) {
        for (int64_t j = 0; j < N; ++j) {
            B[i + j * K] = r.get_f(prec); // Column-major order
        }
    }

    // Initialize matrix C with random values and copy to C_ref for reference
    for (int64_t i = 0; i < M; ++i) {
        for (int64_t j = 0; j < N; ++j) {
            C[i + j * M] = r.get_f(prec);    // Column-major order
            C_ref[i + j * M] = C[i + j * M]; // Copy for reference
            C_wo_openmp[i + j * M] = C[i + j * M]; // Copy for reference
        }
    }

    // Perform _Rgemm
    auto start = std::chrono::high_resolution_clock::now();
    _Rgemm(M, K, N, alpha, A, M, B, K, beta, C, M);
    auto end = std::chrono::high_resolution_clock::now();

    // Perform reference computation using Rgemm
    Rgemm("n", "n", M, N, K, alpha, A, M, B, K, beta, C_ref, M);

    // Calculate elapsed time for _Rgemm
    std::chrono::duration<double> elapsed = end - start;
    // For matrix-matrix multiply, number of floating-point operations is 2 * M * N * K
    double mflops = flops_gemm(M, N, K) / (elapsed.count() * MFLOPS);

    auto start_wo_openmp = std::chrono::high_resolution_clock::now();
    _Rgemm_wo_openmp(M, K, N, alpha, A, M, B, K, beta, C_wo_openmp, M);
    auto end_wo_openmp = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_wo_openmp = end_wo_openmp - start_wo_openmp;
    double mflops_wo_openmp = flops_gemm(M, N, K) / (elapsed_wo_openmp.count() * MFLOPS);

    // Output performance metrics
    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << " " << mflops_wo_openmp << std::endl;

    // Compute L1 norm of the difference between C and C_ref
    mpf_class l1_norm = 0;
    for (int64_t i = 0; i < M; ++i) {
        for (int64_t j = 0; j < N; ++j) {
            mpf_class diff = abs(C[i + j * M] - C_ref[i + j * M]);
            l1_norm += diff;
        }
    }

    // Output L1 norm
    std::cout << "L1 Norm of difference: ";
    gmp_printf("%.4Fg\n", l1_norm.get_mpf_t());

    // Verify correctness
    mpf_class threshold = 1e-5;
    if (l1_norm < threshold) {
        std::cout << "Result OK" << std::endl;
    } else {
        std::cout << "Result NG" << std::endl;
    }

    // Clean up
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] C_ref;
    delete[] C_wo_openmp;

    return EXIT_SUCCESS;
}
