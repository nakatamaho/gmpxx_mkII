// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cassert>
#include <cstdint>
#include <limits>
#include <utility>

#if !defined(GMPXX_MKII_TEST_FUSION_COUNTERS)
#error "test_mpz_addmul_fusion requires GMPXX_MKII_TEST_FUSION_COUNTERS"
#endif

namespace {

using gmpxx_mkII_detail::is_mpz_addmul_fusable_v;

static_assert(is_mpz_addmul_fusable_v<
              decltype(std::declval<mpz_class const&>() *
                       std::declval<mpz_class const&>())>);
static_assert(is_mpz_addmul_fusable_v<
              decltype(std::declval<mpz_class const&>() * 5LL)>);
static_assert(is_mpz_addmul_fusable_v<
              decltype(5LL * std::declval<mpz_class const&>())>);
static_assert(is_mpz_addmul_fusable_v<
              decltype(std::declval<mpz_class const&>() * 0LL)>);
static_assert(is_mpz_addmul_fusable_v<
              decltype(0LL * std::declval<mpz_class const&>())>);

static_assert(!is_mpz_addmul_fusable_v<
              decltype(std::declval<mpz_class const&>() *
                       std::declval<mpz_class const&>() *
                       std::declval<mpz_class const&>())>);
static_assert(!is_mpz_addmul_fusable_v<
              decltype((std::declval<mpz_class const&>() +
                        std::declval<mpz_class const&>()) *
                       std::declval<mpz_class const&>())>);
static_assert(!is_mpz_addmul_fusable_v<
              decltype(-std::declval<mpz_class const&>() *
                       std::declval<mpz_class const&>())>);
static_assert(!is_mpz_addmul_fusable_v<
              decltype(std::declval<mpq_class const&>() *
                       std::declval<mpz_class const&>())>);
static_assert(!is_mpz_addmul_fusable_v<
              decltype(std::declval<mpz_class const&>() *
                       std::declval<mpq_class const&>())>);
static_assert(!is_mpz_addmul_fusable_v<
              decltype(std::declval<mpf_class const&>() *
                       std::declval<mpz_class const&>())>);
static_assert(!is_mpz_addmul_fusable_v<
              decltype(std::declval<mpz_class const&>() *
                       std::declval<mpf_class const&>())>);
static_assert(!is_mpz_addmul_fusable_v<
              decltype(std::declval<mpz_class const&>() * 1.0)>);

std::uint64_t addmul_count() {
    return gmpxx_mkII_detail::test_mpz_addmul_fused_count.load(
        std::memory_order_relaxed);
}

std::uint64_t submul_count() {
    return gmpxx_mkII_detail::test_mpz_submul_fused_count.load(
        std::memory_order_relaxed);
}

std::uint64_t addmul_ui_count() {
    return gmpxx_mkII_detail::test_mpz_addmul_ui_fused_count.load(
        std::memory_order_relaxed);
}

std::uint64_t submul_ui_count() {
    return gmpxx_mkII_detail::test_mpz_submul_ui_fused_count.load(
        std::memory_order_relaxed);
}

void reset_counters() {
    gmpxx_mkII_detail::reset_mpz_addmul_fusion_counters();
}

void assert_equal(mpz_class const& got, mpz_t expected) {
    assert(mpz_cmp(got.get_mpz_t(), expected) == 0);
}

void check_addmul(mpz_class const& a0, mpz_class const& b, mpz_class const& c) {
    mpz_t ref;
    mpz_init_set(ref, a0.get_mpz_t());
    mpz_addmul(ref, b.get_mpz_t(), c.get_mpz_t());

    mpz_class a = a0;
    reset_counters();
    a += b * c;
    assert_equal(a, ref);
    assert(addmul_count() == 1);
    assert(submul_count() == 0);
    assert(addmul_ui_count() == 0);
    assert(submul_ui_count() == 0);

    mpz_clear(ref);
}

void check_submul(mpz_class const& a0, mpz_class const& b, mpz_class const& c) {
    mpz_t ref;
    mpz_init_set(ref, a0.get_mpz_t());
    mpz_submul(ref, b.get_mpz_t(), c.get_mpz_t());

    mpz_class a = a0;
    reset_counters();
    a -= b * c;
    assert_equal(a, ref);
    assert(addmul_count() == 0);
    assert(submul_count() == 1);
    assert(addmul_ui_count() == 0);
    assert(submul_ui_count() == 0);

    mpz_clear(ref);
}

void check_addmul_scalar(mpz_class const& a0, mpz_class const& b,
                         std::int64_t s, bool scalar_left) {
    mpz_t ref;
    mpz_init_set(ref, a0.get_mpz_t());
    mpz_class scalar(s);
    mpz_addmul(ref, b.get_mpz_t(), scalar.get_mpz_t());

    mpz_class a = a0;
    reset_counters();
    if (scalar_left) {
        a += s * b;
    } else {
        a += b * s;
    }
    assert_equal(a, ref);
    const std::uint64_t mag = s >= 0
        ? static_cast<std::uint64_t>(s)
        : gmpxx_mkII_detail::safe_negate(s);
    const bool ui_path = gmpxx_mkII_detail::fits_in_ulong(mag);
    if (s >= 0 && ui_path) {
        assert(addmul_ui_count() == 1);
        assert(submul_ui_count() == 0);
        assert(addmul_count() == 0);
        assert(submul_count() == 0);
    } else if (s < 0 && ui_path) {
        assert(addmul_ui_count() == 0);
        assert(submul_ui_count() == 1);
        assert(addmul_count() == 0);
        assert(submul_count() == 0);
    } else if (s >= 0) {
        assert(addmul_ui_count() == 0);
        assert(submul_ui_count() == 0);
        assert(addmul_count() == 1);
        assert(submul_count() == 0);
    } else {
        assert(addmul_ui_count() == 0);
        assert(submul_ui_count() == 0);
        assert(addmul_count() == 0);
        assert(submul_count() == 1);
    }

    mpz_clear(ref);
}

void check_submul_scalar(mpz_class const& a0, mpz_class const& b,
                         std::int64_t s, bool scalar_left) {
    mpz_t ref;
    mpz_init_set(ref, a0.get_mpz_t());
    mpz_class scalar(s);
    mpz_submul(ref, b.get_mpz_t(), scalar.get_mpz_t());

    mpz_class a = a0;
    reset_counters();
    if (scalar_left) {
        a -= s * b;
    } else {
        a -= b * s;
    }
    assert_equal(a, ref);
    const std::uint64_t mag = s >= 0
        ? static_cast<std::uint64_t>(s)
        : gmpxx_mkII_detail::safe_negate(s);
    const bool ui_path = gmpxx_mkII_detail::fits_in_ulong(mag);
    if (s >= 0 && ui_path) {
        assert(addmul_ui_count() == 0);
        assert(submul_ui_count() == 1);
        assert(addmul_count() == 0);
        assert(submul_count() == 0);
    } else if (s < 0 && ui_path) {
        assert(addmul_ui_count() == 1);
        assert(submul_ui_count() == 0);
        assert(addmul_count() == 0);
        assert(submul_count() == 0);
    } else if (s >= 0) {
        assert(addmul_ui_count() == 0);
        assert(submul_ui_count() == 0);
        assert(addmul_count() == 0);
        assert(submul_count() == 1);
    } else {
        assert(addmul_ui_count() == 0);
        assert(submul_ui_count() == 0);
        assert(addmul_count() == 1);
        assert(submul_count() == 0);
    }

    mpz_clear(ref);
}

void check_addmul_u64(mpz_class const& a0, mpz_class const& b,
                      std::uint64_t s, bool scalar_left) {
    mpz_t ref;
    mpz_init_set(ref, a0.get_mpz_t());
    mpz_class scalar(s);
    mpz_addmul(ref, b.get_mpz_t(), scalar.get_mpz_t());

    mpz_class a = a0;
    reset_counters();
    if (scalar_left) {
        a += s * b;
    } else {
        a += b * s;
    }
    assert_equal(a, ref);
    if (gmpxx_mkII_detail::fits_in_ulong(s)) {
        assert(addmul_ui_count() == 1);
        assert(addmul_count() == 0);
    } else {
        assert(addmul_ui_count() == 0);
        assert(addmul_count() == 1);
    }

    mpz_clear(ref);
}

void check_submul_u64(mpz_class const& a0, mpz_class const& b,
                      std::uint64_t s, bool scalar_left) {
    mpz_t ref;
    mpz_init_set(ref, a0.get_mpz_t());
    mpz_class scalar(s);
    mpz_submul(ref, b.get_mpz_t(), scalar.get_mpz_t());

    mpz_class a = a0;
    reset_counters();
    if (scalar_left) {
        a -= s * b;
    } else {
        a -= b * s;
    }
    assert_equal(a, ref);
    if (gmpxx_mkII_detail::fits_in_ulong(s)) {
        assert(submul_ui_count() == 1);
        assert(submul_count() == 0);
    } else {
        assert(submul_ui_count() == 0);
        assert(submul_count() == 1);
    }

    mpz_clear(ref);
}

void check_alias_cases() {
    mpz_class a0("123456789012345678901234567890");
    mpz_class b("-98765432109876543210987654321");

    {
        mpz_t ref;
        mpz_init_set(ref, a0.get_mpz_t());
        mpz_addmul(ref, a0.get_mpz_t(), b.get_mpz_t());
        mpz_class a = a0;
        reset_counters();
        a += a * b;
        assert_equal(a, ref);
        assert(addmul_count() == 1);
        mpz_clear(ref);
    }
    {
        mpz_t ref;
        mpz_init_set(ref, a0.get_mpz_t());
        mpz_addmul(ref, b.get_mpz_t(), a0.get_mpz_t());
        mpz_class a = a0;
        reset_counters();
        a += b * a;
        assert_equal(a, ref);
        assert(addmul_count() == 1);
        mpz_clear(ref);
    }
    {
        mpz_t ref;
        mpz_init_set(ref, a0.get_mpz_t());
        mpz_submul(ref, a0.get_mpz_t(), b.get_mpz_t());
        mpz_class a = a0;
        reset_counters();
        a -= a * b;
        assert_equal(a, ref);
        assert(submul_count() == 1);
        mpz_clear(ref);
    }
    {
        mpz_t ref;
        mpz_init_set(ref, a0.get_mpz_t());
        mpz_submul(ref, b.get_mpz_t(), a0.get_mpz_t());
        mpz_class a = a0;
        reset_counters();
        a -= b * a;
        assert_equal(a, ref);
        assert(submul_count() == 1);
        mpz_clear(ref);
    }
    {
        mpz_t ref;
        mpz_init_set(ref, a0.get_mpz_t());
        mpz_addmul_ui(ref, a0.get_mpz_t(), 5UL);
        mpz_class a = a0;
        reset_counters();
        a += a * 5LL;
        assert_equal(a, ref);
        assert(addmul_ui_count() == 1);
        mpz_clear(ref);
    }
    {
        mpz_t ref;
        mpz_init_set(ref, a0.get_mpz_t());
        mpz_submul_ui(ref, a0.get_mpz_t(), 5UL);
        mpz_class a = a0;
        reset_counters();
        a -= 5LL * a;
        assert_equal(a, ref);
        assert(submul_ui_count() == 1);
        mpz_clear(ref);
    }
}

void check_non_fused() {
    mpz_class a0("12345");
    mpz_class b("23");
    mpz_class c("-31");
    mpz_class d("7");

    {
        mpz_class expected = a0;
        expected += b * c * d;
        mpz_class a = a0;
        reset_counters();
        a += b * c * d;
        assert(mpz_cmp(a.get_mpz_t(), expected.get_mpz_t()) == 0);
        assert(addmul_count() == 0);
        assert(submul_count() == 0);
        assert(addmul_ui_count() == 0);
        assert(submul_ui_count() == 0);
    }
    {
        mpz_class expected = a0;
        expected -= (b + c) * d;
        mpz_class a = a0;
        reset_counters();
        a -= (b + c) * d;
        assert(mpz_cmp(a.get_mpz_t(), expected.get_mpz_t()) == 0);
        assert(addmul_count() == 0);
        assert(submul_count() == 0);
    }
    {
        mpz_class expected = a0;
        expected += -b * c;
        mpz_class a = a0;
        reset_counters();
        a += -b * c;
        assert(mpz_cmp(a.get_mpz_t(), expected.get_mpz_t()) == 0);
        assert(addmul_count() == 0);
        assert(submul_count() == 0);
    }
    {
        mpz_class a = a0;
        reset_counters();
        a *= b * c;
        a /= b;
        a %= d;
        assert(addmul_count() == 0);
        assert(submul_count() == 0);
        assert(addmul_ui_count() == 0);
        assert(submul_ui_count() == 0);
    }
}

}  // namespace

int main() {
    mpz_class a("123456789012345678901234567890");
    mpz_class b("-98765432109876543210987654321");
    mpz_class c("1122334455667788990011223344556677889900");

    check_addmul(a, b, c);
    check_submul(a, b, c);

    for (std::int64_t s : {
             std::int64_t{0},
             std::int64_t{1},
             std::int64_t{-1},
             std::int64_t{5},
             std::int64_t{-5},
             std::numeric_limits<std::int64_t>::min(),
             std::numeric_limits<std::int64_t>::max(),
         }) {
        check_addmul_scalar(a, b, s, false);
        check_addmul_scalar(a, b, s, true);
        check_submul_scalar(a, b, s, false);
        check_submul_scalar(a, b, s, true);
    }

    for (std::uint64_t s : {
             std::uint64_t{0},
             std::uint64_t{1},
             std::uint64_t{5},
             std::numeric_limits<std::uint64_t>::max(),
         }) {
        check_addmul_u64(a, b, s, false);
        check_addmul_u64(a, b, s, true);
        check_submul_u64(a, b, s, false);
        check_submul_u64(a, b, s, true);
    }

    check_alias_cases();
    check_non_fused();
    return 0;
}
