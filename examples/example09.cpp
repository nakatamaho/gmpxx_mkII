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
 * Example 09: near-multiple-root perturbation.
 *
 * The polynomial
 *
 *     p(x) = (x - 1)^20 + 1e-40
 *
 * is a compact conditioning example.  In exact arithmetic, the perturbation
 * changes the 20-fold root at x = 1 into roots satisfying
 *
 *     (x - 1)^20 = -1e-40,
 *
 * so the roots lie on a circle of radius 1e-2 around x = 1.  The coefficient
 * perturbation is 1e-40, but the root displacement is about 1e-2.  This
 * exposes the same core issue as Wilkinson's polynomial: root location can be
 * dramatically more sensitive than coefficient size suggests.
 *
 * The example runs the same stored polynomial construction at 53, 100, 200,
 * and 500 bits.  At low precision the 1e-40 addition to the constant
 * coefficient can be rounded away, so the visible root circle is not a
 * property of the stored coefficients.  At higher precision the perturbation
 * survives and the Aberth-Ehrlich iteration recovers the expected geometry.
 *
 * This particular polynomial is an elementary didactic variant rather than a
 * separate named historical test.  The sensitivity context follows
 * Wilkinson's root-conditioning example; the simultaneous solver follows
 * Ehrlich and Aberth.
 *
 * Wilkinson reference:
 *
 * - James H. Wilkinson, "Rounding Errors in Algebraic Processes", originally
 *   published by HMSO/Prentice-Hall, 1963; SIAM Classics reissue, 2023.
 *   DOI: 10.1137/1.9781611977523
 *
 * Aberth-Ehrlich references:
 *
 * - Louis W. Ehrlich, "A modified Newton method for polynomials",
 *   Communications of the ACM 10(2), 107-108, 1967.
 *   DOI: 10.1145/363067.363115
 * - Oliver Aberth, "Iteration methods for finding all zeros of a polynomial
 *   simultaneously", Mathematics of Computation 27(122), 339-344, 1973.
 *   DOI: 10.1090/S0025-5718-1973-0329236-7
 */

#include "gmpxx_mkII.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

using gmpxx::mpf_class;
using gmpxx::mpfc_class;

mpfc_class make_complex(mpf_class const& real_value,
                        mpf_class const& imag_value) {
    return mpfc_class(real_value, imag_value);
}

mpfc_class make_real(mpf_class const& real_value, mp_bitcnt_t precision) {
    return make_complex(real_value, mpf_class(0, precision));
}

std::vector<mpf_class> shifted_power_coefficients(std::size_t degree,
                                                  mpf_class const& epsilon,
                                                  mp_bitcnt_t precision) {
    std::vector<mpf_class> coefficients(1, mpf_class(1, precision));

    for (std::size_t factor = 0; factor < degree; ++factor) {
        (void)factor;
        std::vector<mpf_class> next(coefficients.size() + 1,
                                    mpf_class(0, precision));
        for (std::size_t i = 0; i < coefficients.size(); ++i) {
            next[i] -= coefficients[i];
            next[i + 1] += coefficients[i];
        }
        coefficients = next;
    }

    coefficients[0] += epsilon;
    return coefficients;
}

mpfc_class evaluate_polynomial(std::vector<mpf_class> const& coefficients,
                               mpfc_class const& z, mp_bitcnt_t precision) {
    if (coefficients.empty()) {
        return make_real(mpf_class(0, precision), precision);
    }

    mpfc_class value = make_real(coefficients.back(), precision);
    for (std::size_t k = coefficients.size() - 1; k-- > 0;) {
        value *= z;
        value += coefficients[k];
    }
    return value;
}

mpfc_class evaluate_derivative(std::vector<mpf_class> const& coefficients,
                               mpfc_class const& z, mp_bitcnt_t precision) {
    if (coefficients.size() < 2) {
        return make_real(mpf_class(0, precision), precision);
    }

    std::size_t degree = coefficients.size() - 1;
    mpfc_class value = make_real(
        coefficients[degree] *
            mpf_class(static_cast<unsigned long>(degree), precision),
        precision);

    for (std::size_t k = degree - 1; k > 0; --k) {
        value *= z;
        mpf_class derivative_coefficient =
            coefficients[k] *
            mpf_class(static_cast<unsigned long>(k), precision);
        value += derivative_coefficient;
    }
    return value;
}

std::vector<mpfc_class> initial_circle(std::size_t degree,
                                       mpf_class const& center,
                                       mpf_class const& radius,
                                       mp_bitcnt_t precision) {
    std::vector<mpfc_class> roots;
    roots.reserve(degree);

    mpf_class two_pi = gmpxx::two_pi(precision);
    mpf_class pi = gmpxx::const_pi(precision);
    mpf_class degree_value(static_cast<unsigned long>(degree), precision);
    mpf_class angle_offset("0.07", precision);

    for (std::size_t i = 0; i < degree; ++i) {
        mpf_class index(static_cast<unsigned long>(i), precision);
        mpf_class angle = (pi + two_pi * index) / degree_value +
                          angle_offset;
        roots.push_back(make_complex(center + radius * gmpxx::cos(angle),
                                     radius * gmpxx::sin(angle)));
    }

    return roots;
}

