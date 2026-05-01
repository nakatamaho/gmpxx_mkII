// SPDX-License-Identifier: BSD-2-Clause
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
 * Example 15: SIAM 100-Digit Challenge Problem 5.
 *
 * The problem asks for the cubic polynomial p(z) that best approximates
 *
 *     f(z) = 1 / Gamma(z)
 *
 * on the complex unit disk in the supremum norm.  Since f - p is analytic,
 * the maximum modulus theorem moves the search for ||f - p||_infinity to
 * the unit circle.  Also, f(conj(z)) = conj(f(z)), so the best cubic can be
 * taken to have real coefficients:
 *
 *     p(z) = c0 + c1 z + c2 z^2 + c3 z^3,    c_k real.
 *
 * This example is deliberately not a proof-producing complex Remez solver
 * with interval certificates.  It is a compact multiprecision experiment
 * following the first stages of the full workflow:
 *
 *   1. build a candidate polynomial;
 *   2. sample f(exp(i t)) on the unit circle using gmpxx::mpfc_class;
 *   3. search for large-error points on the boundary;
 *   4. improve the sampled supremum norm;
 *   5. compare the result with the published norm.
 *
 * A complete treatment would continue by checking equioscillation of the
 * active extrema and proving the final bound, typically with interval
 * arithmetic.  Those last certification steps are intentionally outside this
 * small example.
 *
 * The point is the numerical structure.  The expensive part is not evaluating
 * a cubic; it is reliably evaluating 1/Gamma(z) and then optimizing a complex
 * uniform error.  A production solver would use a complex Chebyshev/Remez or
 * semi-infinite-programming method and then verify the active extrema.
 *
 * Primary references:
 *
 * - Folkmar Bornemann, Dirk Laurie, Stan Wagon, and Jorg Waldvogel,
 *   "The SIAM 100-Digit Challenge: A Study in High-Accuracy Numerical
 *   Computing", SIAM, 2004.
 *   DOI: 10.1137/1.9780898717969
 *
 * - Bernd Fischer and Jan Modersitzki, "An algorithm for complex linear
 *   approximation based on semi-infinite programming", Numerical Algorithms
 *   5, 287-297, 1993.
 *   DOI: 10.1007/BF02108463
 */

#include "gmpxx_mkII.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

namespace {

using gmpxx::mpf_class;
using gmpxx::mpfc_class;

struct boundary_sample {
    mpfc_class z;
    mpfc_class f;
};

struct max_error_result {
    mpf_class value;
    std::size_t index;
};

mp_bitcnt_t bits_for_decimal_digits(int digits, int guard_bits) {
    double raw_bits = std::ceil(static_cast<double>(digits) * std::log2(10.0));
    return static_cast<mp_bitcnt_t>(raw_bits) +
           static_cast<mp_bitcnt_t>(guard_bits);
}

mpfc_class polynomial(std::array<mpf_class, 4> const& coeffs,
                      mpfc_class const& z, mp_bitcnt_t precision) {
    mpfc_class result(coeffs[3], mpf_class(0, precision));
    result = result * z + mpfc_class(coeffs[2], mpf_class(0, precision));
    result = result * z + mpfc_class(coeffs[1], mpf_class(0, precision));
    result = result * z + mpfc_class(coeffs[0], mpf_class(0, precision));
    return result;
}

std::vector<boundary_sample> make_boundary_samples(std::size_t count,
                                                   mp_bitcnt_t precision) {
    std::vector<boundary_sample> samples;
    samples.reserve(count);

    mpf_class two_pi = gmpxx::two_pi(precision);
    for (std::size_t k = 0; k < count; ++k) {
        mpf_class theta =
            two_pi * mpf_class(static_cast<unsigned long>(k), precision) /
            mpf_class(static_cast<unsigned long>(count), precision);
        mpfc_class z(gmpxx::cos(theta), gmpxx::sin(theta));
        samples.push_back({z, gmpxx::reciprocal_gamma(z)});
    }

    return samples;
}

max_error_result sampled_sup_norm(std::array<mpf_class, 4> const& coeffs,
                                  std::vector<boundary_sample> const& samples,
                                  mp_bitcnt_t precision) {
    mpf_class best(-1, precision);
    std::size_t best_index = 0;

    for (std::size_t i = 0; i < samples.size(); ++i) {
        mpfc_class error = samples[i].f -
                           polynomial(coeffs, samples[i].z, precision);
        mpf_class magnitude = gmpxx::abs(error);
        if (magnitude > best) {
            best = magnitude;
            best_index = i;
        }
    }

    return {best, best_index};
}

std::array<mpf_class, 4> improve_coefficients(
    std::array<mpf_class, 4> coeffs,
    std::vector<boundary_sample> const& samples, mp_bitcnt_t precision) {
    mpf_class step("0.02", precision);
    mpf_class stop("1e-16", precision);
    max_error_result current = sampled_sup_norm(coeffs, samples, precision);

    while (step > stop) {
        bool improved = false;

        for (std::size_t k = 0; k < coeffs.size(); ++k) {
            for (int sign : {1, -1}) {
                std::array<mpf_class, 4> trial = coeffs;
                trial[k] += mpf_class(sign, precision) * step;
                max_error_result candidate =
                    sampled_sup_norm(trial, samples, precision);
                if (candidate.value < current.value) {
                    coeffs = trial;
                    current = candidate;
                    improved = true;
                }
            }
        }

        if (!improved) {
            step /= mpf_class(2, precision);
        }
    }

    return coeffs;
}

void print_coefficients(std::array<mpf_class, 4> const& coeffs) {
    for (std::size_t i = 0; i < coeffs.size(); ++i) {
        std::cout << "  c" << i << " = " << coeffs[i] << '\n';
    }
}

void run_case() {
    constexpr int decimal_digits = 50;
    constexpr std::size_t sample_count = 1440;
    mp_bitcnt_t precision = bits_for_decimal_digits(decimal_digits, 160);
    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    std::vector<boundary_sample> samples =
        make_boundary_samples(sample_count, precision);

    std::array<mpf_class, 4> initial = {
        mpf_class("0.0055", precision),
        mpf_class("1.102", precision),
        mpf_class("0.625", precision),
        mpf_class("-0.603", precision)};
    std::array<mpf_class, 4> coeffs =
        improve_coefficients(initial, samples, precision);

    max_error_result initial_error =
        sampled_sup_norm(initial, samples, precision);
    max_error_result final_error =
        sampled_sup_norm(coeffs, samples, precision);

    std::cout << "sample count on |z|=1 = " << sample_count << '\n';
    std::cout << "initial sampled norm   = " << initial_error.value << '\n';
    std::cout << "improved sampled norm  = " << final_error.value << '\n';
    std::cout << "published norm         = "
              << "0.2143352345904596394615264001847493961134072877895\n";
    std::cout << "\nreal cubic coefficients p(z)=c0+c1*z+c2*z^2+c3*z^3\n";
    print_coefficients(coeffs);

    mpf_class theta =
        gmpxx::two_pi(precision) *
        mpf_class(static_cast<unsigned long>(final_error.index), precision) /
        mpf_class(static_cast<unsigned long>(sample_count), precision);
    std::cout << "\nlargest sampled error at theta = " << theta << '\n';
}

}  // namespace

int main() {
    std::cout << std::scientific << std::setprecision(40);
    std::cout << "SIAM 100-Digit Challenge Problem 5 example\n";
    std::cout << "Cubic uniform approximation of 1/Gamma(z) on |z| <= 1\n";
    std::cout << "The maximum is sampled on the boundary |z| = 1.\n\n";

    run_case();

    return 0;
}
