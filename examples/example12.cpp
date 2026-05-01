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
 * Example 12: high-precision integer-relation detection.
 *
 * Integer-relation algorithms look for nonzero integers a_i such that
 *
 *     a_0 x_0 + a_1 x_1 + ... + a_{n-1} x_{n-1} = 0
 *
 * for numerically computed high-precision constants x_i.  This is a standard
 * experimental-mathematics workflow: compute constants to high precision,
 * then search for relations involving pi, zeta values, logarithms, and other
 * constants.
 *
 * This example uses a structured one-dimensional relation instead of a full
 * production PSLQ implementation.  Let A be a large integer and define
 *
 *     C = A pi^2 + log(2).
 *
 * Then [C, pi^2, log(2)] has the integer relation
 *
 *     C - A pi^2 - log(2) = 0.
 *
 * Recovering A from decimal approximations requires enough digits to make the
 * rounding of (C - log(2)) / pi^2 unambiguous.  With an A of roughly 10^200,
 * a 100-decimal-digit working value is not enough; about 300 digits recovers
 * a candidate, and 1000 digits gives a strong residual check.  This is the
 * same numerical regime in which PSLQ is useful: high precision is part of
 * the data, not an optional refinement.
 *
 * Standard PSLQ reference:
 *
 * - Helaman R. P. Ferguson, David H. Bailey, and Steve Arno, "Analysis of
 *   PSLQ, an integer relation finding algorithm", Mathematics of Computation
 *   68(225), 351-369, 1999.
 *   DOI: 10.1090/S0025-5718-99-00995-3
 */

#include "gmpxx_mkII.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>

namespace {

using gmpxx::mpf_class;
using gmpxx::mpz_class;

constexpr char relation_coefficient_text[] =
    "11111111111111111111111111111111111111111111111111"
    "11111111111111111111111111111111111111111111111111"
    "11111111111111111111111111111111111111111111111111"
    "11111111111111111111111111111111111111111111111111";

mp_bitcnt_t bits_for_decimal_digits(int digits, int guard_bits) {
    double raw_bits = std::ceil(static_cast<double>(digits) * std::log2(10.0));
    return static_cast<mp_bitcnt_t>(raw_bits) +
           static_cast<mp_bitcnt_t>(guard_bits);
}

mpf_class decimal_tolerance(int digits, mp_bitcnt_t precision) {
    mpf_class tolerance(1, precision);
    mpf_class ten(10, precision);
    for (int i = 0; i < digits; ++i) {
        tolerance /= ten;
    }
    return tolerance;
}

mpf_class round_to_decimal_digits(mpf_class const& value, int digits,
                                  mp_bitcnt_t precision) {
    mpf_class scale(1, precision);
    mpf_class ten(10, precision);
    for (int i = 0; i < digits; ++i) {
        scale *= ten;
    }

    mpf_class scaled = value * scale;
    mpf_class half("0.5", precision);
    mpf_class shifted(0, precision);
    if (scaled >= mpf_class(0, precision)) {
        shifted = scaled + half;
    } else {
        shifted = scaled - half;
    }
    mpz_class integer;
    mpz_set_f(integer.get_mpz_t(), shifted.get_mpf_t());
    mpf_class rounded(integer, precision);
    return rounded / scale;
}

mpz_class nearest_integer(mpf_class const& value) {
    mp_bitcnt_t precision = value.get_prec();
    mpf_class half("0.5", precision);
    mpf_class shifted(0, precision);
    if (value >= mpf_class(0, precision)) {
        shifted = value + half;
    } else {
        shifted = value - half;
    }
    return mpz_class(shifted);
}

struct constants {
    mpf_class c;
    mpf_class pi_squared;
    mpf_class log_two;
};

constants make_reference_constants(mp_bitcnt_t precision) {
    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    mpf_class coefficient(mpz_class(relation_coefficient_text), precision);
    mpf_class pi = gmpxx::const_pi(precision);
    mpf_class pi_squared = pi * pi;
    mpf_class log_two = gmpxx::log_two(precision);
    mpf_class c = coefficient * pi_squared + log_two;

    return {c, pi_squared, log_two};
}

mpf_class relation_residual(constants const& values,
                            mpz_class const& coefficient,
                            mp_bitcnt_t precision) {
    mpf_class coefficient_value(coefficient, precision);
    return gmpxx::abs(values.c - coefficient_value * values.pi_squared -
                      values.log_two);
}

void run_decimal_digits(int working_digits, int validation_digits) {
    constexpr mp_bitcnt_t reference_precision =
        static_cast<mp_bitcnt_t>(4096);
    const mp_bitcnt_t working_precision =
        bits_for_decimal_digits(working_digits, 64);

    constants reference = make_reference_constants(reference_precision);
    constants working = {
        round_to_decimal_digits(reference.c, working_digits,
                                reference_precision),
        round_to_decimal_digits(reference.pi_squared, working_digits,
                                reference_precision),
        round_to_decimal_digits(reference.log_two, working_digits,
                                reference_precision)};

    mpf_class quotient((working.c - working.log_two) / working.pi_squared,
                       working_precision);
    mpz_class recovered = nearest_integer(quotient);
    mpz_class expected(relation_coefficient_text);

    mpf_class working_residual =
        relation_residual(working, recovered, reference_precision);
    mpf_class validation_residual =
        relation_residual(reference, recovered, reference_precision);
    mpf_class validation_tolerance =
        decimal_tolerance(validation_digits, reference_precision);

    std::cout << "\nworking decimal digits = " << working_digits << '\n';
    std::cout << "  recovered A matches expected: "
              << (recovered == expected ? "yes" : "no") << '\n';
    std::cout << "  working residual               = "
              << working_residual << '\n';
    std::cout << "  validation residual at 1000 digits = "
              << validation_residual << '\n';
    std::cout << "  validation tolerance           = "
              << validation_tolerance << '\n';
    if (recovered == expected && working_digits >= validation_digits &&
        validation_residual < validation_tolerance) {
        std::cout << "  accepted relation: C - A*pi^2 - log(2) = 0\n";
    } else if (recovered == expected) {
        std::cout << "  candidate recovered; keep more digits for confidence\n";
    } else {
        std::cout << "  no reliable integer relation at this precision\n";
    }
}

}  // namespace

int main() {
    std::cout << std::scientific << std::setprecision(30);
    std::cout << "PSLQ-motivated integer-relation example\n";
    std::cout << "Relation: C - A*pi^2 - log(2) = 0\n";
    std::cout << "A is a " << std::string(relation_coefficient_text).size()
              << "-digit repunit integer.\n";

    run_decimal_digits(100, 900);
    run_decimal_digits(300, 900);
    run_decimal_digits(1000, 900);

    return 0;
}
