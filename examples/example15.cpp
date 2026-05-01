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
#include <cstdlib>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
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

struct options {
    int decimal_digits = 50;
    int guard_bits = 160;
    std::size_t sample_count = 1440;
    std::string initial_step = "0.02";
    std::string stop_step = "1e-16";
    int max_sweeps = 0;
    std::array<std::string, 4> initial_coeffs = {
        "0.0055", "1.102", "0.625", "-0.603"};
};

void print_usage(char const* program) {
    std::cout
        << "Usage: " << program << " [options]\n"
        << "\n"
        << "SIAM 100-Digit Challenge Problem 5 sampled experiment.\n"
        << "Approximates 1/Gamma(z) on |z|<=1 by a real-coefficient cubic\n"
        << "using boundary sampling and coordinate pattern search.\n"
        << "\n"
        << "Options:\n"
        << "  --help          Show this help and exit.\n"
        << "  --digits N      Decimal digits to print/use (default: 50).\n"
        << "  --guard-bits N  Extra precision bits (default: 160).\n"
        << "  --samples N     Boundary sample count on |z|=1 "
           "(default: 1440).\n"
        << "  --step X        Initial coordinate-search step "
           "(default: 0.02).\n"
        << "  --stop X        Stop when step <= X (default: 1e-16).\n"
        << "  --max-sweeps N  Maximum coordinate-search sweeps; 0 means "
           "unlimited (default: 0).\n"
        << "  --c0 X          Initial constant coefficient.\n"
        << "  --c1 X          Initial z coefficient.\n"
        << "  --c2 X          Initial z^2 coefficient.\n"
        << "  --c3 X          Initial z^3 coefficient.\n"
        << "\n"
        << "Examples:\n"
        << "  " << program << "\n"
        << "  " << program << " --digits 60 --samples 2880\n"
        << "  " << program
        << " --samples 720 --step 0.01 --stop 1e-12 --max-sweeps 200\n";
}

int parse_nonnegative_int(char const* option, char const* text) {
    try {
        std::string value(text);
        std::size_t parsed = 0;
        int result = std::stoi(value, &parsed);
        if (parsed != value.size() || result < 0) {
            throw std::invalid_argument("not a non-negative integer");
        }
        return result;
    } catch (std::exception const&) {
        throw std::invalid_argument(std::string(option) +
                                    " expects a non-negative integer");
    }
}

int parse_positive_int(char const* option, char const* text) {
    int result = parse_nonnegative_int(option, text);
    if (result == 0) {
        throw std::invalid_argument(std::string(option) +
                                    " expects a positive integer");
    }
    return result;
}

std::size_t parse_positive_size(char const* option, char const* text) {
    try {
        std::string value(text);
        std::size_t parsed = 0;
        unsigned long result = std::stoul(value, &parsed);
        if (parsed != value.size() || result == 0) {
            throw std::invalid_argument("not a positive integer");
        }
        return static_cast<std::size_t>(result);
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
        } else if (arg == "--guard-bits") {
            result.guard_bits =
                parse_nonnegative_int(argv[i], require_value(argc, argv, i));
        } else if (arg == "--samples") {
            result.sample_count =
                parse_positive_size(argv[i], require_value(argc, argv, i));
        } else if (arg == "--step") {
            result.initial_step = require_value(argc, argv, i);
        } else if (arg == "--stop") {
            result.stop_step = require_value(argc, argv, i);
        } else if (arg == "--max-sweeps") {
            result.max_sweeps =
                parse_nonnegative_int(argv[i], require_value(argc, argv, i));
        } else if (arg == "--c0") {
            result.initial_coeffs[0] = require_value(argc, argv, i);
        } else if (arg == "--c1") {
            result.initial_coeffs[1] = require_value(argc, argv, i);
        } else if (arg == "--c2") {
            result.initial_coeffs[2] = require_value(argc, argv, i);
        } else if (arg == "--c3") {
            result.initial_coeffs[3] = require_value(argc, argv, i);
        } else {
            throw std::invalid_argument("unknown option: " + arg);
        }
    }

    return result;
}

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
    std::vector<boundary_sample> const& samples, options const& opts,
    mp_bitcnt_t precision, int& sweeps_used, mpf_class& final_step) {
    mpf_class step(opts.initial_step, precision);
    mpf_class stop(opts.stop_step, precision);
    max_error_result current = sampled_sup_norm(coeffs, samples, precision);
    sweeps_used = 0;

    while (step > stop) {
        if (opts.max_sweeps > 0 && sweeps_used >= opts.max_sweeps) {
            break;
        }
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
        ++sweeps_used;
    }

    final_step = step;
    return coeffs;
}

void print_coefficients(std::array<mpf_class, 4> const& coeffs) {
    for (std::size_t i = 0; i < coeffs.size(); ++i) {
        std::cout << "  c" << i << " = " << coeffs[i] << '\n';
    }
}

void run_case(options const& opts) {
    mp_bitcnt_t precision =
        bits_for_decimal_digits(opts.decimal_digits, opts.guard_bits);
    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    std::vector<boundary_sample> samples =
        make_boundary_samples(opts.sample_count, precision);

    std::array<mpf_class, 4> initial = {
        mpf_class(opts.initial_coeffs[0], precision),
        mpf_class(opts.initial_coeffs[1], precision),
        mpf_class(opts.initial_coeffs[2], precision),
        mpf_class(opts.initial_coeffs[3], precision)};
    int sweeps_used = 0;
    mpf_class final_step(0, precision);
    std::array<mpf_class, 4> coeffs = improve_coefficients(
        initial, samples, opts, precision, sweeps_used, final_step);

    max_error_result initial_error =
        sampled_sup_norm(initial, samples, precision);
    max_error_result final_error =
        sampled_sup_norm(coeffs, samples, precision);

    std::cout << "decimal digits          = " << opts.decimal_digits << '\n';
    std::cout << "working precision       = " << precision << " bits\n";
    std::cout << "sample count on |z|=1   = " << opts.sample_count << '\n';
    std::cout << "initial step            = "
              << mpf_class(opts.initial_step, precision) << '\n';
    std::cout << "stop step               = "
              << mpf_class(opts.stop_step, precision) << '\n';
    std::cout << "sweeps used             = " << sweeps_used << '\n';
    std::cout << "final step              = " << final_step << '\n';
    std::cout << "initial sampled norm    = " << initial_error.value << '\n';
    std::cout << "improved sampled norm   = " << final_error.value << '\n';
    std::cout << "published norm          = "
              << mpf_class(
                     "0.2143352345904596394615264001847493961134072877895",
                     precision)
              << '\n';
    std::cout << "\nreal cubic coefficients p(z)=c0+c1*z+c2*z^2+c3*z^3\n";
    print_coefficients(coeffs);

    mpf_class theta =
        gmpxx::two_pi(precision) *
        mpf_class(static_cast<unsigned long>(final_error.index), precision) /
        mpf_class(static_cast<unsigned long>(opts.sample_count), precision);
    std::cout << "\nlargest sampled error at theta = " << theta << '\n';
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
    std::cout << "SIAM 100-Digit Challenge Problem 5 example\n";
    std::cout << "Cubic uniform approximation of 1/Gamma(z) on |z| <= 1\n";
    std::cout << "The maximum is sampled on the boundary |z| = 1.\n\n";

    run_case(opts);

    return 0;
}
