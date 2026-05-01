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
 * Example 06: Aberth-Ehrlich root finding with gmpxx::mpfc_class.
 *
 * This is the mpfc_class version of example05.  It keeps the same
 * mathematical method and polynomial, but the complex arithmetic is now
 * expressed through the library's complex GMP type.  The example therefore
 * demonstrates the intended high-level API: real coefficients are stored as
 * mpf_class, complex iterates as mpfc_class, and stream output follows the
 * std::complex-compatible style implemented by gmpxx_mkII.
 *
 * The Aberth-Ehrlich correction combines a Newton step with a sum over the
 * other current approximations.  That sum is the practical reason this method
 * is attractive for examples: all roots are advanced simultaneously, and the
 * code naturally exercises complex division, absolute value, expression
 * templates, and formatted I/O.
 *
 * First publications for the iteration:
 *
 * - Louis W. Ehrlich, "A modified Newton method for polynomials",
 *   Communications of the ACM 10(2), 107-108, 1967.
 *   DOI: 10.1145/363067.363115
 * - Oliver Aberth, "Iteration methods for finding all zeros of a polynomial
 *   simultaneously", Mathematics of Computation 27(122), 339-344, 1973.
 *   DOI: 10.1090/S0025-5718-1973-0329236-7
 */

#include "gmpxx_mkII.h"

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

using gmpxx::mpf_class;
using gmpxx::mpfc_class;

mp_bitcnt_t bits_for_decimal_digits(int digits, int guard_bits) {
    double raw_bits = std::ceil(static_cast<double>(digits) * std::log2(10.0));
    return static_cast<mp_bitcnt_t>(raw_bits) +
           static_cast<mp_bitcnt_t>(guard_bits);
}

mpfc_class make_complex(char const* real_value, char const* imag_value,
                        mp_bitcnt_t precision) {
    return mpfc_class(mpf_class(real_value, precision),
                      mpf_class(imag_value, precision));
}

mpfc_class make_complex(mpf_class const& real_value,
                        mpf_class const& imag_value) {
    return mpfc_class(real_value, imag_value);
}

mpf_class cauchy_radius(std::vector<mpf_class> const& coefficients,
                        mp_bitcnt_t precision) {
    if (coefficients.size() < 2) {
        throw std::invalid_argument("polynomial degree must be at least one");
    }

    mpf_class leading = gmpxx::abs(coefficients.back());
    if (leading == mpf_class(0, precision)) {
        throw std::invalid_argument("leading coefficient must be nonzero");
    }

    mpf_class max_ratio(0, precision);
    for (std::size_t i = 0; i + 1 < coefficients.size(); ++i) {
        mpf_class ratio = gmpxx::abs(coefficients[i]) / leading;
        if (ratio > max_ratio) {
            max_ratio = ratio;
        }
    }

    return mpf_class(1, precision) + max_ratio;
}

mpfc_class evaluate_polynomial(std::vector<mpf_class> const& coefficients,
                               mpfc_class const& z, mp_bitcnt_t precision) {
    if (coefficients.empty()) {
        return make_complex("0.0", "0.0", precision);
    }

    mpfc_class value =
        make_complex(coefficients.back(), mpf_class(0, precision));
    for (std::size_t k = coefficients.size() - 1; k-- > 0;) {
        value *= z;
        value += coefficients[k];
    }
    return value;
}

mpfc_class evaluate_derivative(std::vector<mpf_class> const& coefficients,
                               mpfc_class const& z, mp_bitcnt_t precision) {
    if (coefficients.size() < 2) {
        return make_complex("0.0", "0.0", precision);
    }

    std::size_t degree = coefficients.size() - 1;
    mpfc_class value =
        make_complex(coefficients[degree] *
                         mpf_class(static_cast<unsigned long>(degree),
                                   precision),
                     mpf_class(0, precision));

    for (std::size_t k = degree - 1; k > 0; --k) {
        value *= z;
        value += mpf_class(
            coefficients[k] *
            mpf_class(static_cast<unsigned long>(k), precision));
    }
    return value;
}

