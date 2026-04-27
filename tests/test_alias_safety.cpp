// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <initializer_list>

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

void assert_equal(mpf_class const& a, mpf_class const& b) {
    assert(mpf_cmp(a.get_mpf_t(), b.get_mpf_t()) == 0);
}

}  // namespace

int main() {
    {
        mpf_class a("1.5", 256);
        mpf_class b("2.5", 256);
        std::uint64_t p = final_requested({&a, &b});
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p));
        mpf_add(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        a = a + b;
        assert_equal(a, ref);
    }
    {
        mpf_class a("3.75", 256);
        std::uint64_t p = final_requested({&a});
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p));
        mpf_sub(ref.get_mpf_t(), a.get_mpf_t(), a.get_mpf_t());
        a = a - a;
        assert_equal(a, ref);
    }
    {
        mpf_class a("3.75", 256);
        std::uint64_t p = final_requested({&a});
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p));
        mpf_mul(ref.get_mpf_t(), a.get_mpf_t(), a.get_mpf_t());
        a = a * a;
        assert_equal(a, ref);
    }
    {
        mpf_class a("3.75", 256);
        std::uint64_t p = final_requested({&a});
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p));
        mpf_div(ref.get_mpf_t(), a.get_mpf_t(), a.get_mpf_t());
        a = a / a;
        assert_equal(a, ref);
    }
    {
        mpf_class a("1.5", 256);
        mpf_class b("2.5", 256);
        std::uint64_t p = final_requested({&a, &b});
        mpf_class tmp(gmpxx_mkII_detail::checked_mp_bitcnt(p));
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p));
        mpf_add(tmp.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_mul(ref.get_mpf_t(), tmp.get_mpf_t(), a.get_mpf_t());
        a = (a + b) * a;
        assert_equal(a, ref);
    }
    {
        mpf_class a("7.5", 256);
        mpf_class b("2.5", 256);
        std::uint64_t p = final_requested({&a, &b});
        mpf_class lhs(gmpxx_mkII_detail::checked_mp_bitcnt(p));
        mpf_class rhs(gmpxx_mkII_detail::checked_mp_bitcnt(p));
        mpf_class ref(gmpxx_mkII_detail::checked_mp_bitcnt(p));
        mpf_sub(lhs.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_add(rhs.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_div(ref.get_mpf_t(), lhs.get_mpf_t(), rhs.get_mpf_t());
        a = (a - b) / (a + b);
        assert_equal(a, ref);
    }
    return 0;
}