std::vector<mpfc_class> solve_with_aberth(
    std::vector<mpf_class> const& coefficients,
    std::vector<mpfc_class> roots, mpf_class const& tolerance,
    int max_iterations, mp_bitcnt_t precision) {
    if (coefficients.size() < 2) {
        throw std::invalid_argument("polynomial degree must be at least one");
    }

    std::size_t degree = coefficients.size() - 1;
    if (roots.size() != degree) {
        throw std::invalid_argument("initial root count must match degree");
    }

    for (int iteration = 1; iteration <= max_iterations; ++iteration) {
        mpf_class max_update(0, precision);
        std::vector<mpfc_class> next_roots = roots;

        for (std::size_t i = 0; i < degree; ++i) {
            mpfc_class f =
                evaluate_polynomial(coefficients, roots[i], precision);
            mpfc_class df =
                evaluate_derivative(coefficients, roots[i], precision);
            mpfc_class ratio = f / df;

            mpfc_class one = make_real(mpf_class(1, precision), precision);
            mpfc_class repulsion =
                make_real(mpf_class(0, precision), precision);
            for (std::size_t j = 0; j < degree; ++j) {
                if (i != j) {
                    repulsion += one / (roots[i] - roots[j]);
                }
            }

            mpfc_class correction = ratio / (one - ratio * repulsion);
            next_roots[i] = roots[i] - correction;

            mpf_class update_size = gmpxx::abs(correction);
            if (update_size > max_update) {
                max_update = update_size;
            }
        }

        roots = next_roots;
        if (iteration <= 4 || iteration % 5 == 0) {
            std::cout << "  iteration " << std::setw(2) << iteration
                      << ", max correction = " << max_update << '\n';
        }
        if (max_update < tolerance) {
            break;
        }
    }

    return roots;
}

void sort_by_angle_around_one(std::vector<mpfc_class>& roots,
                              mp_bitcnt_t precision) {
    mpfc_class one(mpf_class(1, precision), mpf_class(0, precision));
    std::sort(roots.begin(), roots.end(),
              [&](mpfc_class const& a, mpfc_class const& b) {
                  return gmpxx::arg(a - one) < gmpxx::arg(b - one);
              });
}

void print_radius_summary(std::vector<mpfc_class> roots,
                          std::vector<mpf_class> const& coefficients,
                          mp_bitcnt_t precision) {
    mpfc_class one(mpf_class(1, precision), mpf_class(0, precision));
    sort_by_angle_around_one(roots, precision);

    mpf_class min_radius("1e100", precision);
    mpf_class max_radius(0, precision);
    mpf_class sum_radius(0, precision);
    mpf_class max_residual(0, precision);

    for (mpfc_class const& root : roots) {
        mpf_class radius = gmpxx::abs(root - one);
        min_radius = radius < min_radius ? radius : min_radius;
        max_radius = radius > max_radius ? radius : max_radius;
        sum_radius += radius;

        mpf_class residual =
            gmpxx::abs(evaluate_polynomial(coefficients, root, precision));
        max_residual = residual > max_residual ? residual : max_residual;
    }

    mpf_class mean_radius =
        sum_radius / mpf_class(static_cast<unsigned long>(roots.size()),
                               precision);

    std::cout << "  radius min/mean/max = " << min_radius << "  "
              << mean_radius << "  " << max_radius << '\n';
    std::cout << "  max |p(root)|       = " << max_residual << '\n';
    std::cout << "  first roots by angle around x=1:\n";
    std::cout << "    angle                  |root-1|"
                 "                 root\n";

    std::size_t shown = std::min<std::size_t>(roots.size(), 8);
    for (std::size_t i = 0; i < shown; ++i) {
        mpfc_class shifted = roots[i] - one;
        std::cout << "    " << std::setw(22) << gmpxx::arg(shifted)
                  << "  " << std::setw(22) << gmpxx::abs(shifted)
                  << "  " << roots[i] << '\n';
    }
}

void run_precision(mp_bitcnt_t precision) {
    constexpr std::size_t degree = 20;

    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    mpf_class epsilon("1e-40", precision);
    mpf_class expected_radius("1e-2", precision);
    mpf_class center(1, precision);
    std::vector<mpf_class> coefficients =
        shifted_power_coefficients(degree, epsilon, precision);
    mpf_class retained_epsilon = coefficients[0] - mpf_class(1, precision);

    std::cout << "\nprecision = " << precision << " bits\n";
    std::cout << "  retained constant perturbation = "
              << retained_epsilon << '\n';

    if (retained_epsilon == 0) {
        std::cout << "  perturbation is rounded away at this precision;"
                     " the 1e-2 root circle is not present in the stored"
                     " coefficients.\n";
        return;
    }

    std::cout << "  expected |root-1| = " << expected_radius << '\n';
    mpf_class initial_radius = expected_radius * mpf_class("1.2", precision);
    mpf_class tolerance("1e-35", precision);
    if (precision >= 256) {
        tolerance = mpf_class("1e-70", precision);
    }

    std::vector<mpfc_class> roots = solve_with_aberth(
        coefficients,
        initial_circle(degree, center, initial_radius, precision),
        tolerance, 40, precision);

    print_radius_summary(roots, coefficients, precision);
}

}  // namespace

int main() {
    constexpr std::array<mp_bitcnt_t, 4> precisions = {53, 100, 200, 500};

    std::cout << std::scientific << std::setprecision(18);
    std::cout << "Near-multiple-root perturbation example\n";
    std::cout << "p(x) = (x - 1)^20 + 1e-40\n";
    std::cout << "The exact roots satisfy |x - 1| = 1e-2.\n";

    for (mp_bitcnt_t precision : precisions) {
        run_precision(precision);
    }

    return 0;
}
