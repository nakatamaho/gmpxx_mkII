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
    static_assert(std::is_constructible_v<mpf_class, int, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, long, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, long long, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, unsigned, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<mpf_class, std::uint64_t, mp_bitcnt_t>);
    static_assert(!std::is_constructible_v<mpf_class, bool, mp_bitcnt_t>);
}

void test_default_constructor_value_zero() {
    mpf_class value;
    assert(mpf_sgn(value.get_mpf_t()) == 0);
    assert(value.get_prec() == gmpxx_defaults::get_default_prec());
}

void test_integral_constructor_with_explicit_precision() {
    mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(384);

    mpf_class zero(0, requested_prec);
    assert(mpf_sgn(zero.get_mpf_t()) == 0);
    assert(zero.get_prec() ==
           gmpxx_mkII_detail::effective_mpf_prec(requested_prec));

    mpf_class negative(-42, requested_prec);
    mpf_class negative_expected(requested_prec);
    mpf_set_si(negative_expected.get_mpf_t(), -42);
    assert(negative.get_prec() == negative_expected.get_prec());
    assert_mpf_equal(negative, negative_expected);

    std::uint64_t large = std::numeric_limits<std::uint64_t>::max();
    mpf_class unsigned_value(large, requested_prec);
    mpz_class large_z(large);
    mpf_class unsigned_expected(requested_prec);
    mpf_set_z(unsigned_expected.get_mpf_t(), large_z.get_mpz_t());
    assert(unsigned_value.get_prec() == unsigned_expected.get_prec());
    assert_mpf_equal(unsigned_value, unsigned_expected);
}

void test_copy_constructor_preserves_value_and_precision() {
    mpf_class original("3.1415926535", static_cast<mp_bitcnt_t>(384));
    mpf_class copy(original);

    assert(copy.get_prec() == original.get_prec());
    assert_mpf_equal(copy, original);
}

void test_copy_assignment_preserves_value_and_source_precision() {
    mpf_class source("-2.5", static_cast<mp_bitcnt_t>(512));
    mpf_class destination("1.25", static_cast<mp_bitcnt_t>(64));

    destination = source;

    assert(destination.get_prec() == source.get_prec());
    assert_mpf_equal(destination, source);
}

void test_double_assignment_preserves_destination_precision() {
    mpf_class destination("0", static_cast<mp_bitcnt_t>(320));
    mp_bitcnt_t old_prec = destination.get_prec();

    destination = 3.1415926535;

    mpf_class expected(3.1415926535, old_prec);
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

    mpf_class expected(requested_prec);
    int rc = mpf_set_str(expected.get_mpf_t(), hex_digits, 16);
    assert(rc == 0);

    assert(value.get_prec() == expected.get_prec());
    assert_mpf_equal(value, expected);
}

}  // namespace

int main() {
    test_compile_time_surface();
    test_default_constructor_value_zero();
    test_integral_constructor_with_explicit_precision();
    test_copy_constructor_preserves_value_and_precision();
    test_copy_assignment_preserves_value_and_source_precision();
    test_double_assignment_preserves_destination_precision();
    test_string_assignment_preserves_destination_precision();
    test_hex_string_constructor_with_explicit_base();
    return 0;
}
