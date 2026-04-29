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

#include "gmpxx_mkII.h"

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {

mp_bitcnt_t bits_for_decimal_digits(int digits, int guard_bits) {
    double raw_bits = std::ceil(static_cast<double>(digits) * std::log2(10.0));
    return static_cast<mp_bitcnt_t>(raw_bits) +
           static_cast<mp_bitcnt_t>(guard_bits);
}

}  // namespace

int main() {
    constexpr int decimal_digits = 50;
    const mp_bitcnt_t precision = bits_for_decimal_digits(decimal_digits, 32);

    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    gmpxx::mpf_class x("1.0", precision);
    gmpxx::mpf_class previous("0.0", precision);
    gmpxx::mpf_class two("2.0", precision);
    gmpxx::mpf_class tolerance("1e-50", precision);

    std::cout << std::fixed << std::setprecision(decimal_digits);
    std::cout << "Newton iteration for sqrt(2)\n";

    int iteration = 0;
    do {
        previous = x;
        x = (x + two / x) / two;
        ++iteration;
        std::cout << "iteration " << std::setw(2) << iteration
                  << ": " << x << '\n';
    } while (gmpxx::abs(x - previous) > tolerance);

    gmpxx::mpf_class reference = gmpxx::sqrt(two);
    std::cout << "sqrt() result: " << reference << '\n';
    return 0;
}
