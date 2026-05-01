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
 * Example 10: Mignotte polynomial root separation.
 *
 * The Mignotte family
 *
 *     p_{n,a}(x) = x^n - 2 (a x - 1)^2
 *
 * has integer coefficients, but for suitable a and n it has two real roots
 * clustered very close to x = 1/a.  The input polynomial is therefore exact
 * and uncomplicated; the numerical difficulty is the root separation itself.
 *
 * Near x = 1/a, write y = a x - 1.  Then
 *
 *     ((1 + y) / a)^n = 2 y^2,
 *
 * so the two nearby roots satisfy approximately
 *
 *     |x - 1/a| ~= a^(-n/2 - 1) / sqrt(2),
 *
 * and their separation is about sqrt(2) * a^(-n/2 - 1).  With n = 20 and
 * a = 1000000, the expected separation is about 1.4e-66 although all
 * coefficients are moderate integers:
 *
 *     x^20 - 2000000000000 x^2 + 4000000 x - 2.
 *
 * This program brackets the two close real roots around 1/a and refines them
 * by bisection at several GMP precisions.  At insufficient precision the
 * bracket endpoints around 1/a collapse to the same stored mpf_class value,
 * making the close pair numerically invisible.
 *
 * Root-separation reference:
 *
 * - Yann Bugeaud and Maurice Mignotte, "On the distance between roots of
 *   integer polynomials", Proceedings of the Edinburgh Mathematical Society
 *   47(3), 553-556, 2004.
 *   DOI: 10.1017/S0013091503000257
 *
 * Multiprecision polynomial root-finding reference:
 *
 * - Dario A. Bini and Giuseppe Fiorentino, "Design, analysis, and
 *   implementation of a multiprecision polynomial rootfinder", Numerical
 *   Algorithms 23, 127-173, 2000.
 *   DOI: 10.1023/A:1019199917103
 */

#include "gmpxx_mkII.h"

#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>

namespace {

using gmpxx::mpf_class;

mpf_class pow_ui(mpf_class base, std::size_t exponent, mp_bitcnt_t precision) {
    mpf_class result(1, precision);
    while (exponent != 0) {
        if ((exponent & 1U) != 0U) {
            result *= base;
        }
        exponent >>= 1U;
        if (exponent != 0) {
            base *= base;
        }
    }
    return result;
}

mpf_class inverse_power(unsigned long base, std::size_t exponent,
                        mp_bitcnt_t precision) {
    return mpf_class(1, precision) /
           pow_ui(mpf_class(base, precision), exponent, precision);
}

mpf_class evaluate_mignotte(mpf_class const& x, std::size_t degree,
                            unsigned long a, mp_bitcnt_t precision) {
    mpf_class ax_minus_one = mpf_class(a, precision) * x -
                             mpf_class(1, precision);
    return pow_ui(x, degree, precision) -
           mpf_class(2, precision) * ax_minus_one * ax_minus_one;
}

struct close_pair {
    close_pair(mp_bitcnt_t precision)
        : left(0, precision),
          right(0, precision),
          separation(0, precision),
          left_residual(0, precision),
          right_residual(0, precision) {}

