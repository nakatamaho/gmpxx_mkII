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
 * Example 14: the Madelung constant of the NaCl lattice.
 *
 * The NaCl Madelung lattice sum is
 *
 *     S = sum_{(i,j,k) != (0,0,0)} (-1)^{i+j+k}
 *         / sqrt(i^2 + j^2 + k^2),
 *
 * and the usual positive Madelung constant is alpha = -S.  Directly summing
 * cubes or spheres is a poor numerical experiment: the Coulomb sum is only
 * conditionally convergent, so the summation prescription is part of the
 * problem.  That is precisely why this is a useful multiprecision example.
 *
 * A clean scalar route starts from the Laplace transform
 *
 *     1 / r = 1 / sqrt(pi) integral_0^infinity u^{-1/2} exp(-r^2 u) du.
 *
 * The alternating three-dimensional lattice then factorizes into Jacobi
 * theta functions:
 *
 *     S = 1 / sqrt(pi) integral_0^infinity
 *         u^{-1/2} (theta_4(exp(-u))^3 - 1) du.
 *
 * After u = exp(t), the integral is over the whole real line.  For small u,
 * directly summing theta_4 causes catastrophic cancellation, so this example
 * switches to the modular form
 *
 *     theta_4(exp(-u)) =
 *         sqrt(pi/u) theta_2(exp(-pi^2/u)).
 *
 * This keeps the calculation GMP-only while still showing the central
 * numerical issue: the physically meaningful constant is not obtained by a
 * naive finite box, but by a summation method that respects the long-range
 * Coulomb structure.
 *
 * Primary references:
 *
 * - P. P. Ewald, "Die Berechnung optischer und elektrostatischer
 *   Gitterpotentiale", Annalen der Physik 369, 253-287, 1921.
 *   DOI: 10.1002/andp.19213690304
 *
 * - Wilbur B. Bridgman, "Calculation of Madelung constants by direct
 *   summation", Journal of Chemical Education 46, 592, 1969.
 *   DOI: 10.1021/ed046p592
 *
 * - E. L. Burrows and S. F. A. Kettle, "Madelung constants and other
 *   lattice sums", Journal of Chemical Education 52, 58, 1975.
 *   DOI: 10.1021/ed052p58
 */

#include "gmpxx_mkII.h"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

using gmpxx::mpf_class;

struct options {
    int decimal_digits = 45;
    int first_panels = 512;
    int max_panels = 2048;
    int reference_panels = 4096;
    int t_extent = 10;
    bool reference_panels_was_set = false;
};

void print_usage(char const* program) {
    std::cout
        << "Usage: " << program << " [options]\n"
        << "\n"
        << "NaCl Madelung constant example.\n"
        << "Computes the positive Madelung constant through a theta-function\n"
        << "lattice-sum transform, avoiding the ambiguous conditionally\n"
        << "convergent direct Coulomb box sum.\n"
        << "\n"
        << "Options:\n"
        << "  --help                 Show this help and exit.\n"
        << "  --digits N             Decimal digits to print/use "
           "(default: 45).\n"
        << "  --first-panels N       First Simpson panel count "
           "(default: 512).\n"
        << "  --max-panels N         Largest reported panel count "
           "(default: 2048).\n"
        << "                         If --reference-panels is omitted, "
           "the reference is\n"
        << "                         raised to at least 2*N.\n"
        << "  --reference-panels N   Simpson panel count used as reference "
           "(default: 4096).\n"
        << "  --extent N             Integrate over -N <= t <= N "
           "(default: 10).\n"
        << "\n"
        << "Examples:\n"
        << "  " << program << "\n"
        << "  " << program << " --max-panels 4096\n"
        << "  " << program
        << " --digits 60 --max-panels 4096 --reference-panels 8192\n";
}

int parse_positive_int(char const* option, char const* text) {
    try {
        std::string value(text);
        std::size_t parsed = 0;
        int result = std::stoi(value, &parsed);
        if (parsed != value.size() || result <= 0) {
            throw std::invalid_argument("not a positive integer");
        }
        return result;
    } catch (std::exception const&) {
        throw std::invalid_argument(std::string(option) +
                                    " expects a positive integer");
    }
}

char const* require_value(int argc, char** argv, int& index) {
    if (index + 1 >= argc) {
        throw std::invalid_argument(std::string(argv[index]) +
                                    " requires a value");
    }
    ++index;
    return argv[index];
}

options parse_options(int argc, char** argv) {
    options result;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            std::exit(0);
        } else if (arg == "--digits") {
            result.decimal_digits =
                parse_positive_int(argv[i], require_value(argc, argv, i));
        } else if (arg == "--first-panels") {
            result.first_panels =
                parse_positive_int(argv[i], require_value(argc, argv, i));
        } else if (arg == "--max-panels") {
            result.max_panels =
                parse_positive_int(argv[i], require_value(argc, argv, i));
        } else if (arg == "--reference-panels") {
            result.reference_panels =
                parse_positive_int(argv[i], require_value(argc, argv, i));
            result.reference_panels_was_set = true;
        } else if (arg == "--extent") {
            result.t_extent =
                parse_positive_int(argv[i], require_value(argc, argv, i));
        } else {
            throw std::invalid_argument("unknown option: " + arg);
        }
    }

    if (result.first_panels > result.max_panels) {
        throw std::invalid_argument(
            "--first-panels must be <= --max-panels");
    }
    if (!result.reference_panels_was_set &&
        result.reference_panels < result.max_panels * 2) {
        result.reference_panels = result.max_panels * 2;
    }
    if (result.reference_panels < result.max_panels) {
        throw std::invalid_argument(
            "--reference-panels must be >= --max-panels");
    }

    return result;
}

