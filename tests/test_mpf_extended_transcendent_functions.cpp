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
#include <stdexcept>
#include <utility>

namespace {

mpf_class two_to_minus(mp_bitcnt_t bits, mp_bitcnt_t prec) {
    mpf_class result(1, prec);
    mpf_div_2exp(result.get_mpf_t(), result.get_mpf_t(), bits);
    return result;
}

void assert_close(mpf_class const& got, mpf_class const& expected,
                  mp_bitcnt_t tolerance_bits) {
    const mp_bitcnt_t prec = std::max(got.get_prec(), expected.get_prec());
    mpf_class diff(0, prec);
    mpf_sub(diff.get_mpf_t(), got.get_mpf_t(), expected.get_mpf_t());
    mpf_abs(diff.get_mpf_t(), diff.get_mpf_t());
    assert(diff < two_to_minus(tolerance_bits, prec));
}

void expect_domain_error(auto&& fn) {
    bool threw = false;
    try {
        fn();
    } catch (std::domain_error const&) {
        threw = true;
    }
    assert(threw);
}

void test_compile_time_surface() {
    static_assert(std::same_as<decltype(e(std::declval<mp_bitcnt_t>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(const_e()), mpf_class>);
    static_assert(std::same_as<decltype(const_e(std::declval<mp_bitcnt_t>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(inv_log_two()), mpf_class>);
    static_assert(std::same_as<
                  decltype(inv_log_two(std::declval<mp_bitcnt_t>())),
                  mpf_class>);
    static_assert(std::same_as<decltype(log_ten(std::declval<mp_bitcnt_t>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(const_log10()), mpf_class>);
    static_assert(std::same_as<
                  decltype(const_log10(std::declval<mp_bitcnt_t>())),
                  mpf_class>);
    static_assert(std::same_as<decltype(pi_over_two()), mpf_class>);
    static_assert(std::same_as<
                  decltype(pi_over_two(std::declval<mp_bitcnt_t>())),
                  mpf_class>);
    static_assert(std::same_as<decltype(pi_over_four()), mpf_class>);
    static_assert(std::same_as<
                  decltype(pi_over_four(std::declval<mp_bitcnt_t>())),
                  mpf_class>);
    static_assert(std::same_as<decltype(two_pi()), mpf_class>);
    static_assert(std::same_as<decltype(two_pi(std::declval<mp_bitcnt_t>())),
                               mpf_class>);

    static_assert(std::same_as<decltype(asin(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(acos(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(sinh(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(cosh(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(tanh(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(asinh(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(acosh(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(atanh(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(exp2(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(exp10(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(gamma(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<
                  decltype(reciprocal_gamma(std::declval<mpf_class const&>())),
                  mpf_class>);

    using expr_type = decltype(std::declval<mpf_class const&>() +
                               std::declval<mpf_class const&>());
    static_assert(std::same_as<decltype(asin(std::declval<expr_type>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(acos(std::declval<expr_type>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(sinh(std::declval<expr_type>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(cosh(std::declval<expr_type>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(tanh(std::declval<expr_type>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(asinh(std::declval<expr_type>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(acosh(std::declval<expr_type>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(atanh(std::declval<expr_type>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(exp2(std::declval<expr_type>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(exp10(std::declval<expr_type>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(gamma(std::declval<expr_type>())),
                               mpf_class>);
    static_assert(std::same_as<
                  decltype(reciprocal_gamma(std::declval<expr_type>())),
                  mpf_class>);
}

void test_constants() {
    const mp_bitcnt_t prec = 192;
    const mpf_class one(1, prec);
    const mpf_class two(2, prec);
    const mpf_class four(4, prec);

    assert(e(prec).get_prec() == gmpxx_detail::effective_mpf_prec(prec));
    assert_close(e(prec), exp(one), 120);
    assert_close(const_e(prec), e(prec), 160);
    assert(const_e().get_prec() == gmpxx_defaults::get_default_prec());

    assert_close(log_ten(prec), log(mpf_class(10, prec)), 120);
    assert_close(const_log10(prec), log_ten(prec), 160);
    assert(const_log10().get_prec() == gmpxx_defaults::get_default_prec());

    assert_close(inv_log_two(prec) * log_two(prec), one, 120);
    assert(inv_log_two().get_prec() == gmpxx_defaults::get_default_prec());

    assert_close(pi_over_two(prec) * two, pi(prec), 120);
    assert_close(pi_over_four(prec) * four, pi(prec), 120);
    assert_close(two_pi(prec), pi(prec) * two, 120);
    assert(pi_over_two().get_prec() == gmpxx_defaults::get_default_prec());
    assert(pi_over_four().get_prec() == gmpxx_defaults::get_default_prec());
    assert(two_pi().get_prec() == gmpxx_defaults::get_default_prec());
}

void test_inverse_trig() {
    const mp_bitcnt_t prec = 192;
    const mpf_class zero(0, prec);
    const mpf_class one(1, prec);
    const mpf_class half("0.5", prec);

    assert(asin(zero) == zero);
    assert(acos(one) == zero);
    assert_close(asin(half) + acos(half), pi_over_two(prec), 110);
    assert_close(sin(asin(half)), half, 110);
    assert_close(cos(acos(half)), half, 110);

    expect_domain_error([&] { (void)asin(mpf_class("1.25", prec)); });
    expect_domain_error([&] { (void)acos(mpf_class("-1.25", prec)); });
}

void test_hyperbolic() {
    const mp_bitcnt_t prec = 192;
    const mpf_class zero(0, prec);
    const mpf_class one(1, prec);
    const mpf_class x("0.75", prec);

    assert(sinh(zero) == zero);
    assert(cosh(zero) == one);
    assert(tanh(zero) == zero);
    assert_close((cosh(x) * cosh(x)) - (sinh(x) * sinh(x)), one, 105);
    assert_close(tanh(x), sinh(x) / cosh(x), 120);
}

void test_inverse_hyperbolic() {
    const mp_bitcnt_t prec = 192;
    const mpf_class zero(0, prec);
    const mpf_class x("0.25", prec);
    const mpf_class y("1.75", prec);

    assert(asinh(zero) == zero);
    assert(acosh(mpf_class(1, prec)) == zero);
    assert(atanh(zero) == zero);
    assert_close(sinh(asinh(x)), x, 105);
    assert_close(cosh(acosh(y)), y, 105);
    assert_close(tanh(atanh(x)), x, 105);

    expect_domain_error([&] { (void)acosh(mpf_class("0.75", prec)); });
    expect_domain_error([&] { (void)atanh(mpf_class(1, prec)); });
    expect_domain_error([&] { (void)atanh(mpf_class(-1, prec)); });
}

void test_exp_base_variants() {
    const mp_bitcnt_t prec = 192;
    const mpf_class zero(0, prec);
    const mpf_class one(1, prec);
    const mpf_class two(2, prec);
    const mpf_class three(3, prec);
    const mpf_class neg_two = -two;

    assert(exp2(zero) == one);
    assert_close(exp2(three), mpf_class(8, prec), 120);
    assert_close(exp2(neg_two), mpf_class("0.25", prec), 120);
    assert(exp10(zero) == one);
    assert_close(exp10(two), mpf_class(100, prec), 120);
    assert_close(log2(exp2(one)), one, 120);
}

void test_gamma_functions() {
    const mp_bitcnt_t prec = 192;
    const mpf_class zero(0, prec);
    const mpf_class one(1, prec);
    const mpf_class half("0.5", prec);
    const mpf_class five(5, prec);
    const mpf_class twenty_four(24, prec);
    const mpf_class x("1.25", prec);

    assert_close(gamma(one), one, 110);
    assert_close(gamma(five), twenty_four, 100);
    assert_close(gamma(half), sqrt(pi(prec)), 100);
    assert_close(gamma(x + one), x * gamma(x), 100);
    assert_close(reciprocal_gamma(x) * gamma(x), one, 110);
    assert(reciprocal_gamma(zero) == zero);
    expect_domain_error([&] { (void)gamma(zero); });
    expect_domain_error([&] { (void)gamma(mpf_class(-2, prec)); });
}

void test_expression_overloads() {
    const mp_bitcnt_t low_prec = 160;
    const mp_bitcnt_t high_prec = 224;
    const mpf_class a("0.25", low_prec);
    const mpf_class b("0.5", high_prec);
    const mpf_class c("1.25", high_prec);
    const mpf_class offset("0.125", high_prec);

    auto small_expr = a + offset;
    auto acosh_expr = c + b;

    const mpf_class small_value(small_expr);
    const mpf_class acosh_value(acosh_expr);

    assert(asin(small_expr).get_prec() == small_value.get_prec());
    assert_close(asin(small_expr), asin(small_value), 135);
    assert_close(acos(small_expr), acos(small_value), 135);
    assert_close(sinh(small_expr), sinh(small_value), 135);
    assert_close(cosh(small_expr), cosh(small_value), 135);
    assert_close(tanh(small_expr), tanh(small_value), 135);
    assert_close(asinh(small_expr), asinh(small_value), 135);
    assert_close(acosh(acosh_expr), acosh(acosh_value), 135);
    assert_close(atanh(small_expr), atanh(small_value), 135);
    assert_close(exp2(small_expr), exp2(small_value), 135);
    assert_close(exp10(small_expr), exp10(small_value), 135);
    assert_close(gamma(acosh_expr), gamma(acosh_value), 110);
    assert_close(reciprocal_gamma(acosh_expr), reciprocal_gamma(acosh_value),
                 110);
}

void test_precision_policy() {
    const mp_bitcnt_t prec = 224;
    const mpf_class x("0.25", prec);
    const mpf_class y("1.25", prec);

    assert(asin(x).get_prec() == x.get_prec());
    assert(acos(x).get_prec() == x.get_prec());
    assert(sinh(x).get_prec() == x.get_prec());
    assert(cosh(x).get_prec() == x.get_prec());
    assert(tanh(x).get_prec() == x.get_prec());
    assert(asinh(x).get_prec() == x.get_prec());
    assert(acosh(y).get_prec() == y.get_prec());
    assert(atanh(x).get_prec() == x.get_prec());
    assert(exp2(x).get_prec() == x.get_prec());
    assert(exp10(x).get_prec() == x.get_prec());
    assert(gamma(y).get_prec() == y.get_prec());
    assert(reciprocal_gamma(y).get_prec() == y.get_prec());
}

}  // namespace

int main() {
    test_compile_time_surface();
    test_constants();
    test_inverse_trig();
    test_hyperbolic();
    test_inverse_hyperbolic();
    test_exp_base_variants();
    test_gamma_functions();
    test_expression_overloads();
    test_precision_policy();
    return 0;
}
