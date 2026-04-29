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

}  // namespace

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>\n";
        return EXIT_FAILURE;
    }

    std::int64_t n = std::atoll(argv[1]);
    mp_bitcnt_t precision = static_cast<mp_bitcnt_t>(std::atoll(argv[2]));
    benchmark_kernels::set_default_precision(precision);

    benchmark_kernels::gmp_randclass random(gmp_randinit_default);
    random.seed(42);

    std::vector<mpf_class> x(static_cast<std::size_t>(n));
    std::vector<mpf_class> y(static_cast<std::size_t>(n));
    for (std::int64_t i = 0; i < n; ++i) {
        x[static_cast<std::size_t>(i)] = random.get_f(precision);
        y[static_cast<std::size_t>(i)] = random.get_f(precision);
    }

    auto start = std::chrono::high_resolution_clock::now();
    mpf_class got = benchmark_kernels::rdot(n, x.data(), 1, y.data(), 1);
    auto end = std::chrono::high_resolution_clock::now();
    mpf_class expected = benchmark_kernels::rdot_reference(n, x.data(), y.data());

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    std::cout << "MFLOPS: "
              << (2.0 * static_cast<double>(n) - 1.0) / elapsed.count() /
                     mflops
              << '\n';

    mpf_class diff = benchmark_kernels::abs_value(got - expected);
    std::cout << "DIFF: " << diff << '\n';
    std::cout << (diff < mpf_class("1e-5", precision) ? "Result OK\n"
                                                       : "Result NG\n");
    return diff < mpf_class("1e-5", precision) ? EXIT_SUCCESS : EXIT_FAILURE;
}
