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

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>\n";
        return EXIT_FAILURE;
    }
    std::int64_t n = std::atoll(argv[1]);
    mp_bitcnt_t prec = static_cast<mp_bitcnt_t>(std::atoll(argv[2]));
    mpf_set_default_prec(prec);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    mpf_t* x = new mpf_t[n];
    mpf_t* y = new mpf_t[n];
    native_kernels::init_vector(x, n, prec, state);
    native_kernels::init_vector(y, n, prec, state);

    mpf_t got;
    mpf_t expected;
    mpf_t diff;
    mpf_init2(got, prec);
    mpf_init2(expected, prec);
    mpf_init2(diff, prec);

    auto start = std::chrono::high_resolution_clock::now();
    native_kernels::rdot(n, x, y, got);
    auto end = std::chrono::high_resolution_clock::now();
    native_kernels::rdot(n, x, y, expected);

    mpf_sub(diff, got, expected);
    mpf_abs(diff, diff);
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    std::cout << "MFLOPS: "
              << (2.0 * static_cast<double>(n) - 1.0) / elapsed.count() /
                     native_kernels::MFLOPS
              << '\n';
    std::cout << "DIFF: ";
    gmp_printf("%.4Fe\n", diff);

    bool ok = native_kernels::less_than_threshold(diff, "1e-5", prec);
    std::cout << (ok ? "Result OK\n" : "Result NG\n");

    mpf_clear(got);
    mpf_clear(expected);
    mpf_clear(diff);
    native_kernels::clear_vector(x, n);
    native_kernels::clear_vector(y, n);
    delete[] x;
    delete[] y;
    gmp_randclear(state);
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
