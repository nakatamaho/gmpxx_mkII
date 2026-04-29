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

#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>

namespace {

void assert_mpf_equal(mpf_class const& got, mpf_class const& expected) {
    assert(mpf_cmp(got.get_mpf_t(), expected.get_mpf_t()) == 0);
}

void test_compile_time_surface() {
    static_assert(std::is_default_constructible_v<mpf_class>);
    static_assert(std::is_copy_constructible_v<mpf_class>);
    static_assert(std::is_move_constructible_v<mpf_class>);
    static_assert(std::is_copy_assignable_v<mpf_class>);
    static_assert(std::is_move_assignable_v<mpf_class>);
    static_assert(std::is_constructible_v<mpf_class, int, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, long, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, long long, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, unsigned, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, std::uint64_t, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, mpf_srcptr>);
    static_assert(std::is_constructible_v<mpf_class, mpf_srcptr, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, bool>);
    static_assert(std::is_constructible_v<mpf_class, bool, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, mpf_class const&,
                                          mp_bitcnt_t>);

    static_assert(std::is_default_constructible_v<mpz_class>);
    static_assert(std::is_copy_constructible_v<mpz_class>);
    static_assert(std::is_move_constructible_v<mpz_class>);
    static_assert(std::is_copy_assignable_v<mpz_class>);
    static_assert(std::is_move_assignable_v<mpz_class>);
    static_assert(std::is_constructible_v<mpz_class, int>);
    static_assert(std::is_constructible_v<mpz_class, bool>);
    static_assert(std::is_constructible_v<mpz_class, std::int32_t>);
    static_assert(std::is_constructible_v<mpz_class, std::uint32_t>);

    static_assert(std::is_default_constructible_v<mpq_class>);
    static_assert(std::is_copy_constructible_v<mpq_class>);
    static_assert(std::is_move_constructible_v<mpq_class>);
    static_assert(std::is_copy_assignable_v<mpq_class>);
    static_assert(std::is_move_assignable_v<mpq_class>);
    static_assert(std::is_constructible_v<mpq_class, bool>);
}

void test_default_constructor_value_zero() {
    mpf_class value;
    assert(mpf_sgn(value.get_mpf_t()) == 0);
    assert(value.get_prec() == gmpxx_defaults::get_default_prec());
}

void test_bare_integral_constructor_is_value_not_precision() {
    mpf_class value(384);
    mpf_class expected(384, gmpxx_defaults::get_default_prec());

    assert(value.get_prec() == gmpxx_defaults::get_default_prec());
    assert_mpf_equal(value, expected);
}

void test_zero_with_explicit_precision_replaces_precision_only_ctor() {
    mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(384);

    mpf_class value(0.0, requested_prec);

    assert(mpf_sgn(value.get_mpf_t()) == 0);
    assert(value.get_prec() ==
           gmpxx_detail::effective_mpf_prec(requested_prec));
}

void test_integral_constructor_with_explicit_precision() {
    mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(384);

    mpf_class zero(0, requested_prec);
    assert(mpf_sgn(zero.get_mpf_t()) == 0);
    assert(zero.get_prec() ==
           gmpxx_detail::effective_mpf_prec(requested_prec));

    mpf_class negative(-42, requested_prec);
    mpf_class negative_expected(0.0, requested_prec);
    mpf_set_si(negative_expected.get_mpf_t(), -42);
    assert(negative.get_prec() == negative_expected.get_prec());
    assert_mpf_equal(negative, negative_expected);

    std::uint64_t large = std::numeric_limits<std::uint64_t>::max();
    mpf_class unsigned_value(large, requested_prec);
    mpz_class large_z(large);
    mpf_class unsigned_expected(0.0, requested_prec);
    mpf_set_z(unsigned_expected.get_mpf_t(), large_z.get_mpz_t());
    assert(unsigned_value.get_prec() == unsigned_expected.get_prec());
    assert_mpf_equal(unsigned_value, unsigned_expected);
}

void test_expression_constructor_with_explicit_precision() {
    mpf_class a("1.25", static_cast<mp_bitcnt_t>(128));
    mpf_class b("2.5", static_cast<mp_bitcnt_t>(512));
    mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(320);

    mpf_class value(a + b, requested_prec);
    mpf_class expected(0.0, requested_prec);
    mpf_add(expected.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());

    assert(value.get_prec() ==
           gmpxx_detail::effective_mpf_prec(requested_prec));
    assert_mpf_equal(value, expected);
}

void test_raw_mpf_t_constructor() {
    mpf_t raw;
    mpf_init2(raw, static_cast<mp_bitcnt_t>(192));
    int rc = mpf_set_str(raw, "0.0390625", 10);
    assert(rc == 0);

    mpf_class value(raw);
    assert(value.get_prec() == mpf_get_prec(raw));
    assert(mpf_cmp(value.get_mpf_t(), raw) == 0);

    mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(384);
    mpf_class with_prec(raw, requested_prec);
    assert(with_prec.get_prec() ==
           gmpxx_detail::effective_mpf_prec(requested_prec));
    assert(mpf_cmp(with_prec.get_mpf_t(), raw) == 0);

    mpf_clear(raw);
}

void test_copy_constructor_preserves_value_and_precision() {
    mpf_class original("3.1415926535", static_cast<mp_bitcnt_t>(384));
    mpf_class copy(original);

    assert(copy.get_prec() == original.get_prec());
    assert_mpf_equal(copy, original);
}

void test_mpf_copy_constructor_with_explicit_precision() {
    mpf_class original("3.1415926535", static_cast<mp_bitcnt_t>(384));
    mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(128);

    mpf_class copy(original, requested_prec);
    mpf_class expected(0.0, requested_prec);
    mpf_set(expected.get_mpf_t(), original.get_mpf_t());

    assert(copy.get_prec() ==
           gmpxx_detail::effective_mpf_prec(requested_prec));
    assert_mpf_equal(copy, expected);
}

void test_bool_constructors() {
    mpf_class f_true(true);
    mpf_class f_false(false, static_cast<mp_bitcnt_t>(128));
    assert_mpf_equal(f_true, mpf_class(1, f_true.get_prec()));
    assert(f_false.get_prec() ==
           gmpxx_detail::effective_mpf_prec(128));
    assert_mpf_equal(f_false, mpf_class(0, f_false.get_prec()));

    mpz_class z_true(true);
    mpz_class z_false(false);
    assert(z_true == mpz_class(1));
    assert(z_false == mpz_class(0));

    mpq_class q_true(true);
    mpq_class q_false(false);
    assert(q_true == mpq_class(1));
    assert(q_false == mpq_class(0));
}

void test_copy_assignment_preserves_value_and_source_precision() {
    mpf_class source("-2.5", static_cast<mp_bitcnt_t>(512));
    mpf_class destination("1.25", static_cast<mp_bitcnt_t>(64));

    destination = source;

    assert(destination.get_prec() == source.get_prec());
    assert_mpf_equal(destination, source);
}

void test_move_constructor_preserves_value() {
    mpf_class original("123.0", static_cast<mp_bitcnt_t>(384));
    mpf_class expected(original);

    mpf_class moved(std::move(original));

    assert(moved.get_prec() == expected.get_prec());
    assert_mpf_equal(moved, expected);
}

void test_move_assignment_preserves_value() {
    mpf_class source("-8.5", static_cast<mp_bitcnt_t>(448));
    mpf_class expected(source);
    mpf_class destination("0", source.get_prec());

    destination = std::move(source);

    assert(destination.get_prec() == expected.get_prec());
    assert_mpf_equal(destination, expected);
}

void test_move_assignment_preserves_destination_precision_on_mismatch() {
    mpf_class source("2.5", static_cast<mp_bitcnt_t>(512));
    mpf_class destination("1.25", static_cast<mp_bitcnt_t>(64));
    mp_bitcnt_t old_prec = destination.get_prec();

    destination = std::move(source);

    mpf_class expected("2.5", old_prec);
    assert(destination.get_prec() == old_prec);
    assert_mpf_equal(destination, expected);
}

void test_move_assignment_uses_same_precision_fast_path_semantics() {
    mpf_class source("3.75", static_cast<mp_bitcnt_t>(256));
    mpf_class destination("-1.5", source.get_prec());
    mp_bitcnt_t old_prec = destination.get_prec();
    mpf_class expected(source);

    destination = std::move(source);

    assert(destination.get_prec() == old_prec);
    assert_mpf_equal(destination, expected);
}

void test_double_assignment_preserves_destination_precision() {
    mpf_class destination("0", static_cast<mp_bitcnt_t>(320));
    mp_bitcnt_t old_prec = destination.get_prec();

    destination = 3.1415926535;

    mpf_class expected(3.1415926535, old_prec);
    assert(destination.get_prec() == old_prec);
    assert_mpf_equal(destination, expected);
}

void test_integral_assignment_preserves_destination_precision() {
    mpf_class destination("0", static_cast<mp_bitcnt_t>(320));
    mp_bitcnt_t old_prec = destination.get_prec();

    destination = 0;
    assert(destination.get_prec() == old_prec);
    assert(mpf_sgn(destination.get_mpf_t()) == 0);

    destination = -42;
    mpf_class expected(-42, old_prec);
    assert(destination.get_prec() == old_prec);
    assert_mpf_equal(destination, expected);
}

void test_string_assignment_preserves_destination_precision() {
    {
        mpf_class destination("0", static_cast<mp_bitcnt_t>(320));
        mp_bitcnt_t old_prec = destination.get_prec();

        destination = "1.4142135624";

        mpf_class expected("1.4142135624", old_prec);
        assert(destination.get_prec() == old_prec);
        assert_mpf_equal(destination, expected);
    }

    {
        mpf_class destination("0", static_cast<mp_bitcnt_t>(448));
        mp_bitcnt_t old_prec = destination.get_prec();
        std::string text = "3.1415926535";

        destination = text;

        mpf_class expected(text, old_prec);
        assert(destination.get_prec() == old_prec);
        assert_mpf_equal(destination, expected);
    }
}

void test_hex_string_constructor_with_explicit_base() {
    char const* hex_digits =
        "3.243F6A8885A308D313198A2E03707344A4093822299F31D008";
    mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(512);

    mpf_class value(hex_digits, requested_prec, 16);

    mpf_class expected(0.0, requested_prec);
    int rc = mpf_set_str(expected.get_mpf_t(), hex_digits, 16);
    assert(rc == 0);

    assert(value.get_prec() == expected.get_prec());
    assert_mpf_equal(value, expected);
}

void test_mpf_swap_member_and_free_function() {
    mpf_class a("123.456", static_cast<mp_bitcnt_t>(192));
    mpf_class b("789.012", static_cast<mp_bitcnt_t>(320));
    mpf_class a_original(a);
    mpf_class b_original(b);

    a.swap(b);
    assert(a.get_prec() == b_original.get_prec());
    assert(b.get_prec() == a_original.get_prec());
    assert_mpf_equal(a, b_original);
    assert_mpf_equal(b, a_original);

    swap(a, b);
    assert(a.get_prec() == a_original.get_prec());
    assert(b.get_prec() == b_original.get_prec());
    assert_mpf_equal(a, a_original);
    assert_mpf_equal(b, b_original);
}

void test_mpz_construction_copy_move_assignment_and_swap() {
    mpz_class zero;
    assert(zero == mpz_class(std::int64_t{0}));

    mpz_class from_int(-123);
    assert(from_int == mpz_class(std::int64_t{-123}));

    std::int32_t i32 = INT32_C(-0x12345678);
    mpz_class from_i32(i32);
    assert(from_i32.get_str() == std::to_string(i32));

    std::uint32_t u32 = UINT32_C(0xfedcba98);
    mpz_class from_u32(u32);
    assert(from_u32.get_str() == std::to_string(u32));

    mpz_class assigned_from_i32;
    assigned_from_i32 = i32;
    assert(assigned_from_i32 == from_i32);

    mpz_class assigned_from_u32;
    assigned_from_u32 = u32;
    assert(assigned_from_u32 == from_u32);

    mpz_class original("12345678901234567890");
    mpz_class copied(original);
    assert(copied == original);

    mpz_class moved(std::move(original));
    assert(moved == copied);

    mpz_class assigned;
    assigned = copied;
    assert(assigned == copied);

    mpz_class move_assigned;
    move_assigned = std::move(moved);
    assert(move_assigned == copied);

    mpz_class a("123456");
    mpz_class b("789012");
    a.swap(b);
    assert(a == mpz_class("789012"));
    assert(b == mpz_class("123456"));

    swap(a, b);
    assert(a == mpz_class("123456"));
    assert(b == mpz_class("789012"));
}

void test_mpq_construction_copy_move_assignment_and_swap() {
    mpq_class zero;
    assert(zero == mpq_class(std::int64_t{0}));

    mpq_class from_int_pair(0, 1);
    assert(from_int_pair == zero);

    mpq_class original("355/113");
    mpq_class copied(original);
    assert(copied == original);

    mpq_class moved(std::move(original));
    assert(moved == copied);

    mpq_class assigned;
    assigned = copied;
    assert(assigned == copied);

    mpq_class move_assigned;
    move_assigned = std::move(moved);
    assert(move_assigned == copied);

    mpq_class a("1/2");
    mpq_class b("-3/4");
    a.swap(b);
    assert(a == mpq_class("-3/4"));
    assert(b == mpq_class("1/2"));

    swap(a, b);
    assert(a == mpq_class("1/2"));
    assert(b == mpq_class("-3/4"));
}

}  // namespace

int main() {
    test_compile_time_surface();
    test_default_constructor_value_zero();
    test_bare_integral_constructor_is_value_not_precision();
    test_zero_with_explicit_precision_replaces_precision_only_ctor();
    test_integral_constructor_with_explicit_precision();
    test_expression_constructor_with_explicit_precision();
    test_raw_mpf_t_constructor();
    test_copy_constructor_preserves_value_and_precision();
    test_mpf_copy_constructor_with_explicit_precision();
    test_bool_constructors();
    test_copy_assignment_preserves_value_and_source_precision();
    test_move_constructor_preserves_value();
    test_move_assignment_preserves_value();
    test_move_assignment_preserves_destination_precision_on_mismatch();
    test_move_assignment_uses_same_precision_fast_path_semantics();
    test_double_assignment_preserves_destination_precision();
    test_integral_assignment_preserves_destination_precision();
    test_string_assignment_preserves_destination_precision();
    test_hex_string_constructor_with_explicit_base();
    test_mpf_swap_member_and_free_function();
    test_mpz_construction_copy_move_assignment_and_swap();
    test_mpq_construction_copy_move_assignment_and_swap();
    return 0;
}
