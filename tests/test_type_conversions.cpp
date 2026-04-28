// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>

namespace {

void assert_mpf_equal(mpf_class const& got, mpf_class const& expected) {
    assert(mpf_cmp(got.get_mpf_t(), expected.get_mpf_t()) == 0);
}

void test_compile_time_surface() {
    static_assert(std::is_constructible_v<mpz_class, double>);
    static_assert(std::is_constructible_v<mpz_class, std::int64_t>);
    static_assert(std::is_constructible_v<mpz_class, std::uint64_t>);
    static_assert(std::is_constructible_v<mpq_class, double>);
    static_assert(std::is_constructible_v<mpq_class, mpz_class const&>);
    static_assert(std::is_constructible_v<mpq_class,
                                          mpz_class const&,
                                          mpz_class const&>);
    static_assert(std::is_constructible_v<mpf_class,
                                          mpz_class const&,
                                          mp_bitcnt_t>);
}

void test_mpz_integer_and_double_construction() {
    std::int64_t i64 = INT64_C(-9223372036854775807);
    std::uint64_t u64 = UINT64_C(18446744073709551614);

    mpz_class from_i64(i64);
    mpz_class from_u64(u64);
    assert(from_i64.get_str() == "-9223372036854775807");
    assert(from_u64.get_str() == "18446744073709551614");

    mpz_class assigned_i64;
    assigned_i64 = i64;
    mpz_class assigned_u64;
    assigned_u64 = u64;
    assert(assigned_i64 == from_i64);
    assert(assigned_u64 == from_u64);

    mpz_class from_double(31415926535.0);
    assert(from_double == mpz_class("31415926535"));

    mpz_class assigned_double;
    assigned_double = 31415926535.0;
    assert(assigned_double == mpz_class("31415926535"));
}

void test_string_and_base_construction() {
    mpz_class from_cstr = "14142135624";
    assert(from_cstr == mpz_class("14142135624"));

    std::string z_text = "31415926535";
    mpz_class from_string = z_text;
    assert(from_string == mpz_class("31415926535"));

    mpz_class from_hex("3243F6A8885A308D313198A2E0370734", 16);
    assert(from_hex.get_str() == "66814286504060421741230023322616923956");

    mpq_class decimal_fraction("-13/297");
    assert(decimal_fraction == mpq_class("-13/297"));

    mpq_class hex_fraction("1/a", 16);
    assert(hex_fraction.get_str(16) == "1/a");

    std::string q_text = "3/4";
    mpq_class q_from_string(q_text);
    assert(q_from_string == mpq_class("3/4"));

    mpq_class reducible("2/8");
    assert(reducible == mpq_class("1/4"));
}

void test_mpz_to_mpf_and_mpq_construction() {
    mpz_class z("-31415926535");

    mpf_class f(z, static_cast<mp_bitcnt_t>(512));
    assert(f.get_prec() == gmpxx_mkII_detail::effective_mpf_prec(512));
    assert_mpf_equal(f, mpf_class("-31415926535", f.get_prec()));

    mpq_class q_from_z(z);
    assert(q_from_z == mpq_class("-31415926535"));

    mpq_class q_from_pair(mpz_class(std::int64_t{355}),
                          mpz_class(std::int64_t{113}));
    assert(q_from_pair == mpq_class("355/113"));
}

void test_mpq_numerator_denominator_accessors() {
    mpq_class value("3/4");
    assert(value.get_num() == mpz_class(std::int64_t{3}));
    assert(value.get_den() == mpz_class(std::int64_t{4}));
}

void test_mpq_double_construction() {
    mpq_class half(0.5);
    assert(half == mpq_class("1/2"));

    mpq_class negative(-0.25);
    assert(negative == mpq_class("-1/4"));
}

}  // namespace

int main() {
    test_compile_time_surface();
    test_mpz_integer_and_double_construction();
    test_string_and_base_construction();
    test_mpz_to_mpf_and_mpq_construction();
    test_mpq_numerator_denominator_accessors();
    test_mpq_double_construction();
    return 0;
}