    bool resolved = false;
    mpf_class left;
    mpf_class right;
    mpf_class separation;
    mpf_class left_residual;
    mpf_class right_residual;
};

bool refine_bisection(mpf_class negative_endpoint,
                      mpf_class positive_endpoint, std::size_t degree,
                      unsigned long a, mp_bitcnt_t precision,
                      mpf_class& root) {
    mpf_class f_negative =
        evaluate_mignotte(negative_endpoint, degree, a, precision);
    mpf_class f_positive =
        evaluate_mignotte(positive_endpoint, degree, a, precision);
    if (!(f_negative < 0 && f_positive > 0)) {
        return false;
    }

    for (mp_bitcnt_t i = 0; i < precision + 32; ++i) {
        mpf_class midpoint = (negative_endpoint + positive_endpoint) /
                             mpf_class(2, precision);
        if (midpoint == negative_endpoint || midpoint == positive_endpoint) {
            root = midpoint;
            return true;
        }

        mpf_class f_midpoint =
            evaluate_mignotte(midpoint, degree, a, precision);
        if (f_midpoint < 0) {
            negative_endpoint = midpoint;
        } else {
            positive_endpoint = midpoint;
        }
    }

    root = (negative_endpoint + positive_endpoint) / mpf_class(2, precision);
    return true;
}

close_pair solve_close_pair(std::size_t degree, unsigned long a,
                            mp_bitcnt_t precision) {
    close_pair result(precision);

    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    mpf_class center = mpf_class(1, precision) / mpf_class(a, precision);
    mpf_class expected_separation =
        gmpxx::sqrt(mpf_class(2, precision)) *
        inverse_power(a, degree / 2 + 1, precision);
    mpf_class half_width = expected_separation * mpf_class(4, precision);

    mpf_class left_outer = center - half_width;
    mpf_class right_outer = center + half_width;
    if (left_outer == center || right_outer == center) {
        return result;
    }

    mpf_class f_center = evaluate_mignotte(center, degree, a, precision);
    if (!(f_center > 0)) {
        return result;
    }

    for (int expansion = 0; expansion < 8; ++expansion) {
        if (evaluate_mignotte(left_outer, degree, a, precision) < 0 &&
            evaluate_mignotte(right_outer, degree, a, precision) < 0) {
            break;
        }
        half_width *= mpf_class(2, precision);
        left_outer = center - half_width;
        right_outer = center + half_width;
        if (left_outer == center || right_outer == center) {
            return result;
        }
    }

    mpf_class left_root(0, precision);
    mpf_class right_root(0, precision);
    if (!refine_bisection(left_outer, center, degree, a, precision,
                         left_root)) {
        return result;
    }
    if (!refine_bisection(right_outer, center, degree, a, precision,
                         right_root)) {
        return result;
    }

    result.resolved = true;
    result.left = left_root;
    result.right = right_root;
    result.separation = right_root - left_root;
    result.left_residual =
        gmpxx::abs(evaluate_mignotte(left_root, degree, a, precision));
    result.right_residual =
        gmpxx::abs(evaluate_mignotte(right_root, degree, a, precision));
    return result;
}

void run_precision(mp_bitcnt_t precision) {
    constexpr std::size_t degree = 20;
    constexpr unsigned long a = 1000000;

    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    mpf_class center = mpf_class(1, precision) / mpf_class(a, precision);
    mpf_class expected_separation =
        gmpxx::sqrt(mpf_class(2, precision)) *
        inverse_power(a, degree / 2 + 1, precision);

    std::cout << "\nprecision = " << precision << " bits\n";
    std::cout << "  center 1/a                  = " << center << '\n';
    std::cout << "  asymptotic close separation = "
              << expected_separation << '\n';

    close_pair roots = solve_close_pair(degree, a, precision);
    if (!roots.resolved) {
        std::cout << "  close pair not resolved: endpoints around 1/a"
                     " collapse or fail to bracket distinct roots at this"
                     " precision.\n";
        return;
    }

    std::cout << "  left root                   = " << roots.left << '\n';
    std::cout << "  right root                  = " << roots.right << '\n';
    std::cout << "  left/right offset from 1/a  = "
              << (roots.left - center) << "  "
              << (roots.right - center) << '\n';
    std::cout << "  computed separation         = "
              << roots.separation << '\n';
    std::cout << "  |p(left)|, |p(right)|       = "
              << roots.left_residual << "  "
              << roots.right_residual << '\n';
}

}  // namespace

int main() {
    constexpr std::array<mp_bitcnt_t, 5> precisions = {53, 80, 128, 256, 512};

    std::cout << std::scientific << std::setprecision(24);
    std::cout << "Mignotte polynomial root-separation example\n";
    std::cout << "p_{20,1000000}(x) = x^20 - 2(1000000x - 1)^2\n";
    std::cout << "                   = x^20 - 2000000000000x^2"
                 " + 4000000x - 2\n";

    for (mp_bitcnt_t precision : precisions) {
        run_precision(precision);
    }

    return 0;
}
