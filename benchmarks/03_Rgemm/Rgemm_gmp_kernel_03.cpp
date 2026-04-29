/*
 * Copyright (c) 2026
 *      Nakata, Maho
 *      All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

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
#if !defined USE_ORIGINAL_GMPXX
    gmpxx::gmpxx_defaults::set_initial_default_prec(prec);
#endif

    // Allocate memory for A (M x K), B (K x N), C (M x N), and reference C (C_ref)
    mpf_class *A = new mpf_class[M * K];
    mpf_class *B = new mpf_class[K * N];
    mpf_class *C = new mpf_class[M * N];
    mpf_class *C_ref = new mpf_class[M * N];

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

    // Output performance metrics
    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

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

    return EXIT_SUCCESS;
}
