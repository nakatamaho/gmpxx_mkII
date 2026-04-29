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

#include "native_kernels.hpp"

namespace {

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
    mp_bitcnt_t prec = static_cast<mp_bitcnt_t>(std::atoll(argv[4]));
    mpf_set_default_prec(prec);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    mpf_t alpha;
    mpf_t beta;
    mpf_init2(alpha, prec);
    mpf_init2(beta, prec);
    mpf_urandomb(alpha, state, prec);
    mpf_urandomb(beta, state, prec);

    mpf_t* a = new mpf_t[m * k];
    mpf_t* b = new mpf_t[k * n];
    mpf_t* c = new mpf_t[m * n];
    mpf_t* expected = new mpf_t[m * n];
    native_kernels::init_matrix(a, m, k, m, prec, state);
    native_kernels::init_matrix(b, k, n, k, prec, state);
    native_kernels::init_matrix(c, m, n, m, prec, state);
    native_kernels::copy_matrix(expected, c, m, n, m, prec);

    auto start = std::chrono::high_resolution_clock::now();
    native_kernels::rgemm(m, k, n, alpha, a, m, b, k, beta, c, m);
    auto end = std::chrono::high_resolution_clock::now();
    native_kernels::rgemm(m, k, n, alpha, a, m, b, k, beta, expected, m);

    mpf_t l1;
    mpf_t temp;
    mpf_init2(l1, prec);
    mpf_init2(temp, prec);
    mpf_set_ui(l1, 0);
    for (std::int64_t j = 0; j < n; ++j) {
        for (std::int64_t i = 0; i < m; ++i) {
            std::int64_t index = i + j * m;
            mpf_sub(temp, c[index], expected[index]);
            mpf_abs(temp, temp);
            mpf_add(l1, l1, temp);
        }
    }

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    std::cout << "MFLOPS: "
              << flops_gemm(m, k, n) / elapsed.count() /
                     native_kernels::MFLOPS
              << '\n';
    std::cout << "L1 Norm of difference: ";
    gmp_printf("%.4Fe\n", l1);

    bool ok = native_kernels::less_than_threshold(l1, "1e-5", prec);
    std::cout << (ok ? "Result OK\n" : "Result NG\n");

    mpf_clear(alpha);
    mpf_clear(beta);
    mpf_clear(l1);
    mpf_clear(temp);
    native_kernels::clear_matrix(a, m, k, m);
    native_kernels::clear_matrix(b, k, n, k);
    native_kernels::clear_matrix(c, m, n, m);
    native_kernels::clear_matrix(expected, m, n, m);
    delete[] a;
    delete[] b;
    delete[] c;
    delete[] expected;
    gmp_randclear(state);
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
