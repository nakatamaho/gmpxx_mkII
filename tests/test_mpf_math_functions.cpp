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

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace {

void assert_mpf_equal(mpf_class const& got, mpf_class const& expected) {
    assert(mpf_cmp(got.get_mpf_t(), expected.get_mpf_t()) == 0);
}

void assert_mpf_close(
    mpf_class const& got,
    mpf_class const& expected,
    mp_bitcnt_t tolerance_bits) {
    mp_bitcnt_t prec = std::max(got.get_prec(), expected.get_prec());
    mpf_class diff = abs(got - expected);
    mpf_class tolerance(1, prec);
    mpf_div_2exp(tolerance.get_mpf_t(), tolerance.get_mpf_t(),
                 tolerance_bits);
    assert(diff < tolerance);
}

void test_compile_time_surface() {
    static_assert(std::same_as<
                  decltype(sqrt(std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(abs(std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(neg(std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(ceil(std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(floor(std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(trunc(std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(mpf_remainder(
                      std::declval<mpf_class const&>(),
                      std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(std::declval<mpf_class&>().set_epsilon()),
                  void>);
    static_assert(std::same_as<
                  decltype(hypot(std::declval<mpf_class const&>(),
                                  std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(sgn(std::declval<mpf_class const&>())),
                  int>);
    static_assert(std::same_as<
                  decltype(abs(std::declval<mpz_class const&>())),
                  mpz_class>);
    static_assert(std::same_as<
                  decltype(abs(std::declval<mpq_class const&>())),
                  mpq_class>);
    static_assert(std::same_as<
                  decltype(sqrt(std::declval<mpz_class const&>())),
                  mpz_class>);
    static_assert(std::same_as<
                  decltype(gcd(std::declval<mpz_class const&>(),
                                std::declval<mpz_class const&>())),
                  mpz_class>);
    static_assert(std::same_as<
                  decltype(lcm(std::declval<mpz_class const&>(),
                                std::declval<mpz_class const&>())),
                  mpz_class>);
    static_assert(std::same_as<
                  decltype(gmpxx::sqrt(std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(gmpxx::abs(std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(gmpxx::neg(std::declval<mpf_class const&>())),
                  mpf_class>);
}

void test_sqrt() {
    mpf_class four("4.0", static_cast<mp_bitcnt_t>(256));
    mpf_class result = sqrt(four);

    mpf_class expected(0.0, four.get_prec());
    mpf_sqrt(expected.get_mpf_t(), four.get_mpf_t());

    assert(result.get_prec() == four.get_prec());
    assert_mpf_equal(result, expected);

    mpf_class zero("0.0", static_cast<mp_bitcnt_t>(192));
    mpf_class zero_result = gmpxx::sqrt(zero);

    mpf_class zero_expected(0.0, zero.get_prec());
    mpf_sqrt(zero_expected.get_mpf_t(), zero.get_mpf_t());

    assert(zero_result.get_prec() == zero.get_prec());
    assert_mpf_equal(zero_result, zero_expected);
}

void test_abs() {
    mpf_class negative("-3.5", static_cast<mp_bitcnt_t>(320));
    mpf_class result = abs(negative);

    mpf_class expected(negative.get_prec());
    mpf_abs(expected.get_mpf_t(), negative.get_mpf_t());

    assert(result.get_prec() == negative.get_prec());
    assert_mpf_equal(result, expected);

    mpf_class positive("2.25", static_cast<mp_bitcnt_t>(128));
    mpf_class positive_result = gmpxx::abs(positive);

    mpf_class positive_expected(positive.get_prec());
    mpf_abs(positive_expected.get_mpf_t(), positive.get_mpf_t());

    assert(positive_result.get_prec() == positive.get_prec());
    assert_mpf_equal(positive_result, positive_expected);
}

void test_neg() {
    mpf_class negative("-3.5", static_cast<mp_bitcnt_t>(384));
    mpf_class result = neg(negative);

    mpf_class expected(negative.get_prec());
    mpf_neg(expected.get_mpf_t(), negative.get_mpf_t());

    assert(result.get_prec() == negative.get_prec());
    assert_mpf_equal(result, expected);

    mpf_class positive("2.25", static_cast<mp_bitcnt_t>(160));
    mpf_class positive_result = gmpxx::neg(positive);

    mpf_class positive_expected(positive.get_prec());
    mpf_neg(positive_expected.get_mpf_t(), positive.get_mpf_t());

    assert(positive_result.get_prec() == positive.get_prec());
    assert_mpf_equal(positive_result, positive_expected);
}

void test_rounding_functions() {
    mpf_class positive("123.456", static_cast<mp_bitcnt_t>(256));
    mpf_class negative("-123.456", static_cast<mp_bitcnt_t>(256));

    assert_mpf_equal(ceil(positive), mpf_class("124", positive.get_prec()));
    assert_mpf_equal(ceil(negative), mpf_class("-123", negative.get_prec()));
    assert_mpf_equal(floor(positive), mpf_class("123", positive.get_prec()));
    assert_mpf_equal(floor(negative), mpf_class("-124", negative.get_prec()));
    assert_mpf_equal(trunc(positive), mpf_class("123", positive.get_prec()));
    assert_mpf_equal(trunc(negative), mpf_class("-123", negative.get_prec()));

    assert(ceil(positive).get_prec() == positive.get_prec());
    assert(floor(negative).get_prec() == negative.get_prec());
    assert(trunc(negative).get_prec() == negative.get_prec());
}

void test_hypot_and_scaling() {
    mpf_class x("3.0", static_cast<mp_bitcnt_t>(192));
    mpf_class y("4.0", static_cast<mp_bitcnt_t>(384));
    mpf_class result = hypot(x, y);

    assert(result.get_prec() == y.get_prec());
    assert_mpf_equal(result, mpf_class("5.0", result.get_prec()));

    mpf_class value("2.0", static_cast<mp_bitcnt_t>(256));
    value.div_2exp(1);
    assert_mpf_equal(value, mpf_class("1.0", value.get_prec()));
    value.mul_2exp(1);
    assert_mpf_equal(value, mpf_class("2.0", value.get_prec()));
}

void test_epsilon_and_remainder() {
    mpf_class epsilon(0, static_cast<mp_bitcnt_t>(128));
    epsilon.set_epsilon();
    mpf_class expected_epsilon(1, epsilon.get_prec());
    mpf_div_2exp(expected_epsilon.get_mpf_t(),
                 expected_epsilon.get_mpf_t(),
                 epsilon.get_prec() - 1);
    assert_mpf_equal(epsilon, expected_epsilon);

    struct case_data {
        char const* x;
        char const* y;
        char const* quotient;
        char const* remainder;
    };

    case_data cases[] = {
        {"10.5", "3.2", "3", "0.9"},
        {"23.7", "4.5", "5", "1.2"},
        {"5.3", "2.1", "2", "1.1"},
        {"-15.8", "6.1", "-3", "2.5"},
        {"-7.6", "2.3", "-4", "1.6"},
    };

    for (case_data const& c : cases) {
        mpf_class x(c.x, static_cast<mp_bitcnt_t>(256));
        mpf_class y(c.y, static_cast<mp_bitcnt_t>(256));
        mpz_class quotient;
        mpf_class remainder = mpf_remainder(x, y, &quotient);

        assert(quotient == mpz_class(c.quotient));
        assert_mpf_close(remainder, mpf_class(c.remainder, remainder.get_prec()),
                         220);

        mpf_class reconstructed = quotient * y + remainder;
        assert_mpf_close(reconstructed, x, 220);
    }

    bool threw = false;
    try {
        (void)mpf_remainder(mpf_class("1"), mpf_class("0"));
    } catch (std::domain_error const&) {
        threw = true;
    }
    assert(threw);
}

void test_sign_and_exact_math_functions() {
    assert(sgn(mpf_class("123.456")) > 0);
    assert(sgn(mpf_class("-123.456")) < 0);
    assert(sgn(mpf_class("0")) == 0);

    assert(abs(mpz_class("-456")) == mpz_class("456"));
    assert(abs(mpq_class("-1/3")) == mpq_class("1/3"));
    assert(sqrt(mpz_class("24")) == mpz_class("4"));
    assert(gcd(mpz_class("24"), mpz_class("36")) == mpz_class("12"));
    assert(lcm(mpz_class("24"), mpz_class("36")) == mpz_class("72"));
    assert(factorial(mpz_class(std::int64_t{5})) == mpz_class("120"));
    assert(primorial(mpz_class(std::int64_t{5})) == mpz_class("30"));
    assert(fibonacci(mpz_class(std::int64_t{7})) == mpz_class("13"));
}

}  // namespace

int main() {
    test_compile_time_surface();
    test_sqrt();
    test_abs();
    test_neg();
    test_rounding_functions();
    test_hypot_and_scaling();
    test_epsilon_and_remainder();
    test_sign_and_exact_math_functions();
    return 0;
}
