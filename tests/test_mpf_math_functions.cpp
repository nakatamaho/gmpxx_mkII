// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cassert>
#include <concepts>
#include <utility>

namespace {

void assert_mpf_equal(mpf_class const& got, mpf_class const& expected) {
    assert(mpf_cmp(got.get_mpf_t(), expected.get_mpf_t()) == 0);
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
                  decltype(gmpxx_mkII::sqrt(std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(gmpxx_mkII::abs(std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(gmpxx_mkII::neg(std::declval<mpf_class const&>())),
                  mpf_class>);
}

void test_sqrt() {
    mpf_class four("4.0", static_cast<mp_bitcnt_t>(256));
    mpf_class result = sqrt(four);

    mpf_class expected(four.get_prec());
    mpf_sqrt(expected.get_mpf_t(), four.get_mpf_t());

    assert(result.get_prec() == four.get_prec());
    assert_mpf_equal(result, expected);

    mpf_class zero("0.0", static_cast<mp_bitcnt_t>(192));
    result = gmpxx_mkII::sqrt(zero);

    mpf_class zero_expected(zero.get_prec());
    mpf_sqrt(zero_expected.get_mpf_t(), zero.get_mpf_t());

    assert(result.get_prec() == zero.get_prec());
    assert_mpf_equal(result, zero_expected);
}

void test_abs() {
    mpf_class negative("-3.5", static_cast<mp_bitcnt_t>(320));
    mpf_class result = abs(negative);

    mpf_class expected(negative.get_prec());
    mpf_abs(expected.get_mpf_t(), negative.get_mpf_t());

    assert(result.get_prec() == negative.get_prec());
    assert_mpf_equal(result, expected);

    mpf_class positive("2.25", static_cast<mp_bitcnt_t>(128));
    result = gmpxx_mkII::abs(positive);

    mpf_class positive_expected(positive.get_prec());
    mpf_abs(positive_expected.get_mpf_t(), positive.get_mpf_t());

    assert(result.get_prec() == positive.get_prec());
    assert_mpf_equal(result, positive_expected);
}

void test_neg() {
    mpf_class negative("-3.5", static_cast<mp_bitcnt_t>(384));
    mpf_class result = neg(negative);

    mpf_class expected(negative.get_prec());
    mpf_neg(expected.get_mpf_t(), negative.get_mpf_t());

    assert(result.get_prec() == negative.get_prec());
    assert_mpf_equal(result, expected);

    mpf_class positive("2.25", static_cast<mp_bitcnt_t>(160));
    result = gmpxx_mkII::neg(positive);

    mpf_class positive_expected(positive.get_prec());
    mpf_neg(positive_expected.get_mpf_t(), positive.get_mpf_t());

    assert(result.get_prec() == positive.get_prec());
    assert_mpf_equal(result, positive_expected);
}

}  // namespace

int main() {
    test_compile_time_surface();
    test_sqrt();
    test_abs();
    test_neg();
    return 0;
}