std::vector<mpfc_class>
initial_circle(std::size_t degree, mpf_class const& radius,
               mp_bitcnt_t precision) {
    std::vector<mpfc_class> roots;
    roots.reserve(degree);

    mpf_class two(2, precision);
    mpf_class quarter("0.25", precision);
    mpf_class pi = gmpxx::const_pi(precision);
    mpf_class denominator(static_cast<unsigned long>(degree), precision);

    for (std::size_t i = 0; i < degree; ++i) {
        mpf_class index(static_cast<unsigned long>(i), precision);
        mpf_class angle = two * pi * (index + quarter) / denominator;
        roots.push_back(make_complex(mpf_class(radius * gmpxx::cos(angle)),
                                     mpf_class(radius * gmpxx::sin(angle))));
    }

    return roots;
}

std::vector<mpfc_class> solve_with_aberth(
    std::vector<mpf_class> const& coefficients, mpf_class const& radius,
    mpf_class const& tolerance, int max_iterations, mp_bitcnt_t precision) {
    if (coefficients.size() < 2) {
        throw std::invalid_argument("polynomial degree must be at least one");
    }
    if (coefficients.back() == mpf_class(0, precision)) {
        throw std::invalid_argument("leading coefficient must be nonzero");
    }

    std::size_t degree = coefficients.size() - 1;
    std::vector<mpfc_class> roots = initial_circle(degree, radius, precision);

    for (int iteration = 1; iteration <= max_iterations; ++iteration) {
        mpf_class max_update(0, precision);
        std::vector<mpfc_class> next_roots = roots;

        for (std::size_t i = 0; i < degree; ++i) {
            mpfc_class f =
                evaluate_polynomial(coefficients, roots[i], precision);
            mpfc_class df =
                evaluate_derivative(coefficients, roots[i], precision);
            mpfc_class ratio = f / df;

            mpfc_class repulsion = make_complex("0.0", "0.0", precision);
            for (std::size_t j = 0; j < degree; ++j) {
                if (i != j) {
                    mpfc_class separation = roots[i] - roots[j];
                    repulsion += mpf_class(1, precision) / separation;
                }
            }

            mpfc_class one = make_complex("1.0", "0.0", precision);
            mpfc_class correction = ratio / (one - ratio * repulsion);
            next_roots[i] = roots[i] - correction;

            mpf_class update_size = gmpxx::abs(correction);
            if (update_size > max_update) {
                max_update = update_size;
            }
        }

        roots = next_roots;
        std::cout << "iteration " << std::setw(2) << iteration
                  << ", max correction = " << max_update << '\n';

        if (max_update < tolerance) {
            break;
        }
    }

    return roots;
}

void print_root(std::size_t index, mpfc_class const& root,
                std::vector<mpf_class> const& coefficients,
                mp_bitcnt_t precision) {
    mpfc_class residual = evaluate_polynomial(coefficients, root, precision);

    std::cout << "root " << index << ": " << root;
    std::cout << ", |f(root)| = " << gmpxx::abs(residual) << '\n';
}

}  // namespace

int main() {
    constexpr int decimal_digits = 60;
    const mp_bitcnt_t precision = bits_for_decimal_digits(decimal_digits, 96);

    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    std::vector<mpf_class> coefficients = {
        mpf_class(13), mpf_class(-11), mpf_class(7), mpf_class(-1),
        mpf_class(0),  mpf_class(2),   mpf_class(0), mpf_class(-3),
        mpf_class(0),  mpf_class(0),   mpf_class(1)};

    mpf_class radius = cauchy_radius(coefficients, precision);
    mpf_class tolerance("1e-50");

    std::cout << std::scientific << std::setprecision(decimal_digits);
    std::cout << "Aberth method using gmpxx::mpfc_class for f(z) = "
                 "z^10 - 3 z^7 + 2 z^5 - z^3 + 7 z^2 - 11 z + 13\n";
    std::cout << "coefficients a0..a10 ="
                 " {13, -11, 7, -1, 0, 2, 0, -3, 0, 0, 1}\n";
    std::cout << "initial radius = " << radius << '\n';

    std::vector<mpfc_class> roots =
        solve_with_aberth(coefficients, radius, tolerance, 80, precision);

    for (std::size_t i = 0; i < roots.size(); ++i) {
        print_root(i, roots[i], coefficients, precision);
    }

    return 0;
}
