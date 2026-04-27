// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <initializer_list>
#include <string>
#include <vector>

namespace {

std::uint64_t final_requested(std::initializer_list<mpf_class const*> xs) {
#if defined(GMPXX_MKII_NOPRECCHANGE)
    (void)xs;
    return gmpxx_mkII_detail::thread_default_prec();
#else
    std::uint64_t result = 0;
    for (mpf_class const* x : xs) {
        result = std::max(result, static_cast<std::uint64_t>(x->get_prec()));
    }
    return result;
#endif
}

void assert_same(char const* label, mpf_class const& got, mpf_class const& ref,
                 std::uint64_t requested) {
    if (mpf_cmp(got.get_mpf_t(), ref.get_mpf_t()) != 0) {
        std::cerr << "value mismatch: " << label << "\n";
        assert(false);
    }
    std::uint64_t actual_prec = static_cast<std::uint64_t>(got.get_prec());
    std::uint64_t expected_prec = gmpxx_mkII_detail::effective_mpf_prec(requested);
    if (actual_prec != expected_prec) {
        std::cerr << "precision mismatch: " << label << " requested=" << requested
                  << " actual=" << actual_prec << " expected=" << expected_prec
                  << "\n";
        assert(false);
    }
}

mpf_class ref_unary(mpf_class const& a, std::uint64_t requested, bool neg) {
    mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(requested));
    if (neg) {
        mpf_neg(ref.get_mpf_t(), a.get_mpf_t());
    } else {
        mpf_set(ref.get_mpf_t(), a.get_mpf_t());
    }
    return ref;
}

mpf_class ref_binary(mpf_class const& a, mpf_class const& b,
                     std::uint64_t requested, char op) {
    mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(requested));
    if (op == '+') {
        mpf_add(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
    } else if (op == '-') {
        mpf_sub(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
    } else if (op == '*') {
        mpf_mul(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
    } else {
        mpf_div(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
    }
    return ref;
}

void check_set(mpf_class const& a, mpf_class const& b,
               mpf_class const& c, mpf_class const& d) {
    std::uint64_t p2 = final_requested({&a, &b});
    assert_same("a+b", mpf_class(a + b), ref_binary(a, b, p2, '+'), p2);
    assert_same("a-b", mpf_class(a - b), ref_binary(a, b, p2, '-'), p2);
    assert_same("a*b", mpf_class(a * b), ref_binary(a, b, p2, '*'), p2);
    assert_same("a/b", mpf_class(a / b), ref_binary(a, b, p2, '/'), p2);
    assert_same("-a", mpf_class(-a), ref_unary(a, final_requested({&a}), true),
                final_requested({&a}));
    assert_same("+a", mpf_class(+a), ref_unary(a, final_requested({&a}), false),
                final_requested({&a}));

    std::uint64_t p3 = final_requested({&a, &b, &c});
    {
        mpf_class tmp(gmpxx_mkII_detail::checked_mp_bitcnt(p3));
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p3));
        mpf_add(tmp.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_mul(ref.get_mpf_t(), tmp.get_mpf_t(), c.get_mpf_t());
        assert_same("(a+b)*c", mpf_class((a + b) * c), ref, p3);
    }
    {
        mpf_class tmp(gmpxx_mkII_detail::checked_mp_bitcnt(p3));
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p3));
        mpf_sub(tmp.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_div(ref.get_mpf_t(), tmp.get_mpf_t(), c.get_mpf_t());
        assert_same("(a-b)/c", mpf_class((a - b) / c), ref, p3);
    }
    {
        mpf_class tmp(gmpxx_mkII_detail::checked_mp_bitcnt(p3));
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p3));
        mpf_sub(tmp.get_mpf_t(), b.get_mpf_t(), c.get_mpf_t());
        mpf_add(ref.get_mpf_t(), a.get_mpf_t(), tmp.get_mpf_t());
        assert_same("a+(b-c)", mpf_class(a + (b - c)), ref, p3);
    }

    std::uint64_t p4 = final_requested({&a, &b, &c, &d});
    {
        mpf_class left(gmpxx_mkII_detail::checked_mp_bitcnt(p4));
        mpf_class right(gmpxx_mkII_detail::checked_mp_bitcnt(p4));
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p4));
        mpf_add(left.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_sub(right.get_mpf_t(), c.get_mpf_t(), d.get_mpf_t());
        mpf_mul(ref.get_mpf_t(), left.get_mpf_t(), right.get_mpf_t());
        assert_same("(a+b)*(c-d)", mpf_class((a + b) * (c - d)), ref, p4);
    }
    {
        mpf_class tmp(gmpxx_mkII_detail::checked_mp_bitcnt(p4));
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p4));
        mpf_mul(tmp.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_add(tmp.get_mpf_t(), tmp.get_mpf_t(), c.get_mpf_t());
        mpf_div(ref.get_mpf_t(), tmp.get_mpf_t(), d.get_mpf_t());
        assert_same("((a*b)+c)/d", mpf_class(((a * b) + c) / d), ref, p4);
    }
    {
        std::uint64_t p1 = final_requested({&a});
        mpf_class tmp(gmpxx_mkII_detail::checked_mp_bitcnt(p1));
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p1));
        mpf_neg(tmp.get_mpf_t(), a.get_mpf_t());
        mpf_neg(ref.get_mpf_t(), tmp.get_mpf_t());
        assert_same("-(-a)", mpf_class(-(-a)), ref, p1);
    }
}

}  // namespace

int main() {
    std::vector<mp_bitcnt_t> precs{64, 256, 1024};
    for (mp_bitcnt_t p : precs) {
        mpf_class zero("0", p);
        mpf_class one("1", p);
        mpf_class neg_one("-1", p);
        mpf_class pi("3.1415926535897932384626433832795028841971", p);
        check_set(pi, one, neg_one, mpf_class("2.5", p));
        check_set(one, pi, mpf_class("1e-100", p), mpf_class("7.25", p));
        check_set(mpf_class(1.25, p), mpf_class("-2.75", p),
                  mpf_class("1e+100", p), mpf_class("9.5", p));
        check_set(zero, one, pi, mpf_class("4", p));
    }

    check_set(mpf_class("3.5", 64), mpf_class("-7.25", 256),
              mpf_class("1e-50", 1024), mpf_class("11.125", 256));
    return 0;
}
