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

/*
 * Example 13: SIAM 100-Digit Challenge Problem 1.
 *
 * The original endpoint-singular, infinitely oscillatory integral is
 *
 *     lim_{eps -> 0+} integral_eps^1 cos(x^{-1} log(x)) / x dx.
 *
 * With t = -log(x), this becomes
 *
 *     integral_0^infinity cos(t exp(t)) dt.
 *
 * Direct real-axis quadrature is unpleasant: the frequency grows without
 * bound.  A standard way to make the problem numerical is to integrate
 * exp(i z exp(z)) on a rectangular contour and move the infinite ray to
 * z = s + i pi/2.  Taking real parts gives two ordinary real integrals:
 *
 *     integral_0^{pi/2} exp(-y cos(y)) sin(y sin(y)) dy
 *   + integral_0^infinity exp(-s exp(s)) cos((pi/2) exp(s)) ds.
 *
 * The second integral decays double-exponentially.  This example uses
 * composite Simpson quadrature on the two transformed integrals.  The point
 * is not that Simpson's rule is special; the point is that the correct
 * transformation changes an essentially impossible real-axis integral into a
 * stable high-precision computation.
 *
 * Primary reference:
 *
 * - Folkmar Bornemann, Dirk Laurie, Stan Wagon, and Jorg Waldvogel,
 *   "The SIAM 100-Digit Challenge: A Study in High-Accuracy Numerical
 *   Computing", SIAM, 2004.
 *   DOI: 10.1137/1.9780898717969
 */

#include "gmpxx_mkII.h"

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {

using gmpxx::mpf_class;

mp_bitcnt_t bits_for_decimal_digits(int digits, int guard_bits) {
    double raw_bits = std::ceil(static_cast<double>(digits) * std::log2(10.0));
    return static_cast<mp_bitcnt_t>(raw_bits) +
           static_cast<mp_bitcnt_t>(guard_bits);
}

template <typename Function>
mpf_class simpson_integral(Function function, mpf_class const& lower,
                           mpf_class const& upper, int panels,
                           mp_bitcnt_t precision) {
    if ((panels % 2) != 0) {
        ++panels;
    }

    mpf_class h = (upper - lower) / mpf_class(panels, precision);
    mpf_class sum = function(lower, precision) + function(upper, precision);

    for (int i = 1; i < panels; ++i) {
        mpf_class x = lower + h * mpf_class(i, precision);
        mpf_class weight((i % 2) == 0 ? 2 : 4, precision);
        sum += weight * function(x, precision);
    }

    return sum * h / mpf_class(3, precision);
}

mpf_class vertical_integrand(mpf_class const& y, mp_bitcnt_t precision) {
    return gmpxx::exp(-y * gmpxx::cos(y)) *
           gmpxx::sin(y * gmpxx::sin(y));
}

mpf_class shifted_ray_integrand(mpf_class const& s, mp_bitcnt_t precision) {
    mpf_class exp_s = gmpxx::exp(s);
    mpf_class half_pi = gmpxx::const_pi(precision) / mpf_class(2, precision);
    return gmpxx::exp(-s * exp_s) * gmpxx::cos(half_pi * exp_s);
}

mpf_class siam_problem_one(int panels, mp_bitcnt_t precision) {
    mpf_class zero(0, precision);
    mpf_class half_pi = gmpxx::const_pi(precision) / mpf_class(2, precision);
    mpf_class upper_tail(6, precision);

    mpf_class vertical = simpson_integral(
        vertical_integrand, zero, half_pi, panels, precision);
    mpf_class shifted = simpson_integral(
        shifted_ray_integrand, zero, upper_tail, panels, precision);
    return vertical + shifted;
}

void run_case(int decimal_digits) {
    mp_bitcnt_t precision = bits_for_decimal_digits(decimal_digits, 128);
    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    mpf_class reference = siam_problem_one(2048, precision);

    std::cout << "\nprecision target = " << decimal_digits
              << " decimal digits\n";
    std::cout << " panels                  value"
                 "                         |delta from 2048|\n";

    for (int panels : {128, 256, 512, 1024}) {
        mpf_class value = siam_problem_one(panels, precision);
        std::cout << std::setw(7) << panels << "  "
                  << std::setw(38) << value << "  "
                  << gmpxx::abs(value - reference) << '\n';
    }

    std::cout << " reference(2048 panels) = " << reference << '\n';
}

}  // namespace

int main() {
    std::cout << std::scientific << std::setprecision(40);
    std::cout << "SIAM 100-Digit Challenge Problem 1 example\n";
    std::cout << "lim_{eps->0+} integral_eps^1 cos(x^{-1} log x)/x dx\n";
    std::cout << "Computed after t=-log(x) and contour deformation.\n";

    run_case(40);

    return 0;
}
