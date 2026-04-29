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
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

using gmpxx::mpf_class;

mp_bitcnt_t bits_for_decimal_digits(int digits, int guard_bits) {
    double raw_bits = std::ceil(static_cast<double>(digits) * std::log2(10.0));
    return static_cast<mp_bitcnt_t>(raw_bits) +
           static_cast<mp_bitcnt_t>(guard_bits);
}

struct complex_mpf {
    mpf_class re;
    mpf_class im;
};

complex_mpf make_complex(char const* re, char const* im, mp_bitcnt_t prec) {
    return {mpf_class(re, prec), mpf_class(im, prec)};
}

complex_mpf make_complex(mpf_class const& re, mpf_class const& im) {
    return {re, im};
}

complex_mpf operator+(complex_mpf const& lhs, complex_mpf const& rhs) {
    return {lhs.re + rhs.re, lhs.im + rhs.im};
}

complex_mpf operator-(complex_mpf const& lhs, complex_mpf const& rhs) {
    return {lhs.re - rhs.re, lhs.im - rhs.im};
}

complex_mpf operator*(complex_mpf const& lhs, complex_mpf const& rhs) {
    return {lhs.re * rhs.re - lhs.im * rhs.im,
            lhs.re * rhs.im + lhs.im * rhs.re};
}

complex_mpf operator*(complex_mpf const& lhs, mpf_class const& rhs) {
    return {lhs.re * rhs, lhs.im * rhs};
}

complex_mpf operator/(complex_mpf const& lhs, complex_mpf const& rhs) {
    mpf_class denom = rhs.re * rhs.re + rhs.im * rhs.im;
    return {(lhs.re * rhs.re + lhs.im * rhs.im) / denom,
            (lhs.im * rhs.re - lhs.re * rhs.im) / denom};
}

mpf_class norm2(complex_mpf const& z) {
    return z.re * z.re + z.im * z.im;
}

mpf_class abs_complex(complex_mpf const& z) {
    return gmpxx::sqrt(norm2(z));
}

complex_mpf reciprocal(complex_mpf const& z) {
    mpf_class denom = norm2(z);
    return {z.re / denom, -z.im / denom};
}

mpf_class cauchy_radius(std::vector<mpf_class> const& coefficients,
                        mp_bitcnt_t prec) {
    if (coefficients.size() < 2) {
        throw std::invalid_argument("polynomial degree must be at least one");
    }

    mpf_class leading = gmpxx::abs(coefficients.back());
    if (leading == mpf_class(0, prec)) {
        throw std::invalid_argument("leading coefficient must be nonzero");
    }

    mpf_class max_ratio(0, prec);
    for (std::size_t i = 0; i + 1 < coefficients.size(); ++i) {
        mpf_class ratio = gmpxx::abs(coefficients[i]) / leading;
        if (ratio > max_ratio) {
            max_ratio = ratio;
        }
    }

    return mpf_class(1, prec) + max_ratio;
}

complex_mpf evaluate_polynomial(std::vector<mpf_class> const& coefficients,
                                complex_mpf const& z, mp_bitcnt_t prec) {
    if (coefficients.empty()) {
        return make_complex("0.0", "0.0", prec);
    }

    complex_mpf value = make_complex(coefficients.back(), mpf_class(0, prec));
    for (std::size_t k = coefficients.size() - 1; k-- > 0;) {
        value = value * z + make_complex(coefficients[k], mpf_class(0, prec));
    }
    return value;
}

complex_mpf evaluate_derivative(std::vector<mpf_class> const& coefficients,
                                complex_mpf const& z, mp_bitcnt_t prec) {
    if (coefficients.size() < 2) {
        return make_complex("0.0", "0.0", prec);
    }

    std::size_t degree = coefficients.size() - 1;
    complex_mpf value = make_complex(coefficients[degree] *
                                         mpf_class(static_cast<unsigned long>(
                                                       degree),
                                                   prec),
                                     mpf_class(0, prec));

    for (std::size_t k = degree - 1; k > 0; --k) {
        value = value * z +
                make_complex(coefficients[k] *
                                 mpf_class(static_cast<unsigned long>(k), prec),
                             mpf_class(0, prec));
    }
    return value;
}

