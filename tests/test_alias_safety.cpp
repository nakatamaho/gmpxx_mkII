// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cassert>
#include <cstdint>

namespace {

void assert_equal(mpf_class const& a, mpf_class const& b,
                  mp_bitcnt_t expected_prec) {
    assert(mpf_cmp(a.get_mpf_t(), b.get_mpf_t()) == 0);
    assert(a.get_prec() == expected_prec);
}

}  // namespace

int main() {
    {
        mpf_class a("1.5", 256);
        mpf_class b("2.5", 256);
        mp_bitcnt_t p = a.get_prec();
        mpf_class ref(gmpxx_detail::checked_mp_bitcnt(p));
        mpf_add(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        a = a + b;
        assert_equal(a, ref, p);
    }
    {
        mpf_class a("3.75", 256);
        mp_bitcnt_t p = a.get_prec();
        mpf_class ref(gmpxx_detail::checked_mp_bitcnt(p));
        mpf_sub(ref.get_mpf_t(), a.get_mpf_t(), a.get_mpf_t());
        a = a - a;
        assert_equal(a, ref, p);
    }
    {
        mpf_class a("3.75", 256);
        mp_bitcnt_t p = a.get_prec();
        mpf_class ref(gmpxx_detail::checked_mp_bitcnt(p));
        mpf_mul(ref.get_mpf_t(), a.get_mpf_t(), a.get_mpf_t());
        a = a * a;
        assert_equal(a, ref, p);
    }
    {
        mpf_class a("3.75", 256);
        mp_bitcnt_t p = a.get_prec();
        mpf_class ref(gmpxx_detail::checked_mp_bitcnt(p));
        mpf_div(ref.get_mpf_t(), a.get_mpf_t(), a.get_mpf_t());
        a = a / a;
        assert_equal(a, ref, p);
    }
    {
        mpf_class a("1.5", 256);
        mpf_class b("2.5", 256);
        mp_bitcnt_t p = a.get_prec();
        mpf_class tmp(gmpxx_detail::checked_mp_bitcnt(p));
        mpf_class ref(gmpxx_detail::checked_mp_bitcnt(p));
        mpf_add(tmp.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_mul(ref.get_mpf_t(), tmp.get_mpf_t(), a.get_mpf_t());
        a = (a + b) * a;
        assert_equal(a, ref, p);
    }
    {
        mpf_class a("7.5", 256);
        mpf_class b("2.5", 256);
        mp_bitcnt_t p = a.get_prec();
        mpf_class lhs(gmpxx_detail::checked_mp_bitcnt(p));
        mpf_class rhs(gmpxx_detail::checked_mp_bitcnt(p));
        mpf_class ref(gmpxx_detail::checked_mp_bitcnt(p));
        mpf_sub(lhs.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_add(rhs.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_div(ref.get_mpf_t(), lhs.get_mpf_t(), rhs.get_mpf_t());
        a = (a - b) / (a + b);
        assert_equal(a, ref, p);
    }
    {
        mpf_class a("1.5", 64);
        mpf_class b("2.5", 1024);
        mp_bitcnt_t p = a.get_prec();
        mpf_class tmp(gmpxx_detail::checked_mp_bitcnt(p));
        mpf_class ref(gmpxx_detail::checked_mp_bitcnt(p));
        mpf_add(tmp.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_mul(ref.get_mpf_t(), tmp.get_mpf_t(), a.get_mpf_t());
        a = (a + b) * a;
        assert_equal(a, ref, p);
    }
    return 0;
}
