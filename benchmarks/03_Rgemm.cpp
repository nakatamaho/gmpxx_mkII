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

#include "benchmark_kernels.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace {

constexpr double mflops = 1.0e6;

using benchmark_kernels::mpf_class;

double flops_gemm(std::int64_t m, std::int64_t k, std::int64_t n) {
    return static_cast<double>(m) * static_cast<double>(n) *
           (2.0 * static_cast<double>(k) + 2.0);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0]
                  << " <rows m> <cols k> <cols n> <precision>\n";
        return EXIT_FAILURE;
    }

    std::int64_t m = std::atoll(argv[1]);
    std::int64_t k = std::atoll(argv[2]);
    std::int64_t n = std::atoll(argv[3]);
    mp_bitcnt_t precision = static_cast<mp_bitcnt_t>(std::atoll(argv[4]));
    benchmark_kernels::set_default_precision(precision);

    benchmark_kernels::gmp_randclass random(gmp_randinit_default);
    random.seed(42);

    mpf_class alpha = random.get_f(precision);
    mpf_class beta = random.get_f(precision);
    std::vector<mpf_class> a(static_cast<std::size_t>(m * k));
    std::vector<mpf_class> b(static_cast<std::size_t>(k * n));
    std::vector<mpf_class> c(static_cast<std::size_t>(m * n));
    std::vector<mpf_class> expected(static_cast<std::size_t>(m * n));

    for (std::int64_t j = 0; j < k; ++j) {
        for (std::int64_t i = 0; i < m; ++i) {
            a[static_cast<std::size_t>(i + j * m)] = random.get_f(precision);
        }
    }
    for (std::int64_t j = 0; j < n; ++j) {
        for (std::int64_t i = 0; i < k; ++i) {
            b[static_cast<std::size_t>(i + j * k)] = random.get_f(precision);
        }
    }
    for (std::int64_t j = 0; j < n; ++j) {
        for (std::int64_t i = 0; i < m; ++i) {
            c[static_cast<std::size_t>(i + j * m)] = random.get_f(precision);
            expected[static_cast<std::size_t>(i + j * m)] =
                c[static_cast<std::size_t>(i + j * m)];
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    benchmark_kernels::rgemm('N', 'N', m, n, k, alpha, a.data(), m, b.data(),
                             k, beta, c.data(), m);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_kernels::rgemm_reference(m, k, n, alpha, a.data(), m, b.data(),
                                       k, beta, expected.data(), m);

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    std::cout << "MFLOPS: " << flops_gemm(m, k, n) / elapsed.count() / mflops
              << '\n';

    mpf_class l1_norm(0, precision);
    for (std::int64_t j = 0; j < n; ++j) {
        for (std::int64_t i = 0; i < m; ++i) {
            std::size_t index = static_cast<std::size_t>(i + j * m);
            l1_norm += benchmark_kernels::abs_value(c[index] - expected[index]);
        }
    }

    std::cout << "L1 Norm of difference: " << l1_norm << '\n';
    std::cout << (l1_norm < mpf_class("1e-5", precision) ? "Result OK\n"
                                                         : "Result NG\n");
    return l1_norm < mpf_class("1e-5", precision) ? EXIT_SUCCESS
                                                  : EXIT_FAILURE;
}