std::vector<complex_mpf>
initial_circle(std::size_t degree, mpf_class const& radius, mp_bitcnt_t prec) {
    std::vector<complex_mpf> roots;
    roots.reserve(degree);

    mpf_class two(2, prec);
    mpf_class quarter("0.25", prec);
    mpf_class pi = gmpxx::const_pi(prec);
    mpf_class denominator(static_cast<unsigned long>(degree), prec);

    for (std::size_t i = 0; i < degree; ++i) {
        mpf_class index(static_cast<unsigned long>(i), prec);
        mpf_class angle = two * pi * (index + quarter) / denominator;
        roots.push_back(
            {radius * gmpxx::cos(angle), radius * gmpxx::sin(angle)});
    }

    return roots;
}

std::vector<complex_mpf> solve_with_aberth(
    std::vector<mpf_class> const& coefficients, mpf_class const& radius,
    mpf_class const& tolerance, int max_iterations, mp_bitcnt_t prec) {
    if (coefficients.size() < 2) {
        throw std::invalid_argument("polynomial degree must be at least one");
    }
    if (coefficients.back() == mpf_class(0, prec)) {
        throw std::invalid_argument("leading coefficient must be nonzero");
    }

    std::size_t degree = coefficients.size() - 1;
    std::vector<complex_mpf> roots = initial_circle(degree, radius, prec);

    for (int iteration = 1; iteration <= max_iterations; ++iteration) {
        mpf_class max_update(0, prec);
        std::vector<complex_mpf> next_roots = roots;

        for (std::size_t i = 0; i < degree; ++i) {
            complex_mpf f = evaluate_polynomial(coefficients, roots[i], prec);
            complex_mpf df = evaluate_derivative(coefficients, roots[i], prec);
            complex_mpf ratio = f / df;

            complex_mpf repulsion = make_complex("0.0", "0.0", prec);
            for (std::size_t j = 0; j < degree; ++j) {
                if (i != j) {
                    repulsion = repulsion + reciprocal(roots[i] - roots[j]);
                }
            }

            complex_mpf one = make_complex("1.0", "0.0", prec);
            complex_mpf correction = ratio / (one - ratio * repulsion);
            next_roots[i] = roots[i] - correction;

            mpf_class update_size = abs_complex(correction);
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

void print_root(std::size_t index, complex_mpf const& root,
                std::vector<mpf_class> const& coefficients,
                mp_bitcnt_t prec) {
    complex_mpf residual = evaluate_polynomial(coefficients, root, prec);

    std::cout << "root " << index << ": " << root.re;
    if (root.im >= mpf_class(0, prec)) {
        std::cout << " + " << root.im << " i";
    } else {
        std::cout << " - " << -root.im << " i";
    }
    std::cout << ", |f(root)| = " << abs_complex(residual) << '\n';
}

}  // namespace

int main() {
    constexpr int decimal_digits = 60;
    const mp_bitcnt_t precision = bits_for_decimal_digits(decimal_digits, 96);

    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    std::vector<mpf_class> coefficients = {
        mpf_class(13, precision), mpf_class(-11, precision),
        mpf_class(7, precision),  mpf_class(-1, precision),
        mpf_class(0, precision),  mpf_class(2, precision),
        mpf_class(0, precision),  mpf_class(-3, precision),
        mpf_class(0, precision),  mpf_class(0, precision),
        mpf_class(1, precision)};

    mpf_class radius = cauchy_radius(coefficients, precision);
    mpf_class tolerance("1e-50", precision);

    std::cout << std::scientific << std::setprecision(decimal_digits);
    std::cout << "Aberth method for f(z) = z^10 - 3 z^7 + 2 z^5"
                 " - z^3 + 7 z^2 - 11 z + 13\n";
    std::cout << "coefficients a0..a10 ="
                 " {13, -11, 7, -1, 0, 2, 0, -3, 0, 0, 1}\n";
    std::cout << "initial radius = " << radius << '\n';

    std::vector<complex_mpf> roots =
        solve_with_aberth(coefficients, radius, tolerance, 80, precision);

    for (std::size_t i = 0; i < roots.size(); ++i) {
        print_root(i, roots[i], coefficients, precision);
    }

    return 0;
}
