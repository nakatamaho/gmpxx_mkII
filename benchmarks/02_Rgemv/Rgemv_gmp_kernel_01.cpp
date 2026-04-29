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

#include "Rgemv.hpp"

#define MFLOPS 1e+6

void _Rgemv(int64_t m, int64_t n, const mpf_class &alpha, const mpf_class *A, int64_t lda, const mpf_class *x, int64_t incx, const mpf_class &beta, mpf_class *y, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    for (int64_t i = 0; i < m; ++i) {
        mpf_class temp = 0;
        for (int64_t j = 0; j < n; ++j) {
            temp += A[i + j * lda] * x[j];
        }
        y[i] = alpha * temp + beta * y[i];
    }
}

int main(int argc, char **argv) {
    // Initialize random state
    gmp_randclass r(gmp_randinit_default);
    r.seed(42);

    // Check command-line arguments
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <rows> <cols> <precision>" << std::endl;
        return EXIT_FAILURE;
    }

    int64_t M = std::atoll(argv[1]); // Number of rows
    int64_t N = std::atoll(argv[2]); // Number of columns
    int prec = std::atoi(argv[3]);
    mpf_set_default_prec(prec);
#if !defined USE_ORIGINAL_GMPXX
    gmpxx::gmpxx_defaults::set_initial_default_prec(prec);
#endif

    // Allocate memory for A, x, y, and yy
    mpf_class *A = new mpf_class[M * N];
    mpf_class *x = new mpf_class[N];
    mpf_class *y = new mpf_class[M];
    mpf_class *yy = new mpf_class[M];

    // Initialize scalars alpha and beta
    mpf_class alpha = r.get_f(prec);
    mpf_class beta = r.get_f(prec);

    // Initialize matrix A and vectors x, y, yy with random values
    for (int64_t i = 0; i < M; ++i) {
        for (int64_t j = 0; j < N; ++j) {
            A[i + j * M] = r.get_f(prec); // A[i][j] = A[i + j*lda]
        }
    }

    for (int64_t j = 0; j < N; ++j) {
        x[j] = r.get_f(prec);
    }

    for (int64_t i = 0; i < M; ++i) {
        y[i] = r.get_f(prec);
        yy[i] = y[i];
    }

    auto start = std::chrono::high_resolution_clock::now();
    _Rgemv(M, N, alpha, A, M, x, 1, beta, y, 1);
    auto end = std::chrono::high_resolution_clock::now();

    // Reference computation
    Rgemv("n", M, N, alpha, A, M, x, 1, beta, yy, 1);

    // Calculate elapsed time for reference implementation
    std::chrono::duration<double> elapsed = end - start;
    double mflops = (2.0 * double(M) * double(N)) / (elapsed.count() * MFLOPS);

    // Output performance metrics
    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    // Compute L1 norm of the difference between y and yy
    mpf_class l1_norm = 0;
    for (int64_t i = 0; i < M; ++i) {
        mpf_class diff = abs(y[i] - yy[i]);
        l1_norm += diff;
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
    delete[] x;
    delete[] y;
    delete[] yy;

    return EXIT_SUCCESS;
}
