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
#include <climits>
#include <concepts>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>

namespace {

void assert_mpf_equal(mpf_class const& got, mpf_class const& expected) {
    assert(mpf_cmp(got.get_mpf_t(), expected.get_mpf_t()) == 0);
}

#if defined(__SIZEOF_INT128__)
using int128_type = gmpxx_detail::int128_type;
using uint128_type = gmpxx_detail::uint128_type;

std::string uint128_to_string(uint128_type value) {
    if (value == 0) {
        return "0";
    }

    std::string result;
    while (value != 0) {
        unsigned digit = static_cast<unsigned>(value % 10);
        result.push_back(static_cast<char>('0' + digit));
        value /= 10;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::string int128_to_string(int128_type value) {
    if (value < 0) {
        uint128_type magnitude =
            static_cast<uint128_type>(-(value + 1)) + 1u;
        return "-" + uint128_to_string(magnitude);
    }
    return uint128_to_string(static_cast<uint128_type>(value));
}
#endif

void test_compile_time_surface() {
    static_assert(std::is_constructible_v<mpz_class, double>);
    static_assert(std::is_constructible_v<mpz_class, std::int64_t>);
    static_assert(std::is_constructible_v<mpz_class, std::uint64_t>);
    static_assert(std::is_constructible_v<mpq_class, double>);
    static_assert(std::is_constructible_v<mpq_class, mpz_class const&>);
    static_assert(std::is_constructible_v<mpq_class, mpf_class const&>);
    static_assert(std::is_constructible_v<mpq_class,
                                          mpz_class const&,
                                          mpz_class const&>);
    static_assert(std::is_constructible_v<mpz_class, mpz_srcptr>);
    static_assert(std::is_constructible_v<mpq_class, mpq_srcptr>);
    static_assert(std::is_constructible_v<mpz_class, mpf_class const&>);
    static_assert(std::is_constructible_v<mpz_class, mpq_class const&>);
    static_assert(std::is_constructible_v<mpf_class, mpz_class const&>);
    static_assert(std::is_constructible_v<mpf_class,
                                          mpz_class const&,
                                          mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, mpq_class const&>);
    static_assert(std::is_constructible_v<mpf_class,
                                          mpq_class const&,
                                          mp_bitcnt_t>);
    static_assert(std::same_as<decltype(std::declval<mpf_class const&>().get_d()),
                               double>);
    static_assert(std::same_as<decltype(std::declval<mpf_class const&>().get_ui()),
                               unsigned long>);
    static_assert(std::same_as<decltype(std::declval<mpf_class const&>().get_si()),
                               signed long>);
    static_assert(std::same_as<decltype(std::declval<mpz_class const&>().get_d()),
                               double>);
    static_assert(std::same_as<decltype(std::declval<mpz_class const&>().get_ui()),
                               unsigned long>);
    static_assert(std::same_as<decltype(std::declval<mpz_class const&>().get_si()),
                               signed long>);
    static_assert(std::same_as<decltype(std::declval<mpq_class const&>().get_d()),
                               double>);
    static_assert(std::same_as<decltype(std::declval<mpq_class&>().get_num_mpz_t()),
                               mpz_ptr>);
    static_assert(std::same_as<decltype(std::declval<mpq_class const&>().get_num_mpz_t()),
                               mpz_srcptr>);
    static_assert(std::same_as<decltype(std::declval<mpq_class&>().get_den_mpz_t()),
                               mpz_ptr>);
    static_assert(std::same_as<decltype(std::declval<mpq_class const&>().get_den_mpz_t()),
                               mpz_srcptr>);
#if defined(__SIZEOF_INT128__)
    static_assert(std::is_constructible_v<mpz_class, int128_type>);
    static_assert(std::is_constructible_v<mpz_class, uint128_type>);
    static_assert(std::is_assignable_v<mpz_class&, int128_type>);
    static_assert(std::is_assignable_v<mpz_class&, uint128_type>);
#endif
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

void test_mpz_int128_construction_and_assignment() {
#if defined(__SIZEOF_INT128__)
    int128_type signed_value =
        static_cast<int128_type>(0x0123456789ABCDEFULL) *
        static_cast<int128_type>(0x0FEDCBA987654321ULL);
    int128_type signed_negative = -signed_value;
    uint128_type unsigned_value =
        static_cast<uint128_type>(0xFEDCBA9876543210ULL) *
        static_cast<uint128_type>(0xFFFFFFFFFFFFFFFFULL);

    mpz_class from_signed(signed_value);
    assert(from_signed.get_str() == int128_to_string(signed_value));

    mpz_class from_negative(signed_negative);
    assert(from_negative.get_str() == int128_to_string(signed_negative));

    mpz_class from_unsigned(unsigned_value);
    assert(from_unsigned.get_str() == uint128_to_string(unsigned_value));

    mpz_class assigned;
    assigned = signed_value;
    assert(assigned == from_signed);
    assigned = signed_negative;
    assert(assigned == from_negative);
    assigned = unsigned_value;
    assert(assigned == from_unsigned);
#endif
}

void test_mpz_string_assignment() {
    mpz_class from_cstr;
    from_cstr = "14142135624";
    assert(from_cstr == mpz_class("14142135624"));

    std::string z_text = "31415926535";
    mpz_class from_string;
    from_string = z_text;
    assert(from_string == mpz_class("31415926535"));

    mpz_class unchanged("123");
    bool threw = false;
    try {
        unchanged = "not-an-integer";
    } catch (std::invalid_argument const&) {
        threw = true;
    }
    assert(threw);
    assert(unchanged == mpz_class("123"));
}

void test_raw_gmp_construction() {
    mpz_t raw_z;
    mpz_init_set_str(raw_z, "-123456789012345678901234567890", 10);
    mpz_class z(raw_z);
    assert(z == mpz_class("-123456789012345678901234567890"));
    mpz_clear(raw_z);

    mpq_t raw_q;
    mpq_init(raw_q);
    mpq_set_str(raw_q, "6/8", 10);
    mpq_class q(raw_q);
    assert(q == mpq_class("3/4"));
    mpq_clear(raw_q);
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

    mpf_class f_default(z);
    assert(f_default.get_prec() == gmpxx_defaults::get_default_prec());
    assert_mpf_equal(f_default, mpf_class("-31415926535", f_default.get_prec()));

    mpf_class f(z, static_cast<mp_bitcnt_t>(512));
    assert(f.get_prec() == gmpxx_detail::effective_mpf_prec(512));
    assert_mpf_equal(f, mpf_class("-31415926535", f.get_prec()));

    mpq_class q_from_z(z);
    assert(q_from_z == mpq_class("-31415926535"));

    mpq_class q_from_pair(mpz_class(std::int64_t{355}),
                          mpz_class(std::int64_t{113}));
    assert(q_from_pair == mpq_class("355/113"));
}

void test_expression_get_prec_alias() {
    mpf_class low("1", static_cast<mp_bitcnt_t>(64));
    mpf_class high("2", static_cast<mp_bitcnt_t>(1024));
    mpz_class z("3");

#if defined(GMPXX_MKII_NOPRECCHANGE)
    mp_bitcnt_t expected = gmpxx_defaults::get_default_prec();
#else
    mp_bitcnt_t expected = high.get_prec();
#endif

    assert((low + high).get_prec() == expected);
    assert((z + high).get_prec() == expected);
    assert((high + z).get_prec() == expected);
    assert((z - high).get_prec() == expected);
    assert((high - z).get_prec() == expected);
    assert((-(high + z)).get_prec() == expected);
}

void test_wrapper_to_wrapper_construction() {
    mpf_class f_pos("4.5", static_cast<mp_bitcnt_t>(256));
    mpf_class f_neg("-4.5", static_cast<mp_bitcnt_t>(256));

    mpz_class z_from_f_pos(f_pos);
    mpz_class z_from_f_neg(f_neg);
    assert(z_from_f_pos == mpz_class(4));
    assert(z_from_f_neg == mpz_class(-4));

    mpq_class q_from_f(f_pos);
    assert(q_from_f == mpq_class("9/2"));

    mpq_class q("3000/13");
    mpz_class z_from_q(q);
    assert(z_from_q == mpz_class(230));

    mpf_class f_from_q(q, static_cast<mp_bitcnt_t>(384));
    assert(f_from_q.get_prec() == gmpxx_detail::effective_mpf_prec(384));
    mpf_class expected_from_q(0, f_from_q.get_prec());
    mpf_set_q(expected_from_q.get_mpf_t(), q.get_mpq_t());
    assert_mpf_equal(f_from_q, expected_from_q);

    mpf_class f_from_q_default(q);
    assert(f_from_q_default.get_prec() == gmpxx_defaults::get_default_prec());
    mpf_class expected_default(0, f_from_q_default.get_prec());
    mpf_set_q(expected_default.get_mpf_t(), q.get_mpq_t());
    assert_mpf_equal(f_from_q_default, expected_default);
}

void test_wrapper_to_wrapper_assignment() {
    mpf_class f("4.5", static_cast<mp_bitcnt_t>(256));
    mpq_class q("3000/13");
    mpz_class z("-31415926535");

    mpz_class z_assigned;
    z_assigned = f;
    assert(z_assigned == mpz_class(4));
    z_assigned = q;
    assert(z_assigned == mpz_class(230));

    mpq_class q_assigned;
    q_assigned = f;
    assert(q_assigned == mpq_class("9/2"));
    q_assigned = z;
    assert(q_assigned == mpq_class("-31415926535"));

    mpf_class f_assigned("0", static_cast<mp_bitcnt_t>(192));
    mp_bitcnt_t old_prec = f_assigned.get_prec();
    f_assigned = z;
    assert(f_assigned.get_prec() == old_prec);
    mpf_class expected_z(0, old_prec);
    mpf_set_z(expected_z.get_mpf_t(), z.get_mpz_t());
    assert_mpf_equal(f_assigned, expected_z);

    f_assigned = q;
    assert(f_assigned.get_prec() == old_prec);
    mpf_class expected_q(0, old_prec);
    mpf_set_q(expected_q.get_mpf_t(), q.get_mpq_t());
    assert_mpf_equal(f_assigned, expected_q);
}

void test_mpq_numerator_denominator_accessors() {
    mpq_class value("3/4");
    assert(value.get_num() == mpz_class(std::int64_t{3}));
    assert(value.get_den() == mpz_class(std::int64_t{4}));
    assert(mpz_cmp_si(value.get_num_mpz_t(), 3) == 0);
    assert(mpz_cmp_si(value.get_den_mpz_t(), 4) == 0);

    mpz_set_si(value.get_num_mpz_t(), 6);
    mpz_set_si(value.get_den_mpz_t(), 8);
    value.canonicalize();

    assert(value == mpq_class("3/4"));
}

void test_mpq_double_construction() {
    mpq_class half(0.5);
    assert(half == mpq_class("1/2"));

    mpq_class negative(-0.25);
    assert(negative == mpq_class("-1/4"));
}

void test_mpf_scalar_conversions_and_fit_queries() {
    mpf_class pos("123");
    assert(pos.get_d() == 123.0);
    assert(pos.get_ui() == 123UL);
    assert(pos.get_si() == 123L);
    assert(pos.fits_sint_p());
    assert(pos.fits_slong_p());
    assert(pos.fits_sshort_p());
    assert(pos.fits_uint_p());
    assert(pos.fits_ulong_p());
    assert(pos.fits_ushort_p());

    mpf_class fractional("123.456");
    assert(fractional.get_ui() == 123UL);
    assert(fractional.get_si() == 123L);

    mpf_class negative("-123.456");
    assert(negative.get_si() == -123L);

    mpf_class large("999999999999999999999999999999");
    assert(!large.fits_sint_p());
    assert(!large.fits_uint_p());
}

void test_mpz_scalar_conversions_and_fit_queries() {
    mpz_class small("123");
    assert(small.get_d() == 123.0);
    assert(small.get_si() == 123L);
    assert(small.get_ui() == 123UL);
    assert(small.fits_sint_p());
    assert(small.fits_slong_p());
    assert(small.fits_sshort_p());
    assert(small.fits_uint_p());
    assert(small.fits_ulong_p());
    assert(small.fits_ushort_p());

    mpz_class negative("-123");
    assert(negative.get_si() == -123L);
    assert(negative.fits_sint_p());
    assert(negative.fits_slong_p());
    assert(negative.fits_sshort_p());
    assert(!negative.fits_uint_p());

    mpz_class large("1234567890123456789012345678901234567890");
    assert(!large.fits_sint_p());
    assert(!large.fits_slong_p());
    assert(!large.fits_sshort_p());
    assert(!large.fits_uint_p());
    assert(!large.fits_ulong_p());
    assert(!large.fits_ushort_p());

    mpz_class ushort_max(std::to_string(USHRT_MAX));
    assert(ushort_max.fits_ushort_p());
    ushort_max += 1;
    assert(!ushort_max.fits_ushort_p());
}

void test_mpq_scalar_conversion() {
    mpq_class value("1/4");
    assert(value.get_d() == 0.25);

    mpq_class negative("-3/4");
    assert(negative.get_d() == -0.75);
}

}  // namespace

int main() {
    test_compile_time_surface();
    test_mpz_integer_and_double_construction();
    test_mpz_int128_construction_and_assignment();
    test_mpz_string_assignment();
    test_raw_gmp_construction();
    test_string_and_base_construction();
    test_mpz_to_mpf_and_mpq_construction();
    test_expression_get_prec_alias();
    test_wrapper_to_wrapper_construction();
    test_wrapper_to_wrapper_assignment();
    test_mpq_numerator_denominator_accessors();
    test_mpq_double_construction();
    test_mpf_scalar_conversions_and_fit_queries();
    test_mpz_scalar_conversions_and_fit_queries();
    test_mpq_scalar_conversion();
    return 0;
}