mp_bitcnt_t bits_for_decimal_digits(int digits, int guard_bits) {
    double raw_bits = std::ceil(static_cast<double>(digits) * std::log2(10.0));
    return static_cast<mp_bitcnt_t>(raw_bits) +
           static_cast<mp_bitcnt_t>(guard_bits);
}

mpf_class theta4_direct(mpf_class const& u, mp_bitcnt_t precision) {
    mpf_class sum(1, precision);
    mpf_class tolerance("1e-75", precision);

    for (int n = 1; n < 10000; ++n) {
        mpf_class n_value(n, precision);
        mpf_class term =
            mpf_class(2, precision) * gmpxx::exp(-n_value * n_value * u);
        if ((n % 2) != 0) {
            term = -term;
        }
        sum += term;
        if (gmpxx::abs(term) < tolerance) {
            break;
        }
    }

    return sum;
}

mpf_class theta4_modular(mpf_class const& u, mp_bitcnt_t precision) {
    mpf_class pi_value = gmpxx::const_pi(precision);
    mpf_class pi_squared = pi_value * pi_value;
    mpf_class q_exponent_scale = pi_squared / u;
    mpf_class theta2_sum(0, precision);
    mpf_class tolerance("1e-75", precision);

    for (int n = 0; n < 10000; ++n) {
        mpf_class half_index =
            mpf_class(n, precision) + mpf_class("0.5", precision);
        mpf_class term =
            gmpxx::exp(-half_index * half_index * q_exponent_scale);
        theta2_sum += term;
        if (term < tolerance) {
            break;
        }
    }

    mpf_class theta2 = mpf_class(2, precision) * theta2_sum;
    return gmpxx::sqrt(pi_value / u) * theta2;
}

mpf_class theta4(mpf_class const& u, mp_bitcnt_t precision) {
    if (u < mpf_class(1, precision)) {
        return theta4_modular(u, precision);
    }
    return theta4_direct(u, precision);
}

mpf_class madelung_integrand_t(mpf_class const& t, mp_bitcnt_t precision) {
    mpf_class u = gmpxx::exp(t);
    mpf_class theta = theta4(u, precision);
    mpf_class theta_cubed = theta * theta * theta;
    return gmpxx::exp(t / mpf_class(2, precision)) *
           (theta_cubed - mpf_class(1, precision));
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

mpf_class nacl_madelung_constant(int panels, int t_extent,
                                 mp_bitcnt_t precision) {
    mpf_class lower(-t_extent, precision);
    mpf_class upper(t_extent, precision);
    mpf_class integral = simpson_integral(
        madelung_integrand_t, lower, upper, panels, precision);
    mpf_class signed_sum = integral / gmpxx::sqrt(gmpxx::const_pi(precision));

    /*
     * For t -> -infinity, theta_4(exp(-exp(t))) tends to zero, so the
     * transformed integrand tends to -exp(t/2).  The omitted left tail is
     * therefore -2 exp(-T/2) in the signed sum integral, or
     * +2 exp(-T/2)/sqrt(pi) in the positive Madelung constant alpha = -S.
     * The right tail is already double-exponentially small at T = 10.
     */
    mpf_class left_tail = mpf_class(2, precision) *
                          gmpxx::exp(-mpf_class(t_extent, precision) /
                                      mpf_class(2, precision)) /
                          gmpxx::sqrt(gmpxx::const_pi(precision));
    return -signed_sum + left_tail;
}

void run_case(options const& opts) {
    mp_bitcnt_t precision = bits_for_decimal_digits(opts.decimal_digits, 160);
    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    mpf_class reference =
        nacl_madelung_constant(opts.reference_panels, opts.t_extent,
                               precision);
    int label_width = 24;
    int value_width = opts.decimal_digits + 8;

    std::cout << "\nprecision target = " << opts.decimal_digits
              << " decimal digits\n";
    std::cout << std::setw(label_width) << "case" << "  "
              << std::setw(value_width) << "alpha(NaCl)" << "  "
              << "|delta from reference|\n";

    for (int panels = opts.first_panels; panels <= opts.max_panels;
         panels *= 2) {
        mpf_class value =
            nacl_madelung_constant(panels, opts.t_extent, precision);
        std::cout << std::setw(label_width) << panels << "  "
                  << std::setw(value_width) << value << "  "
                  << gmpxx::abs(value - reference) << '\n';
        if (panels > opts.max_panels / 2) {
            break;
        }
    }

    std::string reference_label =
        "reference(" + std::to_string(opts.reference_panels) + ")";
    std::cout << std::setw(label_width) << reference_label << "  "
              << std::setw(value_width) << reference << '\n';
    mpf_class literature_value(
        "1.747564594633182190636212035544397403485", precision);
    std::cout << std::setw(label_width) << "literature" << "  "
              << std::setw(value_width) << literature_value << '\n';
    std::cout << " t extent = " << opts.t_extent << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    options opts;
    try {
        opts = parse_options(argc, argv);
    } catch (std::exception const& error) {
        std::cerr << "error: " << error.what() << "\n\n";
        print_usage(argv[0]);
        return 1;
    }

    std::cout << std::scientific << std::setprecision(opts.decimal_digits);
    std::cout << "NaCl Madelung constant by a theta-function lattice sum\n";
    std::cout << "The direct Coulomb sum is conditionally convergent; this\n";
    std::cout << "example uses a transformed integral with theta modularity.\n";

    run_case(opts);

    return 0;
}
